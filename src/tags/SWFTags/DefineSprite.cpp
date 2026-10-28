#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../utils/trackSWF.h"

SWFTag getDefineSpriteTag(rawSWFTag rawTag) {

    SWFTag binOut;

    binOut.DefineSprite.SpriteID = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    SWFShift(rawTag.tagData, 2);
    // std::cout << "DefineSprite: SpriteID: " << (int)binOut.DefineSprite.SpriteID << "\n";

    binOut.DefineSprite.FrameCount = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    SWFShift(rawTag.tagData, 2);
    // std::cout << "DefineSprite: FrameCount: " << (int)binOut.DefineSprite.FrameCount << "\n";

    rawSWFTag tempTag;

    while (true) {

        tempTag = getSWFTag(rawTag.tagData);

        // std::cout << "DefineSprite: EmbeddedTag: Tag Code: " << (int)tempTag.tagCode << "\n";

        if ( tempTag.tagCode == 0 ) {

            binOut.DefineSprite.ControlTags.emplace_back(parseSWFTag(tempTag)); // Emplace because push was taking way too long
            break; // End

        }

        binOut.DefineSprite.ControlTags.emplace_back(parseSWFTag(tempTag)); // Emplace because push was taking way too long

    }

    return binOut;

}

