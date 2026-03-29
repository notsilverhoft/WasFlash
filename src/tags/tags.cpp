#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <unordered_map>
#include <atomic>
#include "tags.h"
#include "SWFTags/Fileattributes.h"
#include "SWFTags/DefineVideoStream.h"
#include "SWFTags/PlaceObject2.h"
#include "SWFTags/VideoFrame.h"
#include "../utils/errcodes.h"
#include "../utils/trackSWF.h"
#include "../header/header.h"
#include "../rendering/renderer.h"
#include "../rendering/videoProcessor.h"
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

    std::cout << "\nTag Code: " << (int)binOut.tagCode << "\n";

    uint8_t shortTagLength = (shortCodeLength & 0x3F);
    binOut.shortTagLength = shortTagLength;

    uint32_t longTagLength = 0;
    uint8_t longOne;
    uint8_t longTwo;
    uint8_t longThree;
    uint8_t longFour;
    if ( shortTagLength == 0x3F ) {

        if (SWFFile.size() < 6) {
            binOut.tagCode = 0;
            binOut.shortTagLength = 0;
            binOut.longTag = false;
            binOut.longTagLength = 0;
            return binOut;
        }

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
        binOut.longTagLength = shortTagLength;
        SWFShift(SWFFile, 2);

    }

    

    if ( binOut.longTag == true ) {

        if (longTagLength > SWFFile.size()) {
            binOut.tagCode = 0;
            binOut.shortTagLength = 0;
            binOut.longTag = false;
            binOut.longTagLength = 0;
            return binOut;
        }

        binOut.tagData.insert(binOut.tagData.end(), SWFFile.begin(), SWFFile.begin() + longTagLength);
        
        SWFShift(SWFFile, longTagLength);
        
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

        std::cout << "Tag length: " << static_cast<int>(shortTagLength) << "\n";

    }

    std::cout << "Actual Tag length: " << binOut.tagData.size() << "\n";

    return binOut;

}


SWFTag parseSWFTag(rawSWFTag rawTag) {

    SWFTag binOut;
    binOut.tagCode = rawTag.tagCode;

    switch(rawTag.tagCode) {

        case 9: // Tag #9 - SetBackgroundColor
            binOut.SetBackgroundColor.red = rawTag.tagData[0];
            std::cout << "Red: " << (int)binOut.SetBackgroundColor.red << "\n";
            binOut.SetBackgroundColor.green = rawTag.tagData[1];
            std::cout << "Green: " << (int)binOut.SetBackgroundColor.green << "\n";
            binOut.SetBackgroundColor.blue = rawTag.tagData[2];
            std::cout << "Blue: " << (int)binOut.SetBackgroundColor.blue << "\n";
        break;

        case 26: // Tag #26 - PlaceObject2
            binOut = getPlaceObject2Tag(rawTag);
        break;
        
        case 60: // Tag #60 - DefineVideoStream
            binOut = getDefineVideoStreamTag(rawTag);
        break;

        case 61: // Tag #61 - VideoFrame
            binOut = getVideoFrameTag(rawTag);
        break;

        case 69: // Tag #69 - FileAttributes
            binOut = getFileAttributesTag(rawTag);
            std::cout <<  "ActionScript 3: " << binOut.FileAttributes.ActionScript3 << "\n";

        break;

        case 72: // Tag# #72 - DoABC
            getDoABCTag(rawTag);
        break;
        
    }

    // Helper parsers return fresh SWFTag structs; force the original tag identity.
    binOut.tagCode = rawTag.tagCode;

    return binOut;

}

void processor(std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv, bool& done, std::deque<rendererInstruction>& renderStream, std::mutex& renderStreamMutex, std::condition_variable& renderCv, std::atomic<bool>& running, const SWFHeader& header) {

    std::unordered_map<int16_t, SWFTag> processedTags;
    std::unordered_map<uint16_t, SWFCharacter> savedCharacters;
    std::deque<rendererInstruction> queuedRenderingInstructions;

    while (true) {
        std::unique_lock<std::mutex> lock(streamMutex);
        cv.wait(lock, [&] { return !stream.empty() || done; });
        
        if ( stream.empty() && done ) break;
        
        SWFTag tag = stream.back();
        stream.pop_back();
        lock.unlock();
        rendererInstruction instruction;
        // Process tags
        switch (tag.tagCode) {

            case 1: // Tag #1 - ShowFrame

                while (!queuedRenderingInstructions.empty()) {
                    pushRendererInstruction(queuedRenderingInstructions.back(), renderStream, renderStreamMutex, renderCv);
                    queuedRenderingInstructions.pop_back();
                    
                }
                std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000.0f / header.SWFFrameRate));

            break;

            case 9: // Tag #9 - SetBackgroundColor

                instruction.instructionCode = 1;
                instruction.red = tag.SetBackgroundColor.red;
                instruction.green = tag.SetBackgroundColor.green;
                instruction.blue = tag.SetBackgroundColor.blue;

                pushRendererInstruction(instruction, renderStream, renderStreamMutex, renderCv);
                
            break;

            int tagCode;

            case 26: // Tag #26 - PlaceObject2

                if ( tag.PlaceObject2.PlaceFlagHasCharacter ) {

                    tagCode = processedTags[tag.PlaceObject2.CharacterID].tagCode;

                    switch (tagCode) {

                        case 60: // Define Video Stream

                            float oldX = savedCharacters[tag.PlaceObject2.CharacterID].xPos;
                            float oldY = savedCharacters[tag.PlaceObject2.CharacterID].yPos;
                            savedCharacters[tag.PlaceObject2.CharacterID].xPos = oldX * tag.PlaceObject2.Matrix.ScaleX + oldY * tag.PlaceObject2.Matrix.RotateSkew1 + tag.PlaceObject2.Matrix.TranslateX;
                            savedCharacters[tag.PlaceObject2.CharacterID].yPos = oldX * tag.PlaceObject2.Matrix.RotateSkew0 + oldY * tag.PlaceObject2.Matrix.ScaleY + tag.PlaceObject2.Matrix.TranslateY;

                        break;

                    }

                }

            break;

            case 60: // Tag #60 - DefineVideoStream
                // Errors:
                    if ( (int)tag.DefineVideoStream.videoFlagsReserved != 0 ) { // Error 601

                        throwErr(601);

                    }

                    if ( header.SWFVersion < 7 && tag.DefineVideoStream.CodecID == 3 ) {

                        throwErr(602);

                    }

                    if ( header.SWFVersion < 8 && tag.DefineVideoStream.CodecID == 4 ) {

                        throwErr(602);

                    }

                    if ( header.SWFVersion < 8 && tag.DefineVideoStream.CodecID == 5 ) {

                        throwErr(602);

                    }
                
                // Store Character
                    savedCharacters[tag.DefineVideoStream.characterID] = {tag.DefineVideoStream.characterID, 0, 0};

                initializeVideoDecoder(tag.DefineVideoStream.CodecID, tag.DefineVideoStream.width, tag.DefineVideoStream.height);

                // Store
                    processedTags[tag.DefineVideoStream.characterID] = tag;

            break;
                    
            case 61: //Tag 61 - VideoFrame

                if (processedTags.find(tag.VideoFrame.StreamID) == processedTags.end() || savedCharacters.find(tag.VideoFrame.StreamID) == savedCharacters.end()) {
                    break;
                }

                {
                    YUVFrame decodedFrame = decodeFrame(tag.VideoFrame.VideoData);
                    if (!decodedFrame.yPlane || !decodedFrame.uPlane || !decodedFrame.vPlane) {
                        break;
                    }

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

            case 69: // Tag #69 - FileAttributes
                // Errors: 
                    if ( (int)tag.FileAttributes.reserved != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( header.SWFVersion < 10 && tag.FileAttributes.UseDirectBlit ) { // Error 692
                            
                        throwErr(692, NULL, header.SWFVersion);

                    }

                    if ( header.SWFVersion < 10 && tag.FileAttributes.UseGPU ) { // Error 692
                            
                        throwErr(692, NULL, header.SWFVersion);

                    }

                    if ( header.SWFVersion < 9 && tag.FileAttributes.ActionScript3 ) { // Error 692
                            
                        throwErr(692, NULL, header.SWFVersion);

                    }

                    if ( (int)tag.FileAttributes.reserved2 != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( (int)tag.FileAttributes.reserved3 != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( (int)tag.FileAttributes.reserved4 != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( (int)tag.FileAttributes.reserved5 != 0 ) { // Error 691

                        throwErr(691);

                    }
                
                // Store
                    processedTags[(0 - tag.tagCode)] = tag;

            break;
                    
        }

    }
}

    void pushTag(SWFTag tag, std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv) {
        std::lock_guard<std::mutex> lock(streamMutex);
        stream.push_front(tag);
        cv.notify_one();  // wake up processor
    }