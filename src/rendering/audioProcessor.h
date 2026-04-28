#pragma once
#include <vector>
#include <cstdint>

int mapSwfSampleRate(uint8_t rate);
bool initializeAudioDecoder(uint8_t streamSoundCompression, uint8_t streamSoundRate, uint8_t streamSoundSize, uint8_t streamSoundType, int16_t latencySeek);
std::vector<uint8_t> decodeAudioFrame(const std::vector<uint8_t>& data);