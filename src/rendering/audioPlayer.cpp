#include <iostream>
#ifdef __EMSCRIPTEN__
#define MA_ENABLE_WEBAUDIO
#endif
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "audioProcessor.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <deque>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/threading.h>
#endif

static ma_device audio_device;
static std::deque<std::vector<uint8_t>> audio_queue;
static std::mutex audio_mutex;
static std::vector<uint8_t> audio_remainder;
static bool audio_device_initialized = false;

// Forward declaration so initAudioOnMainThread can reference it
static void audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

#ifdef __EMSCRIPTEN__
static ma_context audio_context;

struct AudioInitParams {
    ma_format format;
    uint32_t channels;
    uint32_t sampleRate;
    bool success;
};

static void initAudioOnMainThread(void* arg) {

    AudioInitParams* p = (AudioInitParams*)arg;

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = p->format;
    config.playback.channels = p->channels;
    config.sampleRate        = p->sampleRate;
    config.dataCallback      = audioCallback;

    if (ma_context_init(NULL, 0, NULL, &audio_context) != MA_SUCCESS) {
        std::cout << "audioPlayer: ma_context_init failed\n";
        p->success = false;
        return;
    }

    if (ma_device_init(&audio_context, &config, &audio_device) != MA_SUCCESS) {
        std::cout << "audioPlayer: ma_device_init failed\n";
        p->success = false;
        return;
    }

    if (ma_device_start(&audio_device) != MA_SUCCESS) {
        std::cout << "audioPlayer: ma_device_start failed\n";
        p->success = false;
        return;
    }

    p->success = true;

}
#endif

static void audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {

    size_t bytesNeeded = frameCount * pDevice->playback.channels * ma_get_bytes_per_sample(pDevice->playback.format);
    size_t bytesFilled = 0;
    uint8_t* out = (uint8_t*)pOutput;

    if (!audio_remainder.empty()) {
        size_t take = std::min(audio_remainder.size(), bytesNeeded);
        memcpy(out, audio_remainder.data(), take);
        bytesFilled += take;
        audio_remainder.erase(audio_remainder.begin(), audio_remainder.begin() + take);
    }

    std::lock_guard<std::mutex> lock(audio_mutex);

    while (bytesFilled < bytesNeeded && !audio_queue.empty()) {
        auto& chunk = audio_queue.front();
        size_t take = std::min(chunk.size(), bytesNeeded - bytesFilled);
        memcpy(out + bytesFilled, chunk.data(), take);
        bytesFilled += take;
        if (take < chunk.size())
            audio_remainder.assign(chunk.begin() + take, chunk.end());
        audio_queue.pop_front();
    }

    if (bytesFilled < bytesNeeded)
        memset(out + bytesFilled, 0, bytesNeeded - bytesFilled);

}

void releaseAudioPlayer() {

    if (audio_device_initialized) {
        ma_device_uninit(&audio_device);
        audio_device_initialized = false;
    }

#ifdef __EMSCRIPTEN__
    ma_context_uninit(&audio_context);
#endif

    audio_queue.clear();
    audio_remainder.clear();

}

bool initializeAudioPlayer(uint8_t streamSoundRate, uint8_t streamSoundType, uint8_t streamSoundSize, uint8_t streamSoundCompression) {

    releaseAudioPlayer();

    std::cout << "initializeAudioPlayer called rate=" << (int)streamSoundRate
              << " type=" << (int)streamSoundType
              << " compression=" << (int)streamSoundCompression << "\n";

    ma_format format = (streamSoundCompression == 0) ? ma_format_u8 : ma_format_s16;

#ifdef __EMSCRIPTEN__
    AudioInitParams params;
    params.format     = format;
    params.channels   = streamSoundType + 1;
    params.sampleRate = mapSwfSampleRate(streamSoundRate);
    params.success    = false;

    emscripten_sync_run_in_main_runtime_thread(EM_FUNC_SIG_VI, initAudioOnMainThread, &params);

    if (!params.success) {
        std::cout << "audioPlayer: init failed on main thread\n";
        return false;
    }
#else
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = format;
    config.playback.channels = streamSoundType + 1;
    config.sampleRate        = mapSwfSampleRate(streamSoundRate);
    config.dataCallback      = audioCallback;

    if (ma_device_init(NULL, &config, &audio_device) != MA_SUCCESS) {
        std::cout << "audioPlayer: ma_device_init failed\n";
        return false;
    }

    if (ma_device_start(&audio_device) != MA_SUCCESS) {
        std::cout << "audioPlayer: ma_device_start failed\n";
        return false;
    }
#endif

    audio_device_initialized = true;
    std::cout << "audioPlayer: initialized successfully\n";
    return true;

}

void renderAudioFrame(const std::vector<uint8_t>& pcmData) {
    std::lock_guard<std::mutex> lock(audio_mutex);
    audio_queue.push_back(pcmData);
}