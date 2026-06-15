#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "shape.h"
#include "matrix.h"
#include "gradient.h"
#include "shapeRecord.h"
#include "../utils/bitStream.hpp"

SHAPE getSHAPE(std::vector<uint8_t>& data, bool version) {

    SHAPE binOut;

    binOut.NumFillBits = (data[0] >> 4);
    binOut.NumLineBits = (data[0] & 0x0F);
    // std::cout << "Shape: NumFillBits: " << binOut.NumFillBits << "\n";
    // std::cout << "Shape: NumLineBits: " << binOut.NumLineBits << "\n";
    SWFShift(data, 1);

    if ( version ) {

        binOut.ShapeRecords = getShapeRecords(data, 4, binOut.NumFillBits, binOut.NumLineBits);

    }

    else {

        binOut.ShapeRecords = getShapeRecords(data, 3, binOut.NumFillBits, binOut.NumLineBits);

    }

    return binOut;

}