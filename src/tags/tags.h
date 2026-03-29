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
#include "SWFTags/PlaceObject2.h"
#include "SWFTags/AVM/AVM2/doABC.h"
#include "../base/matrix.h"
#include "../base/colorTransformAlpha.h"
#include "../header/header.h"
#include "../rendering/renderer.h"

struct rawSWFTag {
    uint16_t tagCode;
    uint8_t shortTagLength;
    bool longTag;
    uint32_t longTagLength = NULL;
    std::vector<uint8_t> tagData;
};

rawSWFTag getSWFTag(std::vector<uint8_t>& SWFFile);

struct SWFCharacter {
    uint16_t ID;
    int xPos;
    int yPos;
};

// -- Tags -- //

struct SWFTag {

    // Tag #9 - SetBackgroundColor

    struct SWFSetBackgroundColorTag {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } SetBackgroundColor;

    // Tag #26 - PlaceObject2

    struct SWFPlaceObject2Tag {
        bool PlaceFlagHasClipActions;
        bool PlaceFlagHasClipDepth;
        bool PlaceFlagHasName;
        bool PlaceFlagHasRatio;
        bool PlaceFlagHasColorTransform;
        bool PlaceFlagHasMatrix;
        bool PlaceFlagHasCharacter;
        bool PlaceFlagMove;
        uint16_t Depth;
        uint16_t CharacterID;
        MATRIX Matrix;
        CXFORMWITHALPHA ColorTransform;
        uint16_t Ratio;
        std::string Name;
        uint16_t ClipDepth;
        CLIPACTIONS ClipActions;
    } PlaceObject2;

    // Tag #60 - DefineVideoStream

    struct SWFDefineVideoStreamTag {
        uint16_t characterID;
        uint16_t numFrames;
        uint16_t width;
        uint16_t height;
        uint16_t videoFlagsReserved;
        uint8_t videoFlagsDeblocking;
        bool videoFlagsSmoothing;
        uint8_t CodecID;
    } DefineVideoStream;
    
    // Tag #61 - VideoFrame 

    struct SWFVideoFrameTag {
        uint16_t StreamID;
        uint16_t FrameNum;
        std::vector<uint8_t> VideoData;
    } VideoFrame;

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
    } FileAttributes;
        
    // Tag #72 - DoABC
    struct SWFDoABCTag {
        abcFile parsedABC;
    } DoAbc;


    uint16_t tagCode;
};

SWFTag parseSWFTag(rawSWFTag rawTag);

void processor(std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv, bool& done, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv, std::atomic<bool>& running, const SWFHeader& header);

void pushTag(SWFTag tag, std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv);