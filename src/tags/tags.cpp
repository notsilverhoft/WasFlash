#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "tags.h"
#include "../utils/trackSWF.h"

SWFTag getSWFTag(std::vector<uint8_t>& SWFFile) {

    SWFTag binOut;

    uint8_t shortOne = SWFFile[0];
    uint8_t shortTwo = SWFFile[1];
    
    uint16_t shortCodeLength = ((shortTwo << 8) | (shortOne));

    uint16_t tagCode = ((shortCodeLength >> 6) & 0x3FF);
    binOut.tagCode = tagCode;

    uint8_t shortTagLength = (shortCodeLength & 0x3F);
    binOut.shortTagLength = shortTagLength;

    uint32_t longTagLength;
    uint8_t longOne;
    uint8_t longTwo;
    uint8_t longThree;
    uint8_t longFour;
    if ( shortTagLength == 0x3F ) {

        longOne = SWFFile[2];
        longTwo = SWFFile[3];
        longThree = SWFFile[4];
        longFour = SWFFile[5];

        longTagLength = ((longFour << 24) | (longThree << 16) | (longTwo << 8) |(longOne));
        binOut.longTag = true;
        binOut.longTagLength = longTagLength;
        SWFShift(SWFFile, 6);

    } else {

        binOut.longTag = false;
        binOut.longTagLength = longTagLength;
        SWFShift(SWFFile, 2);

    }

    

    if ( binOut.longTag == true ) {

        binOut.tagData.insert(binOut.tagData.end(), SWFFile.begin(), SWFFile.begin() + longTagLength);
        
        SWFShift(SWFFile, longTagLength);

    } else {

        binOut.tagData.insert(binOut.tagData.end(), SWFFile.begin(), SWFFile.begin() + shortTagLength);

        SWFShift(SWFFile, shortTagLength);

    }

    return binOut;

}