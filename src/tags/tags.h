#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <unordered_map>
#include "SWFTags/Fileattributes.h"
#include "../header/header.h"

struct rawSWFTag {
    uint16_t tagCode;
    uint8_t shortTagLength;
    bool longTag;
    uint32_t longTagLength = NULL;
    std::vector<uint8_t> tagData;
};

rawSWFTag getSWFTag(std::vector<uint8_t>& SWFFile);

// -- Tags -- //

// Tag #69 - FileAttributes
struct SWFFileAttributesTag { // Only(And all) SWF version 8+
    uint8_t reserved; // All versions 8+
    bool UseDirectBlit; // Only true for versions 10+
    bool UseGPU; // Only true for versions 10+
    bool HasMetadata; // All versions 8+
    bool ActionScript3; // Only true for versions 9+
    uint8_t reserved2;
    bool UseNetwork; // All versions 8+
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
};

struct SWFTag : SWFFileAttributesTag {
    uint16_t tagCode;
};

SWFTag parseSWFTag(rawSWFTag rawTag);

void processor(std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv, bool& done, SWFHeader header);

void pushTag(SWFTag tag, std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv);