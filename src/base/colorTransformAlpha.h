#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

struct CXFORMWITHALPHA {
    bool HasAddTerms;
    bool HasMultTerms;
    uint8_t nBits;
    int RedMultTerm;
    int GreenMultTerm;
    int BlueMultTerm;
    int AlphaMultTerm;
    int RedAddTerm;
    int GreenAddTerm;
    int BlueAddTerm;
    int AlphaAddTerm;
};

CXFORMWITHALPHA getColorTransformWithAlpha(std::vector<uint8_t>& data);