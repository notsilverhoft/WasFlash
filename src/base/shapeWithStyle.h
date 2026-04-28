#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "matrix.h"
#include "gradient.h"
#include "shapeRecord.h"


struct SHAPEWITHSTYLE {
    FILLSTYLEARRAY FillStyles;
    LINESTYLEARRAY LineStyles;
    uint8_t NumFillBits;
    uint8_t NumLineBits;
    std::vector<SHAPERECORD> ShapeRecords;
};

SHAPEWITHSTYLE getShapeWithStyle(std::vector<uint8_t>& data, int shapeVersion);