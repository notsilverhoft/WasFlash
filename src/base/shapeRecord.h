#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "matrix.h"
#include "gradient.h"

struct FILLSTYLE {
    uint8_t FillStyleType = 0;
    uint8_t Red = 0, Green = 0, Blue = 0, Alpha = 0;
    MATRIX GradientMatrix = {};
    GRADIENT Gradient = {};
    FOCALGRADIENT FocalGradient = {};
    uint16_t BitmapID = 0;
    MATRIX BitmapMatrix = {};
};

struct FILLSTYLEARRAY {
    uint16_t FillStyleCount;
    uint16_t FillStyleCountExtended; // Only for define shape 2 and 3
    std::vector<FILLSTYLE> FillStyles;
};

struct LINESTYLE {
    uint16_t Width;
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Alpha;
};

struct LINESTYLE2 {
    uint16_t Width;
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
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Alpha;
    FILLSTYLE FillType;
};

struct LINESTYLEARRAY {
    uint16_t LineStyleCount;
    uint16_t LineStyleCountExtended;
    std::vector<LINESTYLE> LineStyles;
    std::vector<LINESTYLE2> LineStyles2;
};

struct SHAPERECORD {
    uint8_t TypeFlag;
    
    struct { // Non Edge

        bool ISENDRECORD = false;

        struct { // StyleChange
            bool StateNewStyles;
            bool StateLineStyle;
            bool StateFillStyle1;
            bool StateFillStyle0;
            bool StateMoveTo;
            uint8_t MoveBits;
            int MoveDeltaX;
            int MoveDeltaY;
            uint16_t FillStyle0;
            uint16_t FillStyle1;
            uint16_t LineStyle;
            FILLSTYLEARRAY FillStyles;
            LINESTYLEARRAY LineStyles;
            uint8_t NumFillBits;
            uint8_t NumLineBits;
        } STYLECHANGERECORD;

    } NonEdgeRecords;

    struct { // Edge

        bool StraightFlag;
        struct { // Straight
            uint8_t NumBits;
            bool GeneralLineFlag;
            int32_t GeneralLineDeltaX;
            int32_t GeneralLineDeltaY;
            bool VertLineFlag;
            int32_t LineDeltaX;
            int32_t LineDeltaY;
        } STRAIGHTEDGERECORD;

        struct { // Curved
            uint8_t NumBits;
            int32_t ControlDeltaX;
            int32_t ControlDeltaY;
            int32_t AnchorDeltaX;
            int32_t AnchorDeltaY;
        } CURVEDEDGERECORD;

    } EdgeRecords;

};

FILLSTYLE getFillStyle(std::vector<uint8_t>& data, int shapeVersion);

FILLSTYLEARRAY getFillStyleArray(std::vector<uint8_t>& data, int shapeVersion);

LINESTYLE getLineStyle(std::vector<uint8_t>& data, int shapeVersion);

LINESTYLE2 getLineStyle2(std::vector<uint8_t>& data, int shapeVersion);

LINESTYLEARRAY getLineStyleArray(std::vector<uint8_t>& data, int shapeVersion);

std::vector<SHAPERECORD> getShapeRecords(std::vector<uint8_t>& data, int shapeVersion, uint8_t& NumFillBits, uint8_t& NumLineBits);

