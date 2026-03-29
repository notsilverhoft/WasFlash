#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../utils/trackSWF.h"
#include "../../base/string.h"

SWFTag getPlaceObject2Tag(rawSWFTag& rawTag) {
        
    SWFTag binOut;

    // Parsing:

    uint8_t binOne = rawTag.tagData[0];

    binOut.PlaceObject2.PlaceFlagHasClipActions = ((binOne >> 7) & 0x01);
    // std::cout << "PlaceObject2: HasClipActions: " << binOut.PlaceObject2.PlaceFlagHasClipActions << "\n";
    binOut.PlaceObject2.PlaceFlagHasClipDepth = ((binOne >> 6) & 0x01);
    // std::cout << "PlaceObject2: HasClipDepth: " << binOut.PlaceObject2.PlaceFlagHasClipDepth << "\n";
    binOut.PlaceObject2.PlaceFlagHasName = ((binOne >> 5) & 0x01);
    // std::cout << "PlaceObject2: HasName: " << binOut.PlaceObject2.PlaceFlagHasName << "\n";
    binOut.PlaceObject2.PlaceFlagHasRatio = ((binOne >> 4) & 0x01);
    // std::cout << "PlaceObject2: HasRatio: " << binOut.PlaceObject2.PlaceFlagHasRatio << "\n";
    binOut.PlaceObject2.PlaceFlagHasColorTransform = ((binOne >> 3) & 0x01);
    // std::cout << "PlaceObject2: HasColorTransform: " << binOut.PlaceObject2.PlaceFlagHasColorTransform << "\n";
    binOut.PlaceObject2.PlaceFlagHasMatrix = ((binOne >> 2) & 0x01);
    // std::cout << "PlaceObject2: HasMatrix: " << binOut.PlaceObject2.PlaceFlagHasMatrix << "\n";
    binOut.PlaceObject2.PlaceFlagHasCharacter = ((binOne >> 1) & 0x01);
    // std::cout << "PlaceObject2: HasCharacter: " << binOut.PlaceObject2.PlaceFlagHasCharacter << "\n";
    binOut.PlaceObject2.PlaceFlagMove = (binOne & 0x01);
    // std::cout << "PlaceObject2: Move: " << binOut.PlaceObject2.PlaceFlagMove << "\n";

    SWFShift(rawTag.tagData, 1);

    binOut.PlaceObject2.Depth = static_cast<uint16_t>((rawTag.tagData[1] << 8) & rawTag.tagData[0]);
    
    // std::cout << "PlaceObject2: Depth: " << (int)binOut.PlaceObject2.Depth << "\n";


    SWFShift(rawTag.tagData, 2);

    if ( binOut.PlaceObject2.PlaceFlagHasCharacter ) {

        binOut.PlaceObject2.CharacterID = ((rawTag.tagData[1] << 8) & rawTag.tagData[0]);
        SWFShift(rawTag.tagData, 2);
        std::cout << "PlaceObject2: CharacterID: " << (int)binOut.PlaceObject2.CharacterID << "\n";

    }

    if ( binOut.PlaceObject2.PlaceFlagHasMatrix ) {

        binOut.PlaceObject2.Matrix = getMatrix(rawTag.tagData);
        std::cout << "PlaceObject2: Matrix: HasScale: " << (int)binOut.PlaceObject2.Matrix.HasScale << "\n";
        if ( binOut.PlaceObject2.Matrix.HasScale ) {

            std::cout << "PlaceObject2: Matrix: ScaleX: " << binOut.PlaceObject2.Matrix.ScaleX << "\n";
            std::cout << "PlaceObject2: Matrix: ScaleY: " << binOut.PlaceObject2.Matrix.ScaleY << "\n";

        }

        std::cout << "PlaceObject2: Matrix: HasRotate: " << (int)binOut.PlaceObject2.Matrix.HasRotate << "\n";
        if ( binOut.PlaceObject2.Matrix.HasRotate ) {

            std::cout << "PlaceObject2: Matrix: RotateSkew0: " << binOut.PlaceObject2.Matrix.RotateSkew0 << "\n";
            std::cout << "PlaceObject2: Matrix: RotateSkew1: " << binOut.PlaceObject2.Matrix.RotateSkew1 << "\n";

        }
        
        std::cout << "PlaceObject2: Matrix: TranslateX: " << binOut.PlaceObject2.Matrix.TranslateX << "\n";
        std::cout << "PlaceObject2: Matrix: TranslateY: " << binOut.PlaceObject2.Matrix.TranslateY << "\n";

    }

    if ( binOut.PlaceObject2.PlaceFlagHasColorTransform ) {

        binOut.PlaceObject2.ColorTransform = getColorTransformWithAlpha(rawTag.tagData);
        std::cout << "PlaceObject2: ColorTransform: HasAddTerms: " << binOut.PlaceObject2.ColorTransform.HasAddTerms << "\n";
        std::cout << "PlaceObject2: ColorTransform: HasMultTerms: " << binOut.PlaceObject2.ColorTransform.HasMultTerms << "\n";

        if ( binOut.PlaceObject2.ColorTransform.HasMultTerms ) {

            std::cout << "PlaceObject2: ColorTransform: RedMultTerm: " << binOut.PlaceObject2.ColorTransform.RedMultTerm << "\n";
            std::cout << "PlaceObject2: ColorTransform: GreenMultTerm: " << binOut.PlaceObject2.ColorTransform.GreenMultTerm << "\n";
            std::cout << "PlaceObject2: ColorTransform: BlueMultTerm: " << binOut.PlaceObject2.ColorTransform.BlueMultTerm << "\n";
            std::cout << "PlaceObject2: ColorTransform: AlphaMultTerm: " << binOut.PlaceObject2.ColorTransform.AlphaMultTerm << "\n";

        }

        if ( binOut.PlaceObject2.ColorTransform.HasAddTerms ) {

            std::cout << "PlaceObject2: ColorTransform: RedAddTerm: " << binOut.PlaceObject2.ColorTransform.RedAddTerm << "\n";
            std::cout << "PlaceObject2: ColorTransform: GreenAddTerm: " << binOut.PlaceObject2.ColorTransform.GreenAddTerm << "\n";
            std::cout << "PlaceObject2: ColorTransform: BlueAddTerm: " << binOut.PlaceObject2.ColorTransform.BlueAddTerm << "\n";
            std::cout << "PlaceObject2: ColorTransform: AlphaAddTerm: " << binOut.PlaceObject2.ColorTransform.AlphaAddTerm << "\n";

        }

    }
        
    if ( binOut.PlaceObject2.PlaceFlagHasRatio ) {

        binOut.PlaceObject2.Ratio = ((rawTag.tagData[1] << 8) & rawTag.tagData[0]);
        SWFShift(rawTag.tagData, 2);
        std::cout << "PlaceObject2: Ratio: " << (int)binOut.PlaceObject2.Ratio << "\n";

    }

    if ( binOut.PlaceObject2.PlaceFlagHasName ) {

        binOut.PlaceObject2.Name = getString(rawTag.tagData);
        std::cout << "PlaceObject2: Name: " << binOut.PlaceObject2.Name << "\n";

    }

    if ( binOut.PlaceObject2.PlaceFlagHasClipDepth ) {

        binOut.PlaceObject2.ClipDepth = ((rawTag.tagData[1] << 8) & rawTag.tagData[0]);
        SWFShift(rawTag.tagData, 2);
        std::cout << "PlaceObject2: ClipDepth: " << (int)binOut.PlaceObject2.ClipDepth << "\n";

    }

    if ( binOut.PlaceObject2.PlaceFlagHasClipActions ) {
        
        std::cout << "PlaceObject2: Clip actions is not a supported feature is Action Script is not currently implemented!\n";
        exit(1);

    }


    return binOut;
}