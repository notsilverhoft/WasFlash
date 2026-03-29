#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

struct MATRIX {
    bool HasScale;
    uint8_t nScaleBits;
    float ScaleX = 0;
    float ScaleY = 0;
    bool HasRotate;
    uint8_t nRotateBits;
    float RotateSkew0 = 0;
    float RotateSkew1 = 0;
    uint8_t nTranslateBits;
    int TranslateX = 0;
    int TranslateY = 0;
};

MATRIX getMatrix(std::vector<uint8_t>& data);