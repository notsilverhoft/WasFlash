#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

struct CXFORMWITHALPHA {
    bool HasAddTerms;
    bool HasMultTerms;
    uint8_t nBits;
    int RedMultTerm = 256;
    int GreenMultTerm = 256;
    int BlueMultTerm = 256;
    int AlphaMultTerm = 256;
    int RedAddTerm = 0;
    int GreenAddTerm = 0;
    int BlueAddTerm = 0;
    int AlphaAddTerm = 0;
};

CXFORMWITHALPHA getColorTransformWithAlpha(std::vector<uint8_t>& data, int version);
CXFORMWITHALPHA combineColorTransform(const CXFORMWITHALPHA& parent, const CXFORMWITHALPHA& child);