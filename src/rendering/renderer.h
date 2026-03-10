#pragma once
#include <iostream>
#include <cstdint>
#include <atomic>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include "../base/rect.h"

struct setBackground {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct rendererInstruction : setBackground {
    int instructionCode;
};

void render(RECT frameSize, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv);

void pushRendererInstruction(rendererInstruction instruction, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv);