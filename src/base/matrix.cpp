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

    uint8_t matrixBuffer[32] = {0};  // Zero-initialize

    // Only copy available bytes to prevent buffer overflow
    size_t bytesToCopy = std::min((size_t)32, data.size());
    for (size_t i = 0; i < bytesToCopy; i++) {
        matrixBuffer[i] = data[i];
    }

    BitStream bs(matrixBuffer);

    int totalBits = 0;

    // Parsing:

    binOut.HasScale = bs.readUnsigned(1);

    totalBits += 1;
    
    if ( binOut.HasScale ) {

        binOut.nScaleBits = bs.readUnsigned(5);

        binOut.ScaleX = static_cast<float>(bs.readUnsigned(binOut.nScaleBits) / 65536.0f);
        binOut.ScaleY = static_cast<float>(bs.readUnsigned(binOut.nScaleBits) / 65536.0f);
    
        totalBits += (5 + (2 * binOut.nScaleBits));

    }

    binOut.HasRotate = bs.readUnsigned(1);

    totalBits += 1;

    if ( binOut.HasRotate ) {

        binOut.nRotateBits = bs.readUnsigned(5);

        binOut.RotateSkew0 = static_cast<float>(bs.readUnsigned(binOut.nRotateBits) / 65536.0f);
        binOut.RotateSkew1 = static_cast<float>(bs.readUnsigned(binOut.nRotateBits) / 65536.0f);
    
        totalBits += (5 + (2 * binOut.nRotateBits));

    }

    binOut.nTranslateBits = bs.readUnsigned(5);

    binOut.TranslateX = static_cast<int>(bs.readSigned(binOut.nTranslateBits) / 65536.0f);
    binOut.TranslateY = static_cast<int>(bs.readSigned(binOut.nTranslateBits) / 65536.0f);

    totalBits += (5 + (2 * binOut.nTranslateBits));

    SWFShift(data, ((totalBits + 7) / 8));

    return binOut;

}