// tags.h
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
#include <map>
#include "SWFTags/Fileattributes.h"
#include "SWFTags/PlaceObject2.h"
#include "SWFTags/AVM/AVM2/doABC.h"
#include "../base/matrix.h"
#include "../base/colorTransformAlpha.h"
#include "../base/shapeWithStyle.h"
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
    int code;
    int xPos;
    int yPos;
    std::shared_ptr<Shape> SWFShape;
};

// -- Tags -- //

struct SWFTag {
        
    // Tag #2 - DefineShape

    struct SWFDefineShapeTag {

        uint16_t ShapeID;
        RECT ShapeBounds;
        struct {
            
            RECT EdgeBounds;
            uint8_t Reserved;
            bool UsesFillWindingRule;
            bool UsesNonScalingStrokes;
            bool UsesScalingStrokes;
        
        } DefineShape4;

        SHAPEWITHSTYLE Shapes;


    } DefineShape;

    // Tag #9 - SetBackgroundColor

    struct SWFSetBackgroundColorTag {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } SetBackgroundColor;

    // Tag #18 - SoundStreamHead

    struct SWFSoundStreamHeadTag {

        uint8_t Reserved;
        uint8_t PlaybackSoundRate;
        uint8_t PlaybackSoundSize;
        uint8_t PlaybackSoundType;
        uint8_t StreamSoundCompression;
        uint8_t StreamSoundRate;
        uint8_t StreamSoundSize;
        uint8_t StreamSoundType;
        uint16_t StreamSoundSampleCount;
        int16_t LatencySeek = 0;

    } SoundStreamHead;

    // Tag #19 - SoundStreamBlock

    struct SWFSoundStreamBlockTag {
        std::vector<uint8_t> StreamSoundData;
    } SoundStreamBlock;

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

    // Tag #28 - RemoveObject2
    struct SWFRemoveObject2Tag {
        uint16_t Depth;
    } RemoveObject2;

    // Tag #45 - SoundStreamHead2 

    struct SWFSoundStreamHead2Tag {

        uint8_t Reserved;
        uint8_t PlaybackSoundRate;
        uint8_t PlaybackSoundSize;
        uint8_t PlaybackSoundType;
        uint8_t StreamSoundCompression;
        uint8_t StreamSoundRate;
        uint8_t StreamSoundSize;
        uint8_t StreamSoundType;
        uint16_t StreamSoundSampleCount;
        int16_t LatencySeek = 0;

    } SoundStreamHead2;

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

    struct SWFFileAttributesTag {
        uint8_t reserved;
        bool UseDirectBlit;
        bool UseGPU;
        bool HasMetadata;
        bool ActionScript3;
        uint8_t reserved2;
        bool UseNetwork;
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