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
#include <atomic>
#include <chrono>
#include "tags.h"
#include "SWFTags/Fileattributes.h"
#include "SWFTags/DefineVideoStream.h"
#include "SWFTags/PlaceObject2.h"
#include "SWFTags/VideoFrame.h"
#include "SWFTags/SoundStreamHead.h"
#include "SWFTags/SoundStreamBlock.h"
#include "SWFTags/DefineShape.h"
#include "SWFTags/DefineMorphShape.h"
#include "../utils/errcodes.h"
#include "../utils/trackSWF.h"
#include "../header/header.h"
#include "../rendering/renderer.h"
#include "../rendering/audioPlayer.h"
#include "../rendering/videoProcessor.h"
#include "../rendering/audioProcessor.h"
#include "../rendering/shapeProcessor.h"
#include "../rendering/morphShapeProcessor.h"
#include "../base/colorTransformAlpha.h"
#include "SWFTags/AVM/AVM2/doABC.h"
#include "../../include/skia/include/core/SkColorFilter.h"


rawSWFTag getSWFTag(std::vector<uint8_t>& SWFFile) {

    rawSWFTag binOut;

    if (SWFFile.size() < 2) {
        binOut.tagCode = 0;
        binOut.shortTagLength = 0;
        binOut.longTag = false;
        binOut.longTagLength = 0;
        return binOut;
    }

    uint8_t shortOne = SWFFile[0];
    uint8_t shortTwo = SWFFile[1];

    uint16_t shortCodeLength = ((shortTwo << 8) | (shortOne));

    binOut.tagCode = ((shortCodeLength >> 6) & 0x3FF);

    uint8_t shortTagLength = (shortCodeLength & 0x3F);
    binOut.shortTagLength = shortTagLength;

    uint32_t longTagLength = 0;
    uint8_t longOne;
    uint8_t longTwo;
    uint8_t longThree;
    uint8_t longFour;

    if (shortTagLength == 0x3F) {

        if (SWFFile.size() < 6) {
            binOut.tagCode = 0;
            binOut.shortTagLength = 0;
            binOut.longTag = false;
            binOut.longTagLength = 0;
            return binOut;
        }

        longOne   = SWFFile[2];
        longTwo   = SWFFile[3];
        longThree = SWFFile[4];
        longFour  = SWFFile[5];

        longTagLength = ((longFour << 24) | (longThree << 16) | (longTwo << 8) | (longOne));
        binOut.longTag = true;
        binOut.longTagLength = longTagLength;
        SWFShift(SWFFile, 6);

    } else {

        binOut.longTag = false;
        binOut.longTagLength = shortTagLength;
        SWFShift(SWFFile, 2);

    }

    if (binOut.longTag == true) {

        if (longTagLength > SWFFile.size()) {
            binOut.tagCode = 0;
            binOut.shortTagLength = 0;
            binOut.longTag = false;
            binOut.longTagLength = 0;
            return binOut;
        }

        binOut.tagData.insert(binOut.tagData.end(), SWFFile.begin(), SWFFile.begin() + longTagLength);
        SWFShift(SWFFile, longTagLength);

    } else {

        if (shortTagLength > SWFFile.size()) {
            binOut.tagCode = 0;
            binOut.shortTagLength = 0;
            binOut.longTag = false;
            binOut.longTagLength = 0;
            return binOut;
        }

        binOut.tagData.insert(binOut.tagData.end(), SWFFile.begin(), SWFFile.begin() + shortTagLength);
        SWFShift(SWFFile, shortTagLength);

    }

    return binOut;
}


SWFTag parseSWFTag(rawSWFTag rawTag) {

    SWFTag binOut;
    binOut.tagCode = rawTag.tagCode;

    if ( binOut.tagCode != 1 & binOut.tagCode != 2 & binOut.tagCode != 4 & binOut.tagCode != 9 & binOut.tagCode != 18 & binOut.tagCode != 19 & binOut.tagCode != 22 & binOut.tagCode != 26 & binOut.tagCode != 28 & binOut.tagCode != 32 & binOut.tagCode != 45 & binOut.tagCode != 46 & binOut.tagCode != 60 & binOut.tagCode != 61 & binOut.tagCode != 69 & binOut.tagCode != 83 & binOut.tagCode != 84 ) {

        std::cout << "Tag Code: " << binOut.tagCode << "\n";

    }

    switch (rawTag.tagCode) {

        case 2:
            binOut = getDefineShapeTag(rawTag, 1);
        break;

        case 9:
            binOut.SetBackgroundColor.red   = rawTag.tagData[0];
            binOut.SetBackgroundColor.green = rawTag.tagData[1];
            binOut.SetBackgroundColor.blue  = rawTag.tagData[2];
        break;

        case 18:
            binOut = getSoundStreamHeadTag(rawTag);
        break;

        case 19:
            binOut.SoundStreamBlock.StreamSoundData = rawTag.tagData;
        break;

        case 22:
            binOut = getDefineShapeTag(rawTag, 2);
        break;

        case 26:
            binOut = getPlaceObject2Tag(rawTag);
        break;

        case 28:
            binOut.RemoveObject2.Depth = (rawTag.tagData[0]) | (rawTag.tagData[1] << 8);
        break;

        case 32:
            binOut = getDefineShapeTag(rawTag, 3);
        break;

        case 45:
            binOut = getSoundStreamHeadTag(rawTag);
        break;

        case 46:
            binOut = getDefineMorphShapeTag(rawTag, 1);
        break;

        case 60:
            binOut = getDefineVideoStreamTag(rawTag);
        break;

        case 61:
            binOut = getVideoFrameTag(rawTag);
        break;

        case 69:
            binOut = getFileAttributesTag(rawTag);
        break;

        case 72:
            getDoABCTag(rawTag);
        break;

        case 83:
            binOut = getDefineShapeTag(rawTag, 4);
        break;

        case 84:
            binOut = getDefineMorphShapeTag(rawTag, 2);
        break;
    }

    binOut.tagCode = rawTag.tagCode;
    return binOut;
}

struct DisplayListEntry {
    uint16_t characterID;
    MATRIX   matrix;
    CXFORMWITHALPHA colorTransform;
    bool hasColorTransform = false;
    uint16_t ratio = 0;
};

sk_sp<SkColorFilter> buildColorFilter(const CXFORMWITHALPHA& ct) {
    float multR = ct.HasMultTerms ? ct.RedMultTerm   / 256.0f : 1.0f;
    float multG = ct.HasMultTerms ? ct.GreenMultTerm / 256.0f : 1.0f;
    float multB = ct.HasMultTerms ? ct.BlueMultTerm  / 256.0f : 1.0f;
    float multA = ct.HasMultTerms ? ct.AlphaMultTerm / 256.0f : 1.0f;
    float addR  = ct.HasAddTerms  ? ct.RedAddTerm    / 255.0f : 0.0f;
    float addG  = ct.HasAddTerms  ? ct.GreenAddTerm  / 255.0f : 0.0f;
    float addB  = ct.HasAddTerms  ? ct.BlueAddTerm   / 255.0f : 0.0f;
    float addA  = ct.HasAddTerms  ? ct.AlphaAddTerm  / 255.0f : 0.0f;
    float matrix[20] = {
        multR, 0,     0,     0,     addR,
        0,     multG, 0,     0,     addG,
        0,     0,     multB, 0,     addB,
        0,     0,     0,     multA, addA
    };
    return SkColorFilters::Matrix(matrix);
}

void processor(std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv, bool& done, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv, std::atomic<bool>& running, const SWFHeader& header) {

    (void)running;

    std::unordered_map<int16_t, SWFTag>               processedTags;
    std::unordered_map<uint16_t, SWFCharacter>        savedCharacters;
    std::deque<std::vector<uint8_t>>                  queuedAudioFrames;
    std::deque<rendererInstruction>                   queuedRenderingInstructions;

    std::map<uint16_t, rendererInstruction>           displayList;
    std::map<uint16_t, DisplayListEntry>              displayListEntries;

    rendererInstruction backgroundInstruction;
    bool hasBackground = false;

    auto nextFrame = std::chrono::steady_clock::now();

    auto buildInstruction = [&](uint16_t characterID, MATRIX matrix, bool hasColorTransform = false, CXFORMWITHALPHA colorTransform = {}, uint16_t ratio = 0) -> rendererInstruction {

        rendererInstruction instruction;

        if (savedCharacters.find(characterID) == savedCharacters.end()) return instruction;
        if (processedTags.find((int16_t)characterID) == processedTags.end()) return instruction;

        int characterCode = savedCharacters.at(characterID).code;

        switch (characterCode) {

            case 2:
            {
                instruction.instructionCode = 3;
                instruction.SWFShape = savedCharacters.at(characterID).SWFShape;

                if (hasColorTransform) {
                    instruction.colorFilter = buildColorFilter(colorTransform);
                }

                instruction.canvasTransform = transformShape(*instruction.SWFShape, matrix);
            }
            break;

            case 46:
            case 84:
            {
                instruction.instructionCode = 4;
                instruction.SWFMorphShapes = savedCharacters.at(characterID).SWFMorphShape;

                Shape frame = getMorphFrame(*instruction.SWFMorphShapes, ratio);

                if (hasColorTransform) {
                    applyColorTransform(frame, colorTransform);
                }

                instruction.SWFMorphFrame = std::make_shared<Shape>(std::move(frame));
                instruction.canvasTransform = transformMorphShape(*instruction.SWFMorphShapes, matrix);
            }
            break;

        }

        return instruction;

    };

    auto updateTransform = [&](uint16_t depth, MATRIX matrix) {
        if (displayList.find(depth) == displayList.end()) return;
        // Don't apply transforms intended for unrecognized characters (e.g. sprites)
        // to whatever shape is currently sitting at this depth
        if (displayListEntries.find(depth) != displayListEntries.end()) {
            uint16_t charID = displayListEntries.at(depth).characterID;
            if (processedTags.find((int16_t)charID) == processedTags.end()) return;
        }
        rendererInstruction& instr = displayList[depth];
        if (instr.instructionCode == 3 && instr.SWFShape) {
            instr.canvasTransform = transformShape(*instr.SWFShape, matrix);
        } else if (instr.instructionCode == 4 && instr.SWFMorphShapes) {
            instr.canvasTransform = transformMorphShape(*instr.SWFMorphShapes, matrix);
        }
    };

    auto retroactiveUpdate = [&](uint16_t shapeID) {
        for (auto& [depth, entry] : displayListEntries) {
            if (entry.characterID == shapeID) {
                displayList[depth] = buildInstruction(entry.characterID, entry.matrix,
                    entry.hasColorTransform, entry.colorTransform, entry.ratio);
            }
        }
    };

    while (true) {
        std::unique_lock<std::mutex> lock(streamMutex);
        cv.wait(lock, [&] { return !stream.empty() || done; });

        if (stream.empty() && done) break;

        SWFTag tag = std::move(stream.back());
        stream.pop_back();
        lock.unlock();

        switch (tag.tagCode) {

            case 1: // ShowFrame
            {
                {
                    std::lock_guard<std::mutex> renderLock(renderStreamMutex);

                    if (hasBackground)
                        renderStream.push_front(backgroundInstruction);

                    for (auto& [depth, instr] : displayList)
                        renderStream.push_front(instr);

                    while (!queuedRenderingInstructions.empty()) {
                        renderStream.push_front(queuedRenderingInstructions.back());
                        queuedRenderingInstructions.pop_back();
                    }

                    renderCv.notify_one();
                }

                while (!queuedAudioFrames.empty()) {
                    renderAudioFrame(queuedAudioFrames.back());
                    queuedAudioFrames.pop_back();
                }

                nextFrame += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::duration<float, std::milli>(1000.0f / header.SWFFrameRate)
                );
                std::this_thread::sleep_until(nextFrame);
            }
            break;

            case 2: // DefineShape
                savedCharacters[tag.DefineShape.ShapeID] = {2, 0, 0, std::make_shared<Shape>(getShape(tag.DefineShape.ShapeBounds, tag.DefineShape.Shapes, 1))};
                processedTags[tag.DefineShape.ShapeID] = tag;
                retroactiveUpdate(tag.DefineShape.ShapeID);
                nextFrame = std::chrono::steady_clock::now();
            break;

            case 9: // SetBackgroundColor
                backgroundInstruction.instructionCode = 1;
                backgroundInstruction.red   = tag.SetBackgroundColor.red;
                backgroundInstruction.green = tag.SetBackgroundColor.green;
                backgroundInstruction.blue  = tag.SetBackgroundColor.blue;
                hasBackground = true;
            break;

            case 18: // SoundStreamHead
                if (tag.SoundStreamHead.StreamSoundCompression == 2 && header.SWFVersion < 4) throwErr(181);
                if (tag.SoundStreamHead.StreamSoundSize != 1) throwErr(182);
                if (tag.SoundStreamHead.Reserved != 0) throwErr(183);
                initializeAudioDecoder(tag.SoundStreamHead.StreamSoundCompression, tag.SoundStreamHead.StreamSoundRate, tag.SoundStreamHead.StreamSoundSize, tag.SoundStreamHead.StreamSoundType, tag.SoundStreamHead.LatencySeek);
                initializeAudioPlayer(tag.SoundStreamHead.StreamSoundRate, tag.SoundStreamHead.StreamSoundType, tag.SoundStreamHead.StreamSoundSize, tag.SoundStreamHead.StreamSoundCompression);
                processedTags[(0 - tag.tagCode)] = tag;
            break;

            case 19: // SoundStreamBlock
                if (processedTags.find(-18) != processedTags.end())
                    tag.SoundStreamBlock.StreamSoundData = decodeAudioFrame(getSoundStreamBlock(tag.SoundStreamBlock.StreamSoundData, processedTags[-18].SoundStreamHead.StreamSoundCompression));
                if (processedTags.find(-45) != processedTags.end())
                    tag.SoundStreamBlock.StreamSoundData = decodeAudioFrame(getSoundStreamBlock(tag.SoundStreamBlock.StreamSoundData, processedTags[-45].SoundStreamHead2.StreamSoundCompression));
                queuedAudioFrames.push_front(tag.SoundStreamBlock.StreamSoundData);
            break;

            case 22: // DefineShape2
                savedCharacters[tag.DefineShape.ShapeID] = {2, 0, 0, std::make_shared<Shape>(getShape(tag.DefineShape.ShapeBounds, tag.DefineShape.Shapes, 2))};
                processedTags[tag.DefineShape.ShapeID] = tag;
                retroactiveUpdate(tag.DefineShape.ShapeID);
                nextFrame = std::chrono::steady_clock::now();
            break;

            case 26: // PlaceObject2
            {
                uint16_t depth = tag.PlaceObject2.Depth;

                if (tag.PlaceObject2.PlaceFlagHasCharacter) {

                    uint16_t characterID = tag.PlaceObject2.CharacterID;
                    MATRIX matrix = tag.PlaceObject2.Matrix;
                    uint16_t ratio = tag.PlaceObject2.Ratio;

                    displayListEntries[depth] = {
                        characterID,
                        matrix,
                        tag.PlaceObject2.ColorTransform,
                        tag.PlaceObject2.PlaceFlagHasColorTransform,
                        ratio
                    };

                    if (processedTags.find((int16_t)characterID) != processedTags.end()) {
                        int tagCode = processedTags.at((int16_t)characterID).tagCode;
                        if (tagCode == 60) {
                            float oldX = savedCharacters[characterID].xPos;
                            float oldY = savedCharacters[characterID].yPos;
                            savedCharacters[characterID].xPos = (oldX * matrix.ScaleX + oldY * matrix.RotateSkew1 + matrix.TranslateX) / 20.0f;
                            savedCharacters[characterID].yPos = (oldX * matrix.RotateSkew0 + oldY * matrix.ScaleY  + matrix.TranslateY) / 20.0f;
                        }
                        else if (tag.PlaceObject2.PlaceFlagHasClipDepth) {
                            displayListEntries[depth] = {characterID, matrix, {}, false, ratio};
                        }
                        else if (savedCharacters.find(characterID) != savedCharacters.end()) {
                            displayList[depth] = buildInstruction(characterID, matrix,
                                tag.PlaceObject2.PlaceFlagHasColorTransform,
                                tag.PlaceObject2.ColorTransform,
                                ratio);
                        }
                    }

                } else if (tag.PlaceObject2.PlaceFlagMove) {

                    if (displayListEntries.find(depth) != displayListEntries.end()) {

                        DisplayListEntry& entry = displayListEntries[depth];

                        // Update all changed fields first — multiple flags can be set simultaneously
                        if (tag.PlaceObject2.PlaceFlagHasMatrix) {
                            entry.matrix = tag.PlaceObject2.Matrix;
                        }
                        if (tag.PlaceObject2.PlaceFlagHasColorTransform) {
                            entry.colorTransform = tag.PlaceObject2.ColorTransform;
                            entry.hasColorTransform = true;
                        }
                        if (tag.PlaceObject2.PlaceFlagHasRatio) {
                            entry.ratio = tag.PlaceObject2.Ratio;
                        }

                        // Rebuild instruction if ratio or color transform changed (matrix is baked in),
                        // otherwise just update the canvas transform
                        bool needsRebuild = tag.PlaceObject2.PlaceFlagHasRatio ||
                                            tag.PlaceObject2.PlaceFlagHasColorTransform;

                        if (needsRebuild) {
                            if (processedTags.find((int16_t)entry.characterID) != processedTags.end()) {
                                int tagCode = processedTags.at((int16_t)entry.characterID).tagCode;
                                if (tagCode != 60 && savedCharacters.find(entry.characterID) != savedCharacters.end()) {
                                    displayList[depth] = buildInstruction(entry.characterID, entry.matrix,
                                        entry.hasColorTransform, entry.colorTransform, entry.ratio);
                                }
                            }
                        } else if (tag.PlaceObject2.PlaceFlagHasMatrix) {
                            updateTransform(depth, entry.matrix);
                        }

                    }

                }
            }
            break;

            case 28: // RemoveObject2
                displayList.erase(tag.RemoveObject2.Depth);
                displayListEntries.erase(tag.RemoveObject2.Depth);
            break;

            case 32: // DefineShape3
                savedCharacters[tag.DefineShape.ShapeID] = {2, 0, 0, std::make_shared<Shape>(getShape(tag.DefineShape.ShapeBounds, tag.DefineShape.Shapes, 3))};
                processedTags[tag.DefineShape.ShapeID] = tag;
                retroactiveUpdate(tag.DefineShape.ShapeID);
                nextFrame = std::chrono::steady_clock::now();
            break;

            case 45: // SoundStreamHead2
                if (tag.SoundStreamHead2.StreamSoundCompression == 2  && header.SWFVersion < 4)  throwErr(451);
                if (tag.SoundStreamHead2.StreamSoundCompression == 3  && header.SWFVersion < 4)  throwErr(452);
                if (tag.SoundStreamHead2.StreamSoundCompression == 4  && header.SWFVersion < 10) throwErr(453);
                if (tag.SoundStreamHead2.StreamSoundCompression == 5  && header.SWFVersion < 10) throwErr(454);
                if (tag.SoundStreamHead2.StreamSoundCompression == 6  && header.SWFVersion < 6)  throwErr(455);
                if (tag.SoundStreamHead2.StreamSoundCompression == 11 && header.SWFVersion < 10) throwErr(456);
                if (tag.SoundStreamHead2.Reserved != 0) throwErr(457);
                initializeAudioDecoder(tag.SoundStreamHead2.StreamSoundCompression, tag.SoundStreamHead2.StreamSoundRate, tag.SoundStreamHead2.StreamSoundSize, tag.SoundStreamHead2.StreamSoundType, tag.SoundStreamHead2.LatencySeek);
                initializeAudioPlayer(tag.SoundStreamHead2.StreamSoundRate, tag.SoundStreamHead2.StreamSoundType, tag.SoundStreamHead2.StreamSoundSize, tag.SoundStreamHead2.StreamSoundCompression);
                processedTags[(0 - tag.tagCode)] = tag;
            break;

            case 46: // DefineMorphShape
            {
                auto morphShapes = std::make_shared<std::pair<Shape, Shape>>(MorphShapeToShape(tag, 1));
                savedCharacters[tag.DefineMorphShape.CharacterId] = {46, 0, 0, nullptr, morphShapes};
                processedTags[tag.DefineMorphShape.CharacterId] = tag;
                retroactiveUpdate(tag.DefineMorphShape.CharacterId);
                nextFrame = std::chrono::steady_clock::now();
            }
            break;

            case 60: // DefineVideoStream
                if ((int)tag.DefineVideoStream.videoFlagsReserved != 0) throwErr(601);
                if (header.SWFVersion < 7 && tag.DefineVideoStream.CodecID == 3) throwErr(602);
                if (header.SWFVersion < 8 && tag.DefineVideoStream.CodecID == 4) throwErr(602);
                if (header.SWFVersion < 8 && tag.DefineVideoStream.CodecID == 5) throwErr(602);
                savedCharacters[tag.DefineVideoStream.characterID] = {tag.DefineVideoStream.characterID, 0, 0, {}};
                initializeVideoDecoder(tag.DefineVideoStream.CodecID, tag.DefineVideoStream.width, tag.DefineVideoStream.height);
                processedTags[tag.DefineVideoStream.characterID] = tag;
            break;

            case 61: // VideoFrame
            {
                if (processedTags.find(tag.VideoFrame.StreamID) == processedTags.end() ||
                    savedCharacters.find(tag.VideoFrame.StreamID) == savedCharacters.end())
                    break;

                YUVFrame decodedFrame = decodeFrame(tag.VideoFrame.VideoData);
                if (!decodedFrame.yPlane || !decodedFrame.uPlane || !decodedFrame.vPlane) break;

                queuedRenderingInstructions.push_front(getVideoInstruction(
                    decodedFrame.width,
                    decodedFrame.height,
                    savedCharacters[tag.VideoFrame.StreamID].xPos / 20,
                    savedCharacters[tag.VideoFrame.StreamID].yPos / 20,
                    processedTags[tag.VideoFrame.StreamID].DefineVideoStream.videoFlagsSmoothing,
                    decodedFrame.yPlane,
                    decodedFrame.uPlane,
                    decodedFrame.vPlane,
                    decodedFrame.yStride,
                    decodedFrame.uStride,
                    decodedFrame.vStride
                ));
            }
            break;

            case 69: // FileAttributes
                if (header.SWFVersion < 10 && tag.FileAttributes.UseDirectBlit) throwErr(692, NULL, header.SWFVersion);
                if (header.SWFVersion < 10 && tag.FileAttributes.UseGPU)        throwErr(692, NULL, header.SWFVersion);
                if (header.SWFVersion < 9  && tag.FileAttributes.ActionScript3) throwErr(692, NULL, header.SWFVersion);
                if ((int)tag.FileAttributes.reserved2 != 0) throwErr(691);
                if ((int)tag.FileAttributes.reserved3 != 0) throwErr(691);
                if ((int)tag.FileAttributes.reserved4 != 0) throwErr(691);
                if ((int)tag.FileAttributes.reserved5 != 0) throwErr(691);
                processedTags[(0 - tag.tagCode)] = tag;
            break;

            case 83: // DefineShape4
                savedCharacters[tag.DefineShape.ShapeID] = {2, 0, 0, std::make_shared<Shape>(getShape(tag.DefineShape.ShapeBounds, tag.DefineShape.Shapes, 4, tag.DefineShape.DefineShape4.UsesFillWindingRule))};
                processedTags[tag.DefineShape.ShapeID] = tag;
                retroactiveUpdate(tag.DefineShape.ShapeID);
                nextFrame = std::chrono::steady_clock::now();
            break;

            case 84: // DefineMorphShape2
            {
                auto morphShapes = std::make_shared<std::pair<Shape, Shape>>(MorphShapeToShape(tag, 2));
                savedCharacters[tag.DefineMorphShape.CharacterId] = {84, 0, 0, nullptr, morphShapes};
                processedTags[tag.DefineMorphShape.CharacterId] = tag;
                retroactiveUpdate(tag.DefineMorphShape.CharacterId);
                nextFrame = std::chrono::steady_clock::now();
            }
            break;

        }
    }
}

void pushTag(SWFTag tag, std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv) {
    std::lock_guard<std::mutex> lock(streamMutex);
    stream.push_front(tag);
    cv.notify_one();
}