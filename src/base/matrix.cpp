#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>
#include <cstring>
#include "matrix.h"
#include "../utils/bitStream.hpp"
#include "../utils/trackSWF.h"

MATRIX getMatrix(std::vector<uint8_t>& data) {

    MATRIX binOut;

    uint8_t matrixBuffer[32] = {0};

    size_t bytesToCopy = std::min((size_t)32, data.size());
    for (size_t i = 0; i < bytesToCopy; i++) {
        matrixBuffer[i] = data[i];
    }

    BitStream bs(matrixBuffer);
    int totalBits = 0;

    binOut.HasScale = bs.readUnsigned(1);
    totalBits += 1;

    if ( binOut.HasScale ) {

        binOut.nScaleBits = bs.readUnsigned(5);
        binOut.ScaleX = static_cast<float>(bs.readSigned(binOut.nScaleBits) / 65536.0f);
        binOut.ScaleY = static_cast<float>(bs.readSigned(binOut.nScaleBits) / 65536.0f);
        totalBits += (5 + (2 * binOut.nScaleBits));

    } else {

        binOut.ScaleX = 1;
        binOut.ScaleY = 1;

    }

    binOut.HasRotate = bs.readUnsigned(1);
    totalBits += 1;

    if ( binOut.HasRotate ) {

        binOut.nRotateBits = bs.readUnsigned(5);
        binOut.RotateSkew0 = static_cast<float>(bs.readSigned(binOut.nRotateBits) / 65536.0f);
        binOut.RotateSkew1 = static_cast<float>(bs.readSigned(binOut.nRotateBits) / 65536.0f);
        totalBits += (5 + (2 * binOut.nRotateBits));

    }

    binOut.nTranslateBits = bs.readUnsigned(5);
    binOut.TranslateX = static_cast<int>(bs.readSigned(binOut.nTranslateBits));
    binOut.TranslateY = static_cast<int>(bs.readSigned(binOut.nTranslateBits));
    totalBits += (5 + (2 * binOut.nTranslateBits));

    SWFShift(data, ((totalBits + 7) / 8));

    return binOut;

}

MATRIX combineMatrix(const MATRIX& parent, const MATRIX& child) {

    MATRIX binOut;

    binOut.ScaleX = parent.ScaleX * child.ScaleX + parent.RotateSkew1 * child.RotateSkew0;
    binOut.RotateSkew0 = parent.RotateSkew0 * child.ScaleX  + parent.ScaleY * child.RotateSkew0;
    binOut.RotateSkew1 = parent.ScaleX * child.RotateSkew1  + parent.RotateSkew1 * child.ScaleY;
    binOut.ScaleY = parent.RotateSkew0 * child.RotateSkew1 + parent.ScaleY * child.ScaleY;

    binOut.TranslateX = (int)std::lround(parent.ScaleX * child.TranslateX + parent.RotateSkew1 * child.TranslateY + parent.TranslateX);
    binOut.TranslateY = (int)std::lround(parent.RotateSkew0 * child.TranslateX + parent.ScaleY * child.TranslateY + parent.TranslateY);

    return binOut;

}