#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "DefineVideoStream.h"

SWFTag getDefineVideoStreamTag(rawSWFTag rawTag) {

    SWFTag binOut;

    binOut.DefineVideoStream.characterID = static_cast<uint16_t>((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    std::cout << "DefineVideoStream: CharacterID: " << (int)binOut.DefineVideoStream.characterID << "\n";
    binOut.DefineVideoStream.numFrames = static_cast<uint16_t>((rawTag.tagData[3] << 8) | rawTag.tagData[2]);
    std::cout << "DefineVideoStream: FrameNumber: " << (int)binOut.DefineVideoStream.numFrames << "\n";
    binOut.DefineVideoStream.width = static_cast<uint16_t>((rawTag.tagData[5] << 8) | rawTag.tagData[4]);
    std::cout << "DefineVideoStream: Width: " << (int)binOut.DefineVideoStream.width << "\n";
    binOut.DefineVideoStream.height = static_cast<uint16_t>((rawTag.tagData[7] << 8) | rawTag.tagData[6]);
    std::cout << "DefineVideoStream: Height: " << (int)binOut.DefineVideoStream.height << "\n";
    
    uint8_t binOne = rawTag.tagData[8];
    
    binOut.DefineVideoStream.videoFlagsReserved = ((binOne >> 4) & 0xF);
    std::cout << "DefineVideoStream: Reserved(Should Be 0): " << binOut.DefineVideoStream.videoFlagsReserved << "\n";
    binOut.DefineVideoStream.videoFlagsDeblocking = ((binOne >> 1) & 0x7);
    binOut.DefineVideoStream.videoFlagsSmoothing = (binOne & 0x1);
    std::cout << "DefineVideoStream: Smoothing: " << (bool)binOut.DefineVideoStream.videoFlagsSmoothing << "\n";

    binOut.DefineVideoStream.CodecID = rawTag.tagData[9];
    std::cout << "DefineVideoStream: CodecID: " << (int)binOut.DefineVideoStream.CodecID << "\n";

    return binOut;

}