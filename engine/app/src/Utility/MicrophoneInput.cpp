#ifdef ENABLE_VOICE_CHAT
#include "MicrophoneInput.hpp"
#include "portaudio.h"
#include "standardout.hpp"
#include "aya/rbxTime.hpp"

namespace Aya
{

PaStream* stream;
bool initialized = false;
OpusEncoder* encoder;

void MicrophoneInput::initialize()
{
    // Open microphone input using PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        Aya::StandardOut::singleton()->printf(MESSAGE_ERROR, "Error initializing PortAudio (%d): '%s'", err, Pa_GetErrorText(err));
        return;
    }

    Aya::StandardOut::singleton()->printf(MESSAGE_SENSITIVE, "PortAudio initialized");

    // first see if we even have a default input device.
    PaDeviceIndex device = Pa_GetDefaultInputDevice();
    if (device == paNoDevice)
    {
        Aya::StandardOut::singleton()->printf(MESSAGE_ERROR, "No default input device found when initializing PortAudio.");
        Pa_Terminate();
        return;
    }

    Aya::StandardOut::singleton()->printf(MESSAGE_SENSITIVE, "Default input device: %d", device);

    const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
    if (info != NULL)
    {
        Aya::StandardOut::singleton()->printf(
            MESSAGE_SENSITIVE, "Default input device: %s, max input channels: %d", info->name, info->maxInputChannels);
    }

    Aya::StandardOut::singleton()->printf(MESSAGE_SENSITIVE, "Opening default PortAudio stream");

    err = Pa_OpenDefaultStream(&stream, OPUS_VOICE_CHANNELS, OPUS_VOICE_CHANNELS, paInt16, OPUS_VOICE_SAMPLERATE, OPUS_VOICE_BUFSIZE, NULL, NULL);
    if (err != paNoError)
    {
        Aya::StandardOut::singleton()->printf(MESSAGE_ERROR, "Error opening default PortAudio stream (%d): '%s'", err, Pa_GetErrorText(err));
        Pa_Terminate();
        return;
    }

    Aya::StandardOut::singleton()->printf(MESSAGE_SENSITIVE, "Starting default PortAudio stream");

    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        Aya::StandardOut::singleton()->printf(MESSAGE_ERROR, "Error starting PortAudio stream (%d): '%s'", err, Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return;
    }

    Aya::StandardOut::singleton()->printf(MESSAGE_SENSITIVE, "PortAudio stream started");

    // Now, create our Opus encoder/decoder
    encoder = opus_encoder_create(OPUS_VOICE_SAMPLERATE, OPUS_VOICE_CHANNELS, OPUS_APPLICATION_VOIP, NULL);

    Aya::StandardOut::singleton()->printf(MESSAGE_SENSITIVE, "Opus encoder created");

    initialized = true;
}

void MicrophoneInput::shutdown()
{
    if (initialized)
    {
        // shutdown mic input
        Pa_AbortStream(stream);
        Pa_Terminate();

        // destroy opus encoder
        opus_encoder_destroy(encoder);

        initialized = false;
    }
}

std::tuple<unsigned char*, int> MicrophoneInput::getData()
{
    size_t streamAvailable = Pa_GetStreamReadAvailable(stream);
    if(!streamAvailable) // minimum samples
        return std::make_tuple(nullptr, 0);
    
    opus_int16* streamData = new opus_int16[OPUS_VOICE_BUFSIZE]; // will read bufsize samples into buffer
    Pa_ReadStream(stream, streamData, OPUS_VOICE_BUFSIZE * OPUS_VOICE_CHANNELS);

    unsigned char* opusData = new unsigned char[OPUS_VOICE_BUFSIZE * OPUS_VOICE_CHANNELS];
    int bytes = opus_encode(encoder, streamData, OPUS_VOICE_BUFSIZE * OPUS_VOICE_CHANNELS, opusData, OPUS_VOICE_BUFSIZE * OPUS_VOICE_CHANNELS);

    if (bytes < 0)
    {
        Aya::StandardOut::singleton()->printf(MESSAGE_ERROR, "Unable to encode voice packet (%i) (error: '%s')", bytes, opus_strerror(bytes));
    }
    else
    {
        // Aya::StandardOut::singleton()->printf(MESSAGE_SENSITIVE, "Encoded %d bytes of voice data", bytes);
        return std::make_tuple(opusData, bytes);
    }

    return std::make_tuple(nullptr, 0);
}

bool MicrophoneInput::isInitialized()
{
    return initialized;
}

} // namespace Aya
#endif