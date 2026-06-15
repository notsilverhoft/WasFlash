#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "morphShapeRecord.h"
#include "gradient.h"
#include "../utils/trackSWF.h"
#include "../utils/bitStream.hpp"

MORPHFILLSTYLE getMorphFillStyle(std::vector<uint8_t>& data) {

    MORPHFILLSTYLE binOut;

    binOut.FillStyleType = data[0];
    SWFShift(data, 1);

    if ( binOut.FillStyleType == 0x00 ) {

        binOut.StartColorRed = data[0];
        binOut.StartColorGreen = data[1];
        binOut.StartColorBlue = data[2];
        binOut.StartColorAlpha = data[3];
        binOut.EndColorRed = data[4];
        binOut.EndColorGreen = data[5];
        binOut.EndColorBlue = data[6];
        binOut.EndColorAlpha = data[7];
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": StartColorRed: " << (int)binOut.StartColorRed << "\n";
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": StartColorGreen: " << (int)binOut.StartColorGreen << "\n";
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": StartColorBlue: " << (int)binOut.StartColorBlue << "\n";
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": StartColorAlpha: " << (int)binOut.StartColorAlpha << "\n";
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": EndColorRed: " << (int)binOut.EndColorRed << "\n";
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": EndColorGreen: " << (int)binOut.EndColorGreen << "\n";
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": EndColorBlue: " << (int)binOut.EndColorBlue << "\n";
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": EndColorAlpha: " << (int)binOut.EndColorAlpha << "\n";
        SWFShift(data, 8);

    }

    if ( binOut.FillStyleType == 0x10 | binOut.FillStyleType == 0x12 ) {

        binOut.StartGradientMatrix = getMatrix(data);
        binOut.EndGradientMatrix = getMatrix(data);
        binOut.Gradient = getMorphGradient(data);

    }

    if ( binOut.FillStyleType == 0x40 | binOut.FillStyleType == 0x41 | binOut.FillStyleType == 0x42 | binOut.FillStyleType == 0x43 ) {

        binOut.BitmapID = ((data[1] << 8) | data[0]);
        // std::cout << "MorphFillStyle: Type: "  << (int)binOut.FillStyleType << ": BitmapID: " << (int)binOut.BitmapID << "\n";
        SWFShift(data, 2);
        binOut.StartBitmapMatrix = getMatrix(data);
        binOut.EndBitmapMatrix = getMatrix(data);

    }

    return binOut;

}


MORPHFILLSTYLEARRAY getMorphFillStyleArray(std::vector<uint8_t>& data) {

    MORPHFILLSTYLEARRAY binOut;

    binOut.FillStyleCount = data[0];
    SWFShift(data, 1);

    if ( binOut.FillStyleCount == 0xFF ) {

        binOut.FillStyleCountExtended = ((data[1] << 8) | data[0]);
        // std::cout << "MorphFillStyleArray: FillStyleCount: "  << (int)binOut.FillStyleCountExtended << "\n";
        SWFShift(data, 2);

        binOut.FillStyles.resize(binOut.FillStyleCountExtended);

        for ( int i = 0; i < binOut.FillStyleCountExtended; i++ ) {

            binOut.FillStyles[i] = getMorphFillStyle(data);

        }

        binOut.FillStyleCount = binOut.FillStyleCountExtended;

    }

    else {

        binOut.FillStyles.resize(binOut.FillStyleCount);
        // std::cout << "MorphFillStyleArray: FillStyleCount "  << (int)binOut.FillStyleCount << "\n";

        for ( int i = 0; i < binOut.FillStyleCount; i++ ) {

            binOut.FillStyles[i] = getMorphFillStyle(data);

        }

    }

    return binOut;

}

MORPHLINESTYLE getMorphLineStyle(std::vector<uint8_t>& data) {

    MORPHLINESTYLE binOut;

    binOut.StartWidth = ((data[1] << 8) | data[0]);
    binOut.EndWidth = ((data[3] << 8) | data[2]);
    // std::cout << "MorphLineStyle: StartWidth: " << (int)binOut.StartWidth << "\n";
    // std::cout << "MorphLineStyle: EndWidth: " << (int)binOut.EndWidth << "\n";
    SWFShift(data, 4);

    binOut.StartColorRed = data[0];
    binOut.StartColorGreen = data[1];
    binOut.StartColorBlue = data[2];
    binOut.StartColorAlpha = data[3];
    binOut.EndColorRed = data[4];
    binOut.EndColorGreen = data[5];
    binOut.EndColorBlue = data[6];
    binOut.EndColorAlpha = data[7];
    // std::cout << "MorphLineStyle: StartColorRed: " << (int)binOut.StartColorRed << "\n";
    // std::cout << "MorphLineStyle: StartColorGreen: " << (int)binOut.StartColorGreen << "\n";
    // std::cout << "MorphLineStyle: StartColorBlue: " << (int)binOut.StartColorBlue << "\n";
    // std::cout << "MorphLineStyle: StartColorAlpha: " << (int)binOut.StartColorAlpha << "\n";
    // std::cout << "MorphLineStyle: EndColorRed: " << (int)binOut.EndColorRed << "\n";
    // std::cout << "MorphLineStyle: EndColorGreen: " << (int)binOut.EndColorGreen << "\n";
    // std::cout << "MorphLineStyle: EndColorBlue: " << (int)binOut.EndColorBlue << "\n";
    // std::cout << "MorphLineStyle: EndColorAlpha: " << (int)binOut.EndColorAlpha << "\n";
    SWFShift(data, 8);

    return binOut;

}

MORPHLINESTYLE2 getMorphLineStyle2(std::vector<uint8_t>& data)  {

    MORPHLINESTYLE2 binOut;

    binOut.StartWidth = ((data[1] << 8) | data[0]);
    binOut.EndWidth = ((data[3] << 8) | data[2]);
    // std::cout << "MorphLineStyle2: StartWidth: " << (int)binOut.StartWidth << "\n";
    // std::cout << "MorphLineStyle2: EndWidth: " << (int)binOut.EndWidth << "\n";
    SWFShift(data, 4);

    binOut.StartCapStyle = (data[0] >> 6);
    binOut.JoinStyle = ((data[0] >> 4) & 0x03);
    binOut.HasFillFlag = ((data[0] >> 3) & 0x01);
    binOut.NoHScaleFlag = ((data[0] >> 2) & 0x01);
    binOut.NoVScaleFlag = ((data[0] >> 1) & 0x01);
    binOut.PixelHintingFlag = (data[0] & 0x01);
    // std::cout << "MorphLineStyle2: StartCapStyle: " << (int)binOut.StartCapStyle << "\n";
    // std::cout << "MorphLineStyle2: JoinStyle: " << (int)binOut.JoinStyle << "\n";
    // std::cout << "MorphLineStyle2: HasFillFlag: " << (int)binOut.HasFillFlag << "\n";
    // std::cout << "MorphLineStyle2: NoHScaleFlag: " << (int)binOut.NoHScaleFlag << "\n";
    // std::cout << "MorphLineStyle2: NoVScaleFlag: " << (int)binOut.NoVScaleFlag << "\n";
    // std::cout << "MorphLineStyle2: PixelHintingFlag: " << (int)binOut.PixelHintingFlag << "\n";
    SWFShift(data, 1);

    binOut.Reserved = (data[0] >> 3);
    binOut.NoClose = ((data[0] >> 2) & 0x01);
    binOut.EndCapStyle = ((data[0] >> 1) & 0x03);
    // std::cout << "MorphLineStyle2: Reserved(Should Be 0): " << (int)binOut.Reserved << "\n";
    // std::cout << "MorphLineStyle2: NoClose: " << (int)binOut.NoClose << "\n";
    // std::cout << "MorphLineStyle2: EndCapStyle: " << (int)binOut.EndCapStyle << "\n";
    SWFShift(data, 1);

    if ( binOut.JoinStyle == 0x02 ) {

        binOut.MiterLimitFactor = static_cast<float>((static_cast<int16_t>((data[1] << 8) | data[0])) / 256.0f);
        // std::cout << "MorphLineStyle2: MiterLimitFactor: " << (int)binOut.MiterLimitFactor << "\n";
        SWFShift(data, 2);

    }

    if ( !binOut.HasFillFlag ) {

        binOut.StartColorRed = data[0];
        binOut.StartColorGreen = data[1];
        binOut.StartColorBlue = data[2];
        binOut.StartColorAlpha = data[3];
        binOut.EndColorRed = data[4];
        binOut.EndColorGreen = data[5];
        binOut.EndColorBlue = data[6];
        binOut.EndColorAlpha = data[7];
        // std::cout << "MorphLineStyle2: StartColorRed: " << (int)binOut.StartColorRed << "\n";
        // std::cout << "MorphLineStyle2: StartColorGreen: " << (int)binOut.StartColorGreen << "\n";
        // std::cout << "MorphLineStyle2: StartColorBlue: " << (int)binOut.StartColorBlue << "\n";
        // std::cout << "MorphLineStyle2: StartColorAlpha: " << (int)binOut.StartColorAlpha << "\n";
        // std::cout << "MorphLineStyle2: EndColorRed: " << (int)binOut.EndColorRed << "\n";
        // std::cout << "MorphLineStyle2: EndColorGreen: " << (int)binOut.EndColorGreen << "\n";
        // std::cout << "MorphLineStyle2: EndColorBlue: " << (int)binOut.EndColorBlue << "\n";
        // std::cout << "MorphLineStyle2: EndColorAlpha: " << (int)binOut.EndColorAlpha << "\n";
        SWFShift(data, 8);

    }

    else {

        binOut.FillType = getMorphFillStyle(data);

    }

    return binOut;

}

MORPHLINESTYLEARRAY getMorphLineStyleArray(std::vector<uint8_t>& data, int morphShapeVersion) {

    MORPHLINESTYLEARRAY binOut;

    binOut.LineStyleCount = data[0];
    SWFShift(data, 1);

    if ( binOut.LineStyleCount == 0 ) {

        return {};

    }

    if ( morphShapeVersion == 1) {

        if ( binOut.LineStyleCount == 0xFF ) {

            binOut.LineStyleCountExtended = ((data[1] << 8) | data[0]);
            // std::cout << "MorphLineStyleArray: LineStyleCount: "  << (int)binOut.LineStyleCountExtended << "\n";
            SWFShift(data, 2);

            binOut.LineStyles.resize(binOut.LineStyleCountExtended);

            for ( int i = 0; i < binOut.LineStyleCountExtended; i++ ) {

                binOut.LineStyles[i] = getMorphLineStyle(data);

            }

            binOut.LineStyleCount = binOut.LineStyleCountExtended;
            
        }

        else {

            // std::cout << "MorphLineStyleArray: LineStyleCount: "  << (int)binOut.LineStyleCount << "\n";

            binOut.LineStyles.resize(binOut.LineStyleCount);

            for ( int i = 0; i < binOut.LineStyleCount; i++ ) {

                binOut.LineStyles[i] = getMorphLineStyle(data);

            }

        }


    }

    if ( morphShapeVersion == 2 ) {

        if ( binOut.LineStyleCount == 0xFF ) {

            binOut.LineStyleCountExtended = ((data[1] << 8) | data[0]);
            // std::cout << "MorphLineStyleArray: LineStyleCount: "  << (int)binOut.LineStyleCountExtended << "\n";
            SWFShift(data, 2);

            binOut.LineStyles.resize(binOut.LineStyleCountExtended);

            for ( int i = 0; i < binOut.LineStyleCountExtended; i++ ) {

                binOut.LineStyles2[i] = getMorphLineStyle2(data);

            }

            binOut.LineStyleCount = binOut.LineStyleCountExtended;
            
        }

        else {

            // std::cout << "MorphLineStyleArray: LineStyleCount: "  << (int)binOut.LineStyleCount << "\n";

            binOut.LineStyles.resize(binOut.LineStyleCount);

            for ( int i = 0; i < binOut.LineStyleCount; i++ ) {

                binOut.LineStyles2[i] = getMorphLineStyle2(data);

            }

        }

    }

    return binOut;

}