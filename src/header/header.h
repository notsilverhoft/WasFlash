#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../utils/errcodes.h"
#include "../base/rect.h"

struct LZMA {
    std::vector<uint8_t> LZMAProperties;
    uint32_t LZMADictionary;
};

struct SWFHeader {
    int SWFType;
    int SWFVersion;
    uint32_t SWFFileLength;
    uint32_t SWFCompressedLength; //Only for LZMA
    LZMA lzProperties; //Only for LZMA if you didn't already guess...
    RECT SWFFrameSize;
    float SWFFrameRate;
    uint16_t SWFFrameCount;
    std::vector<uint8_t> SWFFile;
};

int getSWFType(std::vector<uint8_t> SWFFile); 

int getSWFVersion(std::vector<uint8_t> SWFFile);

void errorChecker(int type, char* Filename, int SWFType = NULL, int SWFVersion = NULL);

SWFHeader getSWFHeader(std::vector<uint8_t>& SWFFile, char* Filename);