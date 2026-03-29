#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <unordered_map>
#include <chrono>
#include <memory>
#ifdef __cplusplus
extern "C" {
#endif
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif
#include "renderer.h"

struct YUVFrame {
    std::shared_ptr<std::vector<uint8_t>> yPlane;
    std::shared_ptr<std::vector<uint8_t>> uPlane;
    std::shared_ptr<std::vector<uint8_t>> vPlane;
    int yStride, uStride, vStride;
    int width, height;
};

rendererInstruction getVideoInstruction(int width, int height, int xPos, int yPos, bool useSmoothing,
    std::shared_ptr<std::vector<uint8_t>> yPlane,
    std::shared_ptr<std::vector<uint8_t>> uPlane,
    std::shared_ptr<std::vector<uint8_t>> vPlane,
    int yStride, int uStride, int vStride);
bool initializeVideoDecoder(uint8_t swfCodecId, int width, int height);
YUVFrame decodeFrame(const std::vector<uint8_t>& data);

