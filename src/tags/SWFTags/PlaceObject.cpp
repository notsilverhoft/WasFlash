#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../utils/trackSWF.h"
#include "../../base/string.h"

SWFTag getPlaceObjectTag(rawSWFTag& rawTag, int version) {
        
    SWFTag binOut;

    if ( version == 1 ) {

        binOut.PlaceObject.PlaceFlagHasCharacter = true;
        binOut.PlaceObject.PlaceFlagHasMatrix = true;
        binOut.PlaceObject.PlaceFlagMove = false;
        
        binOut.PlaceObject.PlaceFlagHasClipActions = false;
        binOut.PlaceObject.PlaceFlagHasClipDepth = false;
        binOut.PlaceObject.PlaceFlagHasName = false;
        binOut.PlaceObject.PlaceFlagHasRatio = false;

        binOut.PlaceObject.CharacterID = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
        SWFShift(rawTag.tagData, 2);

        binOut.PlaceObject.Depth = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
        SWFShift(rawTag.tagData, 2);

        binOut.PlaceObject.Matrix = getMatrix(rawTag.tagData);

        if ( rawTag.tagData.size() > 0 ) {

            binOut.PlaceObject.PlaceFlagHasColorTransform = true;
            binOut.PlaceObject.ColorTransform = getColorTransformWithAlpha(rawTag.tagData, 1);

        }

    }

    if ( version == 2 ) {

        uint8_t binOne = rawTag.tagData[0];

        binOut.PlaceObject.PlaceFlagHasClipActions = ((binOne >> 7) & 0x01);
        // std::cout << "PlaceObject: HasClipActions: " << binOut.PlaceObject.PlaceFlagHasClipActions << "\n";
        binOut.PlaceObject.PlaceFlagHasClipDepth = ((binOne >> 6) & 0x01);
        // std::cout << "PlaceObject: HasClipDepth: " << binOut.PlaceObject.PlaceFlagHasClipDepth << "\n";
        binOut.PlaceObject.PlaceFlagHasName = ((binOne >> 5) & 0x01);
        // std::cout << "PlaceObject: HasName: " << binOut.PlaceObject.PlaceFlagHasName << "\n";
        binOut.PlaceObject.PlaceFlagHasRatio = ((binOne >> 4) & 0x01);
        // std::cout << "PlaceObject: HasRatio: " << binOut.PlaceObject.PlaceFlagHasRatio << "\n";
        binOut.PlaceObject.PlaceFlagHasColorTransform = ((binOne >> 3) & 0x01);
        // std::cout << "PlaceObject: HasColorTransform: " << binOut.PlaceObject.PlaceFlagHasColorTransform << "\n";
        binOut.PlaceObject.PlaceFlagHasMatrix = ((binOne >> 2) & 0x01);
        // std::cout << "PlaceObject: HasMatrix: " << binOut.PlaceObject.PlaceFlagHasMatrix << "\n";
        binOut.PlaceObject.PlaceFlagHasCharacter = ((binOne >> 1) & 0x01);
        // std::cout << "PlaceObject: HasCharacter: " << binOut.PlaceObject.PlaceFlagHasCharacter << "\n";
        binOut.PlaceObject.PlaceFlagMove = (binOne & 0x01);
        // std::cout << "PlaceObject: Move: " << binOut.PlaceObject.PlaceFlagMove << "\n";

        SWFShift(rawTag.tagData, 1);

        binOut.PlaceObject.Depth = static_cast<uint16_t>((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
        
        // std::cout << "PlaceObject: Depth: " << (int)binOut.PlaceObject.Depth << "\n";


        SWFShift(rawTag.tagData, 2);

        if ( binOut.PlaceObject.PlaceFlagHasCharacter ) {

            binOut.PlaceObject.CharacterID = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
            SWFShift(rawTag.tagData, 2);
            // std::cout << "PlaceObject: CharacterID: " << (int)binOut.PlaceObject.CharacterID << "\n";

        }

        if ( binOut.PlaceObject.PlaceFlagHasMatrix ) {

            binOut.PlaceObject.Matrix = getMatrix(rawTag.tagData);
            // std::cout << "PlaceObject: Matrix: HasScale: " << (int)binOut.PlaceObject.Matrix.HasScale << "\n";
            if ( binOut.PlaceObject.Matrix.HasScale ) {

                // std::cout << "PlaceObject: Matrix: ScaleX: " << binOut.PlaceObject.Matrix.ScaleX << "\n";
                // std::cout << "PlaceObject: Matrix: ScaleY: " << binOut.PlaceObject.Matrix.ScaleY << "\n";

            }

            // std::cout << "PlaceObject: Matrix: HasRotate: " << (int)binOut.PlaceObject.Matrix.HasRotate << "\n";
            if ( binOut.PlaceObject.Matrix.HasRotate ) {

                // std::cout << "PlaceObject: Matrix: RotateSkew0: " << binOut.PlaceObject.Matrix.RotateSkew0 << "\n";
                // std::cout << "PlaceObject: Matrix: RotateSkew1: " << binOut.PlaceObject.Matrix.RotateSkew1 << "\n";

            }
            
            // std::cout << "PlaceObject: Matrix: TranslateX: " << binOut.PlaceObject.Matrix.TranslateX << "\n";
            // std::cout << "PlaceObject: Matrix: TranslateY: " << binOut.PlaceObject.Matrix.TranslateY << "\n";

        }

        else {

            binOut.PlaceObject.Matrix = {};

        }

        if ( binOut.PlaceObject.PlaceFlagHasColorTransform ) {

            binOut.PlaceObject.ColorTransform = getColorTransformWithAlpha(rawTag.tagData, 2);
            // std::cout << "PlaceObject: ColorTransform: HasAddTerms: " << binOut.PlaceObject.ColorTransform.HasAddTerms << "\n";
            // std::cout << "PlaceObject: ColorTransform: HasMultTerms: " << binOut.PlaceObject.ColorTransform.HasMultTerms << "\n";

            if ( binOut.PlaceObject.ColorTransform.HasMultTerms ) {

                // std::cout << "PlaceObject: ColorTransform: RedMultTerm: " << binOut.PlaceObject.ColorTransform.RedMultTerm << "\n";
                // std::cout << "PlaceObject: ColorTransform: GreenMultTerm: " << binOut.PlaceObject.ColorTransform.GreenMultTerm << "\n";
                // std::cout << "PlaceObject: ColorTransform: BlueMultTerm: " << binOut.PlaceObject.ColorTransform.BlueMultTerm << "\n";
                // std::cout << "PlaceObject: ColorTransform: AlphaMultTerm: " << binOut.PlaceObject.ColorTransform.AlphaMultTerm << "\n";

            }

            if ( binOut.PlaceObject.ColorTransform.HasAddTerms ) {

                // std::cout << "PlaceObject: ColorTransform: RedAddTerm: " << binOut.PlaceObject.ColorTransform.RedAddTerm << "\n";
                // std::cout << "PlaceObject: ColorTransform: GreenAddTerm: " << binOut.PlaceObject.ColorTransform.GreenAddTerm << "\n";
                // std::cout << "PlaceObject: ColorTransform: BlueAddTerm: " << binOut.PlaceObject.ColorTransform.BlueAddTerm << "\n";
                // std::cout << "PlaceObject: ColorTransform: AlphaAddTerm: " << binOut.PlaceObject.ColorTransform.AlphaAddTerm << "\n";

            }

        }
            
        if ( binOut.PlaceObject.PlaceFlagHasRatio ) {

            binOut.PlaceObject.Ratio = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
            SWFShift(rawTag.tagData, 2);
            // std::cout << "PlaceObject: Ratio: " << (int)binOut.PlaceObject.Ratio << "\n";

        }

        if ( binOut.PlaceObject.PlaceFlagHasName ) {

            binOut.PlaceObject.Name = getString(rawTag.tagData);
            // std::cout << "PlaceObject: Name: " << binOut.PlaceObject.Name << "\n";

        }

        if ( binOut.PlaceObject.PlaceFlagHasClipDepth ) {

            binOut.PlaceObject.ClipDepth = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
            SWFShift(rawTag.tagData, 2);
            // std::cout << "PlaceObject: ClipDepth: " << (int)binOut.PlaceObject.ClipDepth << "\n";

        }

        if ( binOut.PlaceObject.PlaceFlagHasClipActions ) {
            
            // std::cout << "PlaceObject: Clip actions is not a supported feature is Action Script is not currently implemented!\n";
            //exit(1);

        }

    }

    return binOut;
}