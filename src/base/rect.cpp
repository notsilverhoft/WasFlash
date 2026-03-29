#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>
#include <cstring>
#include "rect.h"
#include "../utils/bitStream.hpp"
#include "../utils/trackSWF.h"

RECT getRect(std::vector<uint8_t>& data, bool FWSHeaderRect) {

    RECT binOut;
    int fileByte;

    if ( FWSHeaderRect ) {

        fileByte = 8;

    }
    else {

        fileByte = 0;

    }


    uint64_t buffer1 = static_cast<uint64_t>(data[fileByte]);
    
    for (int i = 1; i < 8; i++) {

        buffer1 = ((buffer1 << 8) | (data[fileByte + i]));

    }

    int secondOffset = fileByte + 8;

    uint64_t buffer2 = static_cast<uint64_t>(data[secondOffset]);

    for (int i = 1; i < 8; i++) {

        buffer2 = ((buffer2 << 8) | (data[secondOffset + i]));

    }

    uint8_t bitstream[16];

    for (int i = 0; i < 8; i++) {

        bitstream[i] = (buffer1 >> (56 - i * 8)) & 0xFF;
        bitstream[8 + i] = (buffer2 >> (56 - i * 8)) & 0xFF;

    }   

    BitStream bs(bitstream);

    uint8_t nbits = static_cast<uint8_t> (bs.readUnsigned(5));
    binOut.xMin = bs.readSigned(nbits);
    binOut.xMax = bs.readSigned(nbits);
    binOut.yMin = bs.readSigned(nbits);
    binOut.yMax = bs.readSigned(nbits);

    int totalBytes = (5 + 4 * nbits + 7) / 8;
    SWFShift(data, (FWSHeaderRect ? 8 : 0) + totalBytes);

    return binOut;
}
