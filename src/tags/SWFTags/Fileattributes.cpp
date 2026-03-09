#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"

SWFTag getFileAttributesTag(rawSWFTag rawTag) {

    SWFTag binOut;
            
    uint8_t binOne = rawTag.tagData[0];  
    binOut.reserved = static_cast<uint8_t>(binOne & 1);
    std::cout << "reserved: " << static_cast<int>(binOut.reserved) << "\n";
    binOut.UseDirectBlit = static_cast<bool>((binOne >> 1) & 1);
    std::cout << "UseDirectBlit: " << binOut.UseDirectBlit << "\n";
    binOut.UseGPU = static_cast<bool>((binOne >> 2) & 1);
    std::cout << "UseGPU: " << binOut.UseGPU << "\n";
    binOut.HasMetadata = static_cast<bool>((binOne >> 3) & 1);
    std::cout << "HasMetadata: " << binOut.HasMetadata << "\n";
    binOut.ActionScript3 = static_cast<bool>((binOne >> 4) & 1);
    std::cout << "ActionScript3: " << binOut.ActionScript3 << "\n";
    binOut.reserved2 = static_cast<uint8_t>((binOne >> 5) & 3);
    std::cout << "reserved: " << static_cast<int>(binOut.reserved2) << "\n";
    binOut.UseNetwork = static_cast<bool>((binOne >> 7) & 1);
    std::cout << "UseNetwork: " << binOut.UseNetwork << "\n";
    binOut.reserved3 = rawTag.tagData[1];
    std::cout << "reserved: " << static_cast<int>(binOut.reserved3) << "\n";
    binOut.reserved4 = rawTag.tagData[2];
    std::cout << "reserved: " << static_cast<int>(binOut.reserved4) << "\n";
    binOut.reserved5 = rawTag.tagData[3];
    std::cout << "reserved: " << static_cast<int>(binOut.reserved5) << "\n";

    return binOut;

}
