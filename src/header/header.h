#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../utils/errcodes.h"
#include "../base/rect.h"


struct SWFHeader {
    int SWFType;
    int SWFVersion;
    uint32_t SWFFileLength;
    RECT FrameSize;
};

int getSWFType(std::vector<uint8_t> SWFFile); 

int getSWFVersion(std::vector<uint8_t> SWFFile);

void errorChecker(int type, char* Filename, int SWFType = NULL, int SWFVersion = NULL);

SWFHeader getSWFHeader(std::vector<uint8_t>& SWFFile, char* Filename);