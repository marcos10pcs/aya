#ifdef ENABLE_VOICE_CHAT
#pragma once

#include "Tree/Instance.hpp"
#include <opus/opus.h>

namespace FMOD
{
class Channel;
class Sound;
}

namespace Aya {
extern const char* const sVoiceChatOutput;

// received from Players
typedef std::pair<int, unsigned char*> VoiceChatOpusData;
// decoded from above and played
typedef std::pair<int, opus_int16*> VoiceChatPcm;

class VoiceChatOutput : public DescribedCreatable<VoiceChatOutput, Instance, sVoiceChatOutput> {
  FMOD::Channel* fmodChannel;
  FMOD::Sound* fmodSound;
  OpusDecoder* opusDecoder;
  std::deque<VoiceChatPcm> pendingPcmData;
  bool initialized;

public:
  VoiceChatOutput();

  bool isInitialized() { return initialized; }
  void load(const Aya::Instance* context);

  int dataPending() { return pendingPcmData.size(); }
  VoiceChatPcm getData();
  void addData(VoiceChatOpusData data);

  std::mutex pcmMutex;
};
}
#endif