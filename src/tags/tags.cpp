#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <unordered_map>
#include "tags.h"
#include "../utils/errcodes.h"
#include "../utils/trackSWF.h"
#include "SWFTags/Fileattributes.h"
#include "../header/header.h"

rawSWFTag getSWFTag(std::vector<uint8_t>& SWFFile) {

    rawSWFTag binOut;

    uint8_t shortOne = SWFFile[0];
    uint8_t shortTwo = SWFFile[1];
    
    uint16_t shortCodeLength = ((shortTwo << 8) | (shortOne));
    
    binOut.tagCode = ((shortCodeLength >> 6) & 0x3FF);

    std::cout << "\nTag Code: " << (int)binOut.tagCode << "\n";

    uint8_t shortTagLength = (shortCodeLength & 0x3F);
    binOut.shortTagLength = shortTagLength;

    uint32_t longTagLength;
    uint8_t longOne;
    uint8_t longTwo;
    uint8_t longThree;
    uint8_t longFour;
    if ( shortTagLength == 0x3F ) {

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
        binOut.longTagLength = longTagLength;
        SWFShift(SWFFile, 2);

    }

    

    if ( binOut.longTag == true ) {

        binOut.tagData.insert(binOut.tagData.end(), SWFFile.begin(), SWFFile.begin() + longTagLength);
        
        SWFShift(SWFFile, longTagLength);
        
        std::cout << "Tag length:" << longTagLength << "\n";

    } else {

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

        case 69: // Tag #69 - FileAttributes
            binOut = getFileAttributesTag(rawTag);
            binOut.tagCode = rawTag.tagCode;
        break;

    }

    return binOut;

}

void processor(std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv, bool& done, SWFHeader header) {

    std::unordered_map<int16_t, SWFTag> processedTags;

    while (true) {
        std::unique_lock<std::mutex> lock(streamMutex);
        cv.wait(lock, [&] { return !stream.empty() || done; });
        
        if ( stream.empty() && done ) break;
        
        SWFTag tag = stream.back();
        stream.pop_back();
        lock.unlock();
        
        // Process tags
        switch (tag.tagCode) {
                
            case 69: // Tag #69 - FileAttributes
                // Errors: 
                    if ( (int)tag.reserved != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( header.SWFVersion < 10 && tag.UseDirectBlit ) { // Error 692
                            
                        throwErr(692, NULL, header.SWFVersion);

                    }

                    if ( header.SWFVersion < 10 && tag.UseGPU ) { // Error 692
                            
                        throwErr(692, NULL, header.SWFVersion);

                    }

                    if ( header.SWFVersion < 9 && tag.ActionScript3 ) { // Error 692
                            
                        throwErr(692, NULL, header.SWFVersion);

                    }

                    if ( (int)tag.reserved2 != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( (int)tag.reserved3 != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( (int)tag.reserved4 != 0 ) { // Error 691

                        throwErr(691);

                    }

                    if ( (int)tag.reserved5 != 0 ) { // Error 691

                        throwErr(691);

                    }
                
                // Store
                    processedTags[(0 - tag.tagCode)] = tag;
        }

    }
}

    void pushTag(SWFTag tag, std::deque<SWFTag>& stream, std::mutex& streamMutex, std::condition_variable& cv) {
        std::lock_guard<std::mutex> lock(streamMutex);
        stream.push_front(tag);
        cv.notify_one();  // wake up processor
    }