#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>
#include <cstring>
#include "colorTransformAlpha.h"
#include "../utils/bitStream.hpp"
#include "../utils/trackSWF.h"
 
CXFORMWITHALPHA getColorTransformWithAlpha(std::vector<uint8_t>& data) {
 
    CXFORMWITHALPHA binOut;
 
    // CXFORMWITHALPHA is variable length — cap copy to available data,
    // zero-fill remainder so BitStream reads clean garbage-free bits.
    uint8_t CXBuffer[16] = {};
    int copySize = std::min((int)data.size(), 16);
    for (int i = 0; i < copySize; i++) {
        CXBuffer[i] = data[i];
    }
 
    BitStream bs(CXBuffer);
    int totalBits = 0;
 
    // Parsing:
    binOut.HasAddTerms  = bs.readUnsigned(1);
    binOut.HasMultTerms = bs.readUnsigned(1);
    binOut.nBits        = bs.readUnsigned(4);
    totalBits += 6;
 
    if ( binOut.HasMultTerms ) {
        binOut.RedMultTerm   = bs.readSigned(binOut.nBits);
        binOut.GreenMultTerm = bs.readSigned(binOut.nBits);
        binOut.BlueMultTerm  = bs.readSigned(binOut.nBits);
        binOut.AlphaMultTerm = bs.readSigned(binOut.nBits);
        totalBits += (4 * binOut.nBits);
    }
 
    if ( binOut.HasAddTerms ) {
        binOut.RedAddTerm   = bs.readSigned(binOut.nBits);
        binOut.GreenAddTerm = bs.readSigned(binOut.nBits);
        binOut.BlueAddTerm  = bs.readSigned(binOut.nBits);
        binOut.AlphaAddTerm = bs.readSigned(binOut.nBits);
        totalBits += (4 * binOut.nBits);
    }
 
    SWFShift(data, ((totalBits + 7) / 8));
    return binOut;
 
}
