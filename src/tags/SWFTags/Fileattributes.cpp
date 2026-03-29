#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"

SWFTag getFileAttributesTag(rawSWFTag rawTag) {

    SWFTag binOut;
            
    uint8_t binOne = rawTag.tagData[0];  
    binOut.FileAttributes.reserved = static_cast<uint8_t>(binOne & 1);
    binOut.FileAttributes.UseDirectBlit = static_cast<bool>((binOne >> 1) & 1);
    binOut.FileAttributes.UseGPU = static_cast<bool>((binOne >> 2) & 1);
    binOut.FileAttributes.HasMetadata = static_cast<bool>((binOne >> 3) & 1);
    binOut.FileAttributes.ActionScript3 = static_cast<bool>((binOne >> 4) & 1);
    binOut.FileAttributes.reserved2 = static_cast<uint8_t>((binOne >> 5) & 3);
    binOut.FileAttributes.UseNetwork = static_cast<bool>((binOne >> 7) & 1);
    binOut.FileAttributes.reserved3 = rawTag.tagData[1];
    binOut.FileAttributes.reserved4 = rawTag.tagData[2];
    binOut.FileAttributes.reserved5 = rawTag.tagData[3];

    return binOut;

}
