#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../utils/trackSWF.h"
#include "SoundStreamBlock.h"

std::vector<uint8_t> getSoundStreamBlock(std::vector<uint8_t> block, int compression) {

    std::vector<uint8_t> binOut;
    if ( compression == 2 ) {

        SWFShift(block, 4);

    }
    
    binOut = block;

    return binOut;

}