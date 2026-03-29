#pragma once
#include <iostream>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <memory>
#include <vector>
#include "../base/rect.h"
#include "../base/matrix.h"

struct setBackground {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct drawVideoFrame {
    // YUV path
    std::shared_ptr<std::vector<uint8_t>> yPlane;
    std::shared_ptr<std::vector<uint8_t>> uPlane;
    std::shared_ptr<std::vector<uint8_t>> vPlane;
    int yStride;
    int uStride;
    int vStride;

    int videoFrameWidth;
    int videoFrameHeight;
    int x;
    int y;
    bool useSmoothing;
};

struct rendererInstruction : setBackground, drawVideoFrame {
    int instructionCode;
};

void render(RECT frameSize, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv);
void pushRendererInstruction(rendererInstruction instruction, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv);