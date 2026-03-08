#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "header.h"
#include "decomp.h"
#include "../utils/trackSWF.h"
#include "../utils/errcodes.h"
#include "../base/rect.h"


int getSWFType(std::vector<uint8_t> SWFFile) {

    if ( SWFFile[0] == 70 && SWFFile[1] == 87 && SWFFile[2] == 83 ) {

        std::cout << "File is FWS...\n";
        std::cout << "File is Uncompressed...\n";
        return 0;

    }
    if ( SWFFile[0] == 67 && SWFFile[1] == 87 && SWFFile[2] == 83 ) {

        std::cout << "File is CWS...\n";
        std::cout << "File is Compressed with Zlib...\n";
        return 1;

    }
    if ( SWFFile[0] == 90 && SWFFile[1] == 87 && SWFFile[2] == 83 ) {

        std::cout << "File is ZWS...\n";
        std::cout << "File is Compressed with LZMA...\n";
        return 2;

    }
    else {
        
        return 3;

    }

}

int getSWFVersion(std::vector<uint8_t> SWFFile) {

    int versionOut = static_cast<int>(SWFFile[3]);

    return versionOut;

}

uint32_t getSWFFileLength(std::vector<uint8_t> SWFFile) {

    uint8_t binOne = SWFFile[4];
    uint8_t binTwo = SWFFile[5];
    uint8_t binThree = SWFFile[6];
    uint8_t binFour = SWFFile[7];
    uint32_t binOut = static_cast<uint32_t>((binFour << 24) | (binThree << 16) | (binTwo << 8) | (binOne)); //Little Endian

    return binOut;

}

uint32_t getSWFCompressedLength(std::vector<uint8_t> SWFFile) {

    uint8_t binOne = SWFFile[8];
    uint8_t binTwo = SWFFile[9];
    uint8_t binThree = SWFFile[10];
    uint8_t binFour = SWFFile[11];
    uint32_t binOut = static_cast<uint32_t>((binFour << 24) | (binThree << 16) | (binTwo << 8) | (binOne)); //Little Endian

    return binOut;

}

LZMA getLzmaOptions(std::vector<uint8_t> SWFFile) {

    LZMA binOut;
    std::vector<uint8_t> properties;

    uint8_t binOne = SWFFile[12]; //Properties

    uint8_t oneOut = binOne % 9; //lc
    uint8_t twoOut = (binOne / 9) % 5; //lp
    uint8_t threeOut = binOne / 45; //pb

    uint8_t binTwo = SWFFile[9];
    uint8_t binThree = SWFFile[10];
    uint8_t binFour = SWFFile[11];
    uint8_t binFive = SWFFile[12];
    
    uint32_t fourOut = static_cast<uint32_t>((binFive << 24) | (binFour << 16) | (binThree << 8) | (binTwo)); //Little Endian

    properties.push_back(oneOut);
    properties.push_back(twoOut);
    properties.push_back(threeOut);
    
    binOut.LZMAProperties = properties;
    binOut.LZMADictionary = fourOut;

    return binOut;

}

float getSWFFrameRate(std::vector<uint8_t>& SWFFile) {

    uint8_t binOne = SWFFile[0];
    uint8_t binTwo = SWFFile[1];

    float binOut = static_cast<float>(((binTwo << 8) | (binOne)) / 256.0f);

    SWFShift(SWFFile, 2);

    return binOut;

}

uint16_t getSWFFrameCount(std::vector<uint8_t>& SWFFile) {

    uint8_t binOne = SWFFile[0];
    uint8_t binTwo = SWFFile[1];

    uint16_t binOut = static_cast<uint16_t>(((binTwo << 8) | (binOne)));

    SWFShift(SWFFile, 2);

    return binOut;

}

void errorChecker(int type, char* Filename, int SWFType, int SWFVersion) {


    // SWFType Only
        if ( type == 1 ) {

            // Error Code 11 - Invalid File
                if ( SWFType == 3) {

                    throwErr(11, Filename);
                    
                } 
        }

    // SWFVersion Only
        if ( type == 2 ) {

            // Error Code 21 - Bad Version
                if ( SWFVersion > 19 | SWFVersion < 1 ) {

                    throwErr(21, Filename);

                }

        }
    
    // Both
        if ( type == 3) {

            // Error Code 31 - LZMA is only SWF 13+
                if ( SWFType == 2 && SWFVersion < 13 ) {

                    throwErr(31, Filename, SWFVersion);

                }
                
            // Error Code 31 - LZMA is only SWF 13+
                if ( SWFType == 1 && SWFVersion < 6 ) {

                    throwErr(32, Filename, SWFVersion);

                }

        }

        return;
}

SWFHeader getSWFHeader(std::vector<uint8_t>& SWFFile, char* Filename) {

    SWFHeader binOut;
     
    binOut.SWFType = getSWFType(SWFFile);
    errorChecker(1, Filename, binOut.SWFType);
    
    binOut.SWFVersion = getSWFVersion(SWFFile);
    errorChecker(2, Filename, NULL, binOut.SWFVersion);
    errorChecker(3, Filename, binOut.SWFType, binOut.SWFVersion);

    binOut.SWFFileLength = getSWFFileLength(SWFFile);

    switch(binOut.SWFType) {

        case 0:
            std::cout << "Decompression: File is Uncompressed(FWS)! Continue...\n";
            binOut.SWFFrameSize = getRect(SWFFile, true);
        break;

        case 1:
            std::cout << "Decompression: File is Compressed with ZLIB! Decompressing...\n";
            SWFFile = decompressSWF(SWFFile, binOut.SWFFileLength, 1);
            std::cout << "Decompression: Done! Continue...\n";
            binOut.SWFFrameSize = getRect(SWFFile);
        break;

        case 2:
            binOut.SWFCompressedLength = getSWFCompressedLength(SWFFile);
            binOut.lzProperties = getLzmaOptions(SWFFile);
            SWFFile = decompressSWF(SWFFile, binOut.SWFFileLength, 2, binOut.SWFCompressedLength, binOut.lzProperties);
            binOut.SWFFrameSize = getRect(SWFFile);
        break;

    }

    

    std::cout << "XMin: " << binOut.SWFFrameSize.xMin / 20 << "\n";
    std::cout << "XMax: " << binOut.SWFFrameSize.xMax / 20 << "\n";
    std::cout << "YMin: " << binOut.SWFFrameSize.yMin / 20 << "\n";
    std::cout << "YMax: " << binOut.SWFFrameSize.yMax / 20 << "\n";

    binOut.SWFFrameRate = getSWFFrameRate(SWFFile);
    std::cout << "Frame Rate: " << binOut.SWFFrameRate << "\n";

    binOut.SWFFrameCount = getSWFFrameCount(SWFFile);
    std::cout << "Frame Count: " << binOut.SWFFrameCount << "\n";

    return binOut;
}