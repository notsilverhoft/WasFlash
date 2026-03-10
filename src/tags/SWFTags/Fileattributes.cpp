#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"

SWFTag getFileAttributesTag(rawSWFTag rawTag) {

    SWFTag binOut;
            
    uint8_t binOne = rawTag.tagData[0];  
    binOut.reserved = static_cast<uint8_t>(binOne & 1);
    binOut.UseDirectBlit = static_cast<bool>((binOne >> 1) & 1);
    binOut.UseGPU = static_cast<bool>((binOne >> 2) & 1);
    binOut.HasMetadata = static_cast<bool>((binOne >> 3) & 1);
    binOut.ActionScript3 = static_cast<bool>((binOne >> 4) & 1);
    binOut.reserved2 = static_cast<uint8_t>((binOne >> 5) & 3);
    binOut.UseNetwork = static_cast<bool>((binOne >> 7) & 1);
    binOut.reserved3 = rawTag.tagData[1];
    binOut.reserved4 = rawTag.tagData[2];
    binOut.reserved5 = rawTag.tagData[3];

    return binOut;

}
