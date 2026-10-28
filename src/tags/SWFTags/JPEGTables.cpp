#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../utils/trackSWF.h"

SWFTag getJPEGTablesTag(rawSWFTag rawTag) {

    SWFTag binOut;

    std::cout << "RawSizeJpegTables: " << rawTag.tagData.size() << "\n";

    if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD9 && rawTag.tagData[2] == 0xFF && rawTag.tagData[3] == 0xD8 ) {
        SWFShift(rawTag.tagData, 4);
    }

    while (true) {

        if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD9 ) {
            SWFShift(rawTag.tagData, 2);
            break;
        }

        binOut.JPEGTables.JPEGData.push_back(rawTag.tagData[0]);
        SWFShift(rawTag.tagData, 1);

    }

    return binOut;

}