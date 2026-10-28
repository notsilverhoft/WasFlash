#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>
#include <cstring>
#include "colorTransformAlpha.h"
#include "../utils/bitStream.hpp"
#include "../utils/trackSWF.h"
 
CXFORMWITHALPHA getColorTransformWithAlpha(std::vector<uint8_t>& data, int version) {
 
    CXFORMWITHALPHA binOut;


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
 
    if ( binOut.HasMultTerms && version == 2 ) {
        binOut.RedMultTerm   = bs.readSigned(binOut.nBits);
        binOut.GreenMultTerm = bs.readSigned(binOut.nBits);
        binOut.BlueMultTerm  = bs.readSigned(binOut.nBits);
        binOut.AlphaMultTerm = bs.readSigned(binOut.nBits);
        totalBits += (4 * binOut.nBits);
    }

    if ( binOut.HasMultTerms && version == 1 ) {
        binOut.RedMultTerm   = bs.readSigned(binOut.nBits);
        binOut.GreenMultTerm = bs.readSigned(binOut.nBits);
        binOut.BlueMultTerm  = bs.readSigned(binOut.nBits);
        binOut.AlphaMultTerm = 256;
        totalBits += (3 * binOut.nBits);
    }
 
    if ( binOut.HasAddTerms && version == 2 ) {
        binOut.RedAddTerm   = bs.readSigned(binOut.nBits);
        binOut.GreenAddTerm = bs.readSigned(binOut.nBits);
        binOut.BlueAddTerm  = bs.readSigned(binOut.nBits);
        binOut.AlphaAddTerm = bs.readSigned(binOut.nBits);
        totalBits += (4 * binOut.nBits);
    }

    if ( binOut.HasAddTerms && version == 1 ) {
        binOut.RedAddTerm   = bs.readSigned(binOut.nBits);
        binOut.GreenAddTerm = bs.readSigned(binOut.nBits);
        binOut.BlueAddTerm  = bs.readSigned(binOut.nBits);
        binOut.AlphaAddTerm = 0;
        totalBits += (3 * binOut.nBits);
    }
 
    SWFShift(data, ((totalBits + 7) / 8));
    return binOut;
 
}

CXFORMWITHALPHA combineColorTransform(const CXFORMWITHALPHA& parent, const CXFORMWITHALPHA& child) {

    CXFORMWITHALPHA binOut;

    binOut.RedMultTerm = (child.RedMultTerm * parent.RedMultTerm) / 256;
    binOut.GreenMultTerm = (child.GreenMultTerm * parent.GreenMultTerm) / 256;
    binOut.BlueMultTerm = (child.BlueMultTerm * parent.BlueMultTerm) / 256;
    binOut.AlphaMultTerm = (child.AlphaMultTerm * parent.AlphaMultTerm) / 256;

    binOut.RedAddTerm = ((child.RedAddTerm * parent.RedMultTerm) / 256) + parent.RedAddTerm;
    binOut.GreenAddTerm = ((child.GreenAddTerm * parent.GreenMultTerm) / 256) + parent.GreenAddTerm;
    binOut.BlueAddTerm = ((child.BlueAddTerm * parent.BlueMultTerm) / 256) + parent.BlueAddTerm;
    binOut.AlphaAddTerm = ((child.AlphaAddTerm * parent.AlphaMultTerm) / 256) + parent.AlphaAddTerm;

    binOut.HasMultTerms = ( binOut.RedMultTerm != 256 || binOut.GreenMultTerm != 256 || binOut.BlueMultTerm != 256 || binOut.AlphaMultTerm != 256 );
    binOut.HasAddTerms = ( binOut.RedAddTerm != 0 || binOut.GreenAddTerm != 0 || binOut.BlueAddTerm != 0 || binOut.AlphaAddTerm != 0 );

    return binOut;

}
