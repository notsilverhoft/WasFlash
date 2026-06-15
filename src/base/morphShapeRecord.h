#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "matrix.h"
#include "gradient.h"

struct MORPHFILLSTYLE {
    uint8_t FillStyleType;
    uint8_t StartColorRed, StartColorGreen, StartColorBlue, StartColorAlpha;
    uint8_t EndColorRed, EndColorGreen, EndColorBlue, EndColorAlpha;
    MATRIX StartGradientMatrix;
    MATRIX EndGradientMatrix;
    MORPHGRADIENT Gradient;
    uint16_t BitmapID;
    MATRIX StartBitmapMatrix;
    MATRIX EndBitmapMatrix;
};

struct MORPHFILLSTYLEARRAY {
    uint8_t FillStyleCount;
    uint16_t FillStyleCountExtended;
    std::vector<MORPHFILLSTYLE> FillStyles;
};

struct MORPHLINESTYLE {
    uint16_t StartWidth;
    uint16_t EndWidth;
    uint8_t StartColorRed, StartColorGreen, StartColorBlue, StartColorAlpha;
    uint8_t EndColorRed, EndColorGreen, EndColorBlue, EndColorAlpha;
};

struct MORPHLINESTYLE2 {
    uint16_t StartWidth;
    uint16_t EndWidth;
    uint8_t StartCapStyle;
    uint8_t JoinStyle;
    bool HasFillFlag;
    bool NoHScaleFlag;
    bool NoVScaleFlag;
    bool PixelHintingFlag;
    uint8_t Reserved;
    bool NoClose;
    uint8_t EndCapStyle;
    float MiterLimitFactor;
    uint8_t StartColorRed, StartColorGreen, StartColorBlue, StartColorAlpha;
    uint8_t EndColorRed, EndColorGreen, EndColorBlue, EndColorAlpha;
    MORPHFILLSTYLE FillType;
};

struct MORPHLINESTYLEARRAY {
    uint8_t LineStyleCount;
    uint16_t LineStyleCountExtended; 
    std::vector<MORPHLINESTYLE> LineStyles;
    std::vector<MORPHLINESTYLE2> LineStyles2;
};

MORPHFILLSTYLE getMorphFillStyle(std::vector<uint8_t>& data);

MORPHFILLSTYLEARRAY getMorphFillStyleArray(std::vector<uint8_t>& data);

MORPHLINESTYLE getMorphLineStyle(std::vector<uint8_t>& data);

MORPHLINESTYLE2 getMorphLineStyle2(std::vector<uint8_t>& data);

MORPHLINESTYLEARRAY getMorphLineStyleArray(std::vector<uint8_t>& data, int morphShapeVersion);