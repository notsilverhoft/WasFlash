#pragma once
#include <vector>
#include <cstdint>

bool initializeAudioPlayer(uint8_t streamSoundRate, uint8_t streamSoundType, uint8_t streamSoundSize, uint8_t streamSoundCompression);
void renderAudioFrame(const std::vector<uint8_t>& pcmData);
void releaseAudioPlayer();