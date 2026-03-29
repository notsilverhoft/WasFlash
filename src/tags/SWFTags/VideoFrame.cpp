#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../utils/trackSWF.h"
#include "VideoFrame.h"

SWFTag getVideoFrameTag(rawSWFTag& rawTag) {

    SWFTag binOut;

    binOut.VideoFrame.StreamID = static_cast<uint16_t>((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    std::cout << "VideoFrame: StreamID: " << (int)binOut.VideoFrame.StreamID << "\n";
    SWFShift(rawTag.tagData, 2);

    binOut.VideoFrame.FrameNum = static_cast<uint16_t>((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    std::cout << "VideoFrame: FrameNum: " << (int)binOut.VideoFrame.FrameNum << "\n";
    SWFShift(rawTag.tagData, 2);

    binOut.VideoFrame.VideoData = rawTag.tagData;

    return binOut;

}