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
#include "../utils/errcodes.h"
#include "../utils/trackSWF.h"
#include "../header/header.h"
#include "../rendering/renderer.h"
#include "../rendering/audioPlayer.h"
#include "../rendering/videoProcessor.h"
#include "../rendering/audioProcessor.h"
#include "../rendering/shapeProcessor.h"
#include "../base/colorTransformAlpha.h"
#include "SWFTags/AVM/AVM2/doABC.h"


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

    bool output = false;

    if (binOut.tagCode != 9 && binOut.tagCode != 60 && binOut.tagCode != 61 && binOut.tagCode != 69 && binOut.tagCode != 19 && binOut.tagCode != 18 && binOut.tagCode != 45) {
        output = true;
        std::cout << "\nTag Code: " << (int)binOut.tagCode << "\n";
    }

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

        if (output)
            std::cout << "Tag length:" << longTagLength << "\n";

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

        if (output)
            std::cout << "Tag length: " << static_cast<int>(shortTagLength) << "\n";
    }

    if (output)
        std::cout << "Actual Tag length: " << binOut.tagData.size() << "\n";

    return binOut;
}


SWFTag parseSWFTag(rawSWFTag rawTag) {

    SWFTag binOut;
    binOut.tagCode = rawTag.tagCode;

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

        case 60:
            binOut = getDefineVideoStreamTag(rawTag);
        break;

        case 61:
            binOut = getVideoFrameTag(rawTag);
        break;

        case 69:
            binOut = getFileAttributesTag(rawTag);
            std::cout << "ActionScript 3: " << binOut.FileAttributes.ActionScript3 << "\n";
        break;

        case 72:
            getDoABCTag(rawTag);
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
};

void processor(std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv, bool& done, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv, std::atomic<bool>& running, const SWFHeader& header) {

    (void)running;

    std::unordered_map<int16_t, SWFTag>        processedTags;
    std::unordered_map<uint16_t, SWFCharacter> savedCharacters;
    std::deque<std::vector<uint8_t>>           queuedAudioFrames;
    std::deque<rendererInstruction>            queuedRenderingInstructions;

    std::map<uint16_t, rendererInstruction>    displayList;
    std::map<uint16_t, DisplayListEntry>       displayListEntries;

    rendererInstruction backgroundInstruction;
    bool hasBackground = false;

    auto nextFrame = std::chrono::steady_clock::now();

    auto buildInstruction = [&](uint16_t characterID, MATRIX matrix, bool hasColorTransform = false, CXFORMWITHALPHA colorTransform = {}) -> rendererInstruction {

        rendererInstruction instruction;

        if (savedCharacters.find(characterID) == savedCharacters.end()) return instruction;
        if (processedTags.find((int16_t)characterID) == processedTags.end()) return instruction;

        int characterCode = savedCharacters.at(characterID).code;

        switch (characterCode) {

            case 2:
            {
                instruction.instructionCode = 3;
                Shape transformed = transformShape(savedCharacters.at(characterID).SWFShape, matrix, header.SWFFrameSize);
                if (hasColorTransform) applyColorTransform(transformed, colorTransform);
                instruction.SWFShape = std::make_shared<Shape>(transformed);
            }
            break;

        }

        return instruction;

    };

    auto retroactiveUpdate = [&](uint16_t shapeID) {
        for (auto& [depth, entry] : displayListEntries) {
            if (entry.characterID == shapeID) {
                displayList[depth] = buildInstruction(entry.characterID, entry.matrix,
                    entry.hasColorTransform, entry.colorTransform);
            }
        }
    };

    while (true) {
        std::unique_lock<std::mutex> lock(streamMutex);
        cv.wait(lock, [&] { return !stream.empty() || done; });

        if (stream.empty() && done) break;

        SWFTag tag = stream.back();
        stream.pop_back();
        lock.unlock();

        std::cout << "Current Tag to Process: " << tag.tagCode << "\n";

        rendererInstruction instruction;

        switch (tag.tagCode) {

            case 1: // ShowFrame
            {
                std::cout << "Hit ShowFrameTag\n";

                if (hasBackground)
                    pushRendererInstruction(backgroundInstruction, renderStream, renderStreamMutex, renderCv);

                for (auto& [depth, instr] : displayList) {
                    pushRendererInstruction(instr, renderStream, renderStreamMutex, renderCv);
                }

                while (!queuedRenderingInstructions.empty()) {
                    pushRendererInstruction(queuedRenderingInstructions.back(), renderStream, renderStreamMutex, renderCv);
                    queuedRenderingInstructions.pop_back();
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
                std::cout << "Processing Shape Started!\n";
                savedCharacters[tag.DefineShape.ShapeID] = {2, 0, 0, getShape(tag.DefineShape.ShapeBounds, tag.DefineShape.Shapes, 1)};
                processedTags[tag.DefineShape.ShapeID] = tag;
                retroactiveUpdate(tag.DefineShape.ShapeID);
                std::cout << "Processing Shape Ended!\n";
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
                std::cout << "Processing Shape Started!\n";
                savedCharacters[tag.DefineShape.ShapeID] = {2, 0, 0, getShape(tag.DefineShape.ShapeBounds, tag.DefineShape.Shapes, 2)};
                processedTags[tag.DefineShape.ShapeID] = tag;
                retroactiveUpdate(tag.DefineShape.ShapeID);
                std::cout << "Processing Shape Ended!\n";
                nextFrame = std::chrono::steady_clock::now();
            break;

            case 26: // PlaceObject2
            {
                uint16_t depth = tag.PlaceObject2.Depth;

                if (tag.PlaceObject2.PlaceFlagHasCharacter) {

                    uint16_t characterID = tag.PlaceObject2.CharacterID;
                    MATRIX matrix = tag.PlaceObject2.Matrix;

                    displayListEntries[depth] = {
                        characterID,
                        matrix,
                        tag.PlaceObject2.ColorTransform,
                        tag.PlaceObject2.PlaceFlagHasColorTransform
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
                            displayListEntries[depth] = {characterID, matrix, {}, false};
                            // store the entry but don't add to displayList
                            std::cout << "PlaceObject2: SKIPPED characterID=" << characterID << " at depth=" << depth << " tagCode=" << processedTags.at((int16_t)characterID).tagCode << "(Has Clip Depth)\n";
                        }
                        else if (savedCharacters.find(characterID) != savedCharacters.end()) {
                            displayList[depth] = buildInstruction(characterID, matrix,
                                tag.PlaceObject2.PlaceFlagHasColorTransform,
                                tag.PlaceObject2.ColorTransform);
                                std::cout << "PlaceObject2: placed characterID=" << characterID << " at depth=" << depth << " tagCode=" << processedTags.at((int16_t)characterID).tagCode << "\n";
                        } else {
                            std::cout << "PlaceObject2: SKIPPED characterID=" << characterID << " at depth=" << depth << " (not in savedCharacters)\n";
                        }
                    } else {
                        std::cout << "PlaceObject2: SKIPPED characterID=" << characterID << " at depth=" << depth << " (not in processedTags)\n";
                    }

                } else if (tag.PlaceObject2.PlaceFlagMove) {

                    if (displayListEntries.find(depth) != displayListEntries.end()) {

                        DisplayListEntry& entry = displayListEntries[depth];

                        if (tag.PlaceObject2.PlaceFlagHasMatrix) {
                            entry.matrix = tag.PlaceObject2.Matrix;
                        }

                        if (tag.PlaceObject2.PlaceFlagHasColorTransform) {
                            entry.colorTransform = tag.PlaceObject2.ColorTransform;
                            entry.hasColorTransform = true;
                        }

                        if (processedTags.find((int16_t)entry.characterID) != processedTags.end()) {
                            int tagCode = processedTags.at((int16_t)entry.characterID).tagCode;
                            if (tagCode != 60 && savedCharacters.find(entry.characterID) != savedCharacters.end()) {
                                displayList[depth] = buildInstruction(entry.characterID, entry.matrix,
                                    entry.hasColorTransform, entry.colorTransform);
                            }
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
                std::cout << "Processing Shape Started!\n";
                savedCharacters[tag.DefineShape.ShapeID] = {2, 0, 0, getShape(tag.DefineShape.ShapeBounds, tag.DefineShape.Shapes, 3)};
                processedTags[tag.DefineShape.ShapeID] = tag;
                retroactiveUpdate(tag.DefineShape.ShapeID);
                std::cout << "Processing Shape Ended!\n";
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
                std::cout << "VideoFrame: StreamID=" << tag.VideoFrame.StreamID
                          << " processedTags=" << (processedTags.find(tag.VideoFrame.StreamID) != processedTags.end())
                          << " savedCharacters=" << (savedCharacters.find(tag.VideoFrame.StreamID) != savedCharacters.end()) << "\n";

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
        }
    }
}

void pushTag(SWFTag tag, std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv) {
    std::lock_guard<std::mutex> lock(streamMutex);
    stream.push_front(tag);
    cv.notify_one();
}