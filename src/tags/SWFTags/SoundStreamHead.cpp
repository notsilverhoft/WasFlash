#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../utils/trackSWF.h"
#include "SoundStreamHead.h"

SWFTag getSoundStreamHeadTag(rawSWFTag rawTag) {
    
    SWFTag binOut;

    if (rawTag.tagCode == 18 ) {

    binOut.SoundStreamHead.Reserved = (rawTag.tagData[0] >> 4);
    // std::cout << "SoundStreamHead: Reserved(Should Be 0): " << (int)binOut.SoundStreamHead.Reserved << "\n";

    binOut.SoundStreamHead.PlaybackSoundRate = ((rawTag.tagData[0] >> 2) & 0x03);
    // std::cout << "SoundStreamHead: PlaybackSoundRate: " << (int)binOut.SoundStreamHead.PlaybackSoundRate << "\n";

    binOut.SoundStreamHead.PlaybackSoundSize = ((rawTag.tagData[0] >> 1) & 0x01);
    // std::cout << "SoundStreamHead: PlaybackSoundSize: " << (int)binOut.SoundStreamHead.PlaybackSoundSize << "\n";

    binOut.SoundStreamHead.PlaybackSoundType = ((rawTag.tagData[0]) & 0x01);
    // std::cout << "SoundStreamHead: PlaybackSoundType: " << (int)binOut.SoundStreamHead.PlaybackSoundType << "\n";

    SWFShift(rawTag.tagData, 1);
    binOut.SoundStreamHead.StreamSoundCompression = (rawTag.tagData[0] >> 4);
    // std::cout << "SoundStreamHead: StreamSoundCompression: " << (int)binOut.SoundStreamHead.StreamSoundCompression << "\n";

    binOut.SoundStreamHead.StreamSoundRate= ((rawTag.tagData[0] >> 2) & 0x03);
    // std::cout << "SoundStreamHead: StreamSoundRate: " << (int)binOut.SoundStreamHead.StreamSoundRate << "\n";

    binOut.SoundStreamHead.StreamSoundSize = ((rawTag.tagData[0] >> 1) & 0x01);
    // std::cout << "SoundStreamHead: StreamSoundSize: " << (int)binOut.SoundStreamHead.StreamSoundSize << "\n";

    binOut.SoundStreamHead.StreamSoundType = ((rawTag.tagData[0]) & 0x01);
    // std::cout << "SoundStreamHead: StreamSoundType: " << (int)binOut.SoundStreamHead.StreamSoundType << "\n";

    SWFShift(rawTag.tagData, 1);
    binOut.SoundStreamHead.StreamSoundSampleCount = static_cast<uint16_t>((rawTag.tagData[1] << 8) | (rawTag.tagData[0]));
    // std::cout << "SoundStreamHead: StreamSoundSampleCount: " << (int)binOut.SoundStreamHead.StreamSoundSampleCount << "\n";

    SWFShift(rawTag.tagData, 2);

    if ( binOut.SoundStreamHead.StreamSoundCompression == 2 ) {
     
        binOut.SoundStreamHead.LatencySeek = static_cast<int16_t>((rawTag.tagData[1] << 8) | (rawTag.tagData[0]));
        // std::cout << "SoundStreamHead: LatencySeek: " << (int)binOut.SoundStreamHead.LatencySeek << "\n";
        
        SWFShift(rawTag.tagData, 2);

    }

    }

    if (rawTag.tagCode == 45 ) {

    binOut.SoundStreamHead2.Reserved = (rawTag.tagData[0] >> 4);
    // std::cout << "SoundStreamHead2: Reserved(Should Be 0): " << (int)binOut.SoundStreamHead2.Reserved << "\n";

    binOut.SoundStreamHead2.PlaybackSoundRate = ((rawTag.tagData[0] >> 2) & 0x03);
    // std::cout << "SoundStreamHead2: PlaybackSoundRate: " << (int)binOut.SoundStreamHead2.PlaybackSoundRate << "\n";

    binOut.SoundStreamHead2.PlaybackSoundSize = ((rawTag.tagData[0] >> 1) & 0x01);
    // std::cout << "SoundStreamHead2: PlaybackSoundSize: " << (int)binOut.SoundStreamHead2.PlaybackSoundSize << "\n";

    binOut.SoundStreamHead2.PlaybackSoundType = ((rawTag.tagData[0]) & 0x01);
    // std::cout << "SoundStreamHead2: PlaybackSoundType: " << (int)binOut.SoundStreamHead2.PlaybackSoundType << "\n";

    SWFShift(rawTag.tagData, 1);
    binOut.SoundStreamHead2.StreamSoundCompression = (rawTag.tagData[0] >> 4);
    // std::cout << "SoundStreamHead2: StreamSoundCompression: " << (int)binOut.SoundStreamHead2.StreamSoundCompression << "\n";

    binOut.SoundStreamHead2.StreamSoundRate= ((rawTag.tagData[0] >> 2) & 0x03);
    // std::cout << "SoundStreamHead2: StreamSoundRate: " << (int)binOut.SoundStreamHead2.StreamSoundRate << "\n";

    binOut.SoundStreamHead2.StreamSoundSize = ((rawTag.tagData[0] >> 1) & 0x01);
    // std::cout << "SoundStreamHead2: StreamSoundSize: " << (int)binOut.SoundStreamHead2.StreamSoundSize << "\n";

    binOut.SoundStreamHead2.StreamSoundType = ((rawTag.tagData[0]) & 0x01);
    // std::cout << "SoundStreamHead2: StreamSoundType: " << (int)binOut.SoundStreamHead2.StreamSoundType << "\n";

    SWFShift(rawTag.tagData, 1);
    binOut.SoundStreamHead2.StreamSoundSampleCount = static_cast<uint16_t>((rawTag.tagData[1] << 8) | (rawTag.tagData[0]));
    // std::cout << "SoundStreamHead2: StreamSoundSampleCount: " << (int)binOut.SoundStreamHead2.StreamSoundSampleCount << "\n";

    SWFShift(rawTag.tagData, 2);

    if ( binOut.SoundStreamHead2.StreamSoundCompression == 2 ) {
     
        binOut.SoundStreamHead2.LatencySeek = static_cast<int16_t>((rawTag.tagData[1] << 8) | (rawTag.tagData[0]));
        // std::cout << "SoundStreamHead2: LatencySeek: " << (int)binOut.SoundStreamHead2.LatencySeek << "\n";
        
        SWFShift(rawTag.tagData, 2);

    }

    }
    return binOut;

}