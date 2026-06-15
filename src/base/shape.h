#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "shapeRecord.h"

struct SHAPE {
    uint8_t NumFillBits;
    uint8_t NumLineBits;
    std::vector<SHAPERECORD> ShapeRecords;
};

SHAPE getSHAPE(std::vector<uint8_t>& data, bool version);