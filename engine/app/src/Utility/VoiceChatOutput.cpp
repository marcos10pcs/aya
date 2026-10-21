#ifdef ENABLE_VOICE_CHAT
#include "VoiceChatOutput.hpp"
#include "Tree/Service.hpp"
#include "Utility/VoiceChatOutput.hpp"
#include "Utility/SoundService.hpp"

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"

namespace Aya
{
const char* const sVoiceChatOutput = "VoiceChatOutput";

VoiceChatOutput::VoiceChatOutput() :
  fmodSound(0),
  fmodChannel(0),
  opusDecoder(0),
  initialized(false),
  DescribedCreatable<VoiceChatOutput, Instance, sVoiceChatOutput>() {
  
}

FMOD_RESULT F_CALLBACK __voiceChatCallback(FMOD_SOUND* _sound, void* _data, unsigned int datalen) {
  VoiceChatOutput* src;
  FMOD::Sound* sound = (FMOD::Sound*)_sound;
  opus_int16* data = (opus_int16*)_data;
  sound->getUserData((void**)&src);

  int bytesToAdd = datalen;
  int bytesRead = 0;
  while(bytesToAdd > 0) {
    if(src->dataPending()) {
      VoiceChatPcm pk = src->getData();
      if(pk.second && pk.first) {
        printf("Got %i:%p from deque (remaining size: %i, bytes to read: %i, bytes read: %i)\n", pk.first, pk.second, src->dataPending(), bytesToAdd, bytesRead);
        memcpy(data + bytesRead, pk.second, std::min(pk.first, bytesToAdd));
        bytesToAdd -= pk.first;
        bytesRead += pk.first;
      }
    } else {
      return (FMOD_RESULT)F_OK;
    }
  }
  return (FMOD_RESULT)F_OK;
}

void VoiceChatOutput::load(const Aya::Instance* context) {
  if(initialized)
    return;
  initialized = true;

  Soundscape::SoundService* soundService = ServiceProvider::create<Soundscape::SoundService>(context);
  FMOD::System* system = soundService->getSystem();

  opusDecoder = opus_decoder_create(OPUS_VOICE_SAMPLERATE, OPUS_VOICE_CHANNELS, NULL);

  FMOD_CREATESOUNDEXINFO exinfo;
  memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
  exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
  exinfo.numchannels = 1;  // mono
  exinfo.format = FMOD_SOUND_FORMAT_PCM16;
  exinfo.defaultfrequency = OPUS_VOICE_SAMPLERATE;  // sample rate
  exinfo.length = OPUS_VOICE_BUFSIZE * sizeof(opus_int16);
  exinfo.pcmreadcallback = __voiceChatCallback;
  exinfo.userdata = this;
  system->createSound(NULL, FMOD_LOOP_NORMAL | FMOD_CREATESTREAM | FMOD_OPENUSER, &exinfo, &fmodSound);
  system->playSound(fmodSound, NULL, false, &fmodChannel);

  fmodSound->setUserData(this);
  printf("HLLO I AM %p\n", this);
}

VoiceChatPcm VoiceChatOutput::getData() {
  VoiceChatPcm p = pendingPcmData[0];
  pendingPcmData.pop_front();
  return p;
}

void VoiceChatOutput::addData(VoiceChatOpusData od) {
  if(!opusDecoder)
    return;

  opus_int16* data = new opus_int16[OPUS_VOICE_BUFSIZE];
  int samps_decoded = opus_decode(opusDecoder, od.second, od.first, data, OPUS_VOICE_BUFSIZE, 1);
  printf("Added %i:%p to pcm deque\n", samps_decoded * sizeof(opus_int16), data);
  pendingPcmData.push_back(VoiceChatPcm(samps_decoded * sizeof(opus_int16), data)); 

  delete[] od.second;
}
}
#endif