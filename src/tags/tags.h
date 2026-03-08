#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>

struct SWFTag {
    uint16_t tagCode;
    uint8_t shortTagLength;
    bool longTag;
    uint32_t longTagLength = NULL;
    std::vector<uint8_t> tagData;
};

SWFTag getSWFTag(std::vector<uint8_t>& SWFFile);