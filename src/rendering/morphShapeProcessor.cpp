#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../base/shapeRecord.h"
#include "../base/morphShapeRecord.h"
#include "shapeProcessor.h"
#include "../tags/tags.h"
#include "../../include/skia/include/core/SkPath.h"
#include "../../include/skia/include/core/SkPaint.h"
#include "../../include/skia/include/core/SkColor.h"
#include <cmath>

std::pair<GRADRECORD,GRADRECORD> MorphGradRecordToGradRecord(MORPHGRADRECORD input) {

    std::pair<GRADRECORD,GRADRECORD> binOut;

    binOut.first.Ratio = input.StartRatio;
    binOut.first.Red = input.StartColorRed;
    binOut.first.Green = input.StartColorGreen;
    binOut.first.Blue = input.StartColorBlue;
    binOut.first.Alpha = input.StartColorAlpha;

    binOut.second.Ratio = input.EndRatio;
    binOut.second.Red = input.EndColorRed;
    binOut.second.Green = input.EndColorGreen;
    binOut.second.Blue = input.EndColorBlue;
    binOut.second.Alpha = input.EndColorAlpha;

    return binOut;

}

std::pair<GRADIENT, GRADIENT> MorphGradientToGradient(MORPHGRADIENT input) {

    std::pair<GRADIENT, GRADIENT> binOut;

    binOut.first.SpreadMode = 0;
    binOut.first.InterpolationMode = 0;
    binOut.first.NumGradients = input.NumGradients;
    binOut.first.GradientRecords.resize(input.NumGradients);

    binOut.second.SpreadMode = 0;
    binOut.second.InterpolationMode = 0;
    binOut.second.NumGradients = input.NumGradients;
    binOut.second.GradientRecords.resize(input.NumGradients);

    for (int i = 0; i < input.NumGradients; i++) {

        binOut.first.GradientRecords[i] = MorphGradRecordToGradRecord(input.GradientRecords[i]).first;

        binOut.second.GradientRecords[i] = MorphGradRecordToGradRecord(input.GradientRecords[i]).second;

    }

    return binOut;

}

std::pair<FILLSTYLE, FILLSTYLE> MorphFillToFill(MORPHFILLSTYLE input) {

    std::pair<FILLSTYLE, FILLSTYLE> binOut;

    binOut.first.FillStyleType = input.FillStyleType;
    binOut.second.FillStyleType = input.FillStyleType;

    if ( input.FillStyleType == 0x00 ) {

        binOut.first.Red = input.StartColorRed;
        binOut.first.Green = input.StartColorGreen;
        binOut.first.Blue = input.StartColorBlue;
        binOut.first.Alpha = input.StartColorAlpha;

        binOut.second.Red = input.EndColorRed;
        binOut.second.Green = input.EndColorGreen;
        binOut.second.Blue = input.EndColorBlue;
        binOut.second.Alpha = input.EndColorAlpha;

    }

    if ( input.FillStyleType == 0x10 | input.FillStyleType == 0x12 ) {

        binOut.first.GradientMatrix = input.StartGradientMatrix;
        binOut.first.Gradient = MorphGradientToGradient(input.Gradient).first;

        binOut.second.GradientMatrix = input.EndGradientMatrix;
        binOut.second.Gradient = MorphGradientToGradient(input.Gradient).second;

    }

    if ( input.FillStyleType == 0x40 | input.FillStyleType == 0x41 | input.FillStyleType == 0x42 | input.FillStyleType == 0x43 ) {

        binOut.first.BitmapID = input.BitmapID;
        binOut.first.BitmapMatrix = input.StartBitmapMatrix;

        binOut.second.BitmapID = input.BitmapID;
        binOut.second.BitmapMatrix = input.EndBitmapMatrix;

    }

    return binOut;

}

std::pair<LINESTYLE,LINESTYLE> MorphLineToLine(MORPHLINESTYLE input) {

    std::pair<LINESTYLE,LINESTYLE> binOut;

    binOut.first.Width = input.StartWidth;
    binOut.first.Red = input.StartColorRed;
    binOut.first.Green = input.StartColorGreen;
    binOut.first.Blue = input.StartColorBlue;
    binOut.first.Alpha = input.StartColorAlpha;

    binOut.second.Width = input.EndWidth;
    binOut.second.Red = input.EndColorRed;
    binOut.second.Green = input.EndColorGreen;
    binOut.second.Blue = input.EndColorBlue;
    binOut.second.Alpha = input.EndColorAlpha;

    return binOut;

}

std::pair<LINESTYLE2, LINESTYLE2> MorphLine2ToLine2(MORPHLINESTYLE2 input) {

    std::pair<LINESTYLE2, LINESTYLE2> binOut;

    binOut.first.Width = input.StartWidth;
    binOut.first.StartCapStyle = input.StartCapStyle;
    binOut.first.JoinStyle = input.JoinStyle;
    binOut.first.HasFillFlag = input.HasFillFlag;
    binOut.first.NoHScaleFlag = input.NoHScaleFlag;
    binOut.first.NoVScaleFlag = input.NoVScaleFlag;
    binOut.first.PixelHintingFlag = input.PixelHintingFlag;
    binOut.first.NoClose = input.NoClose;
    binOut.first.EndCapStyle = input.EndCapStyle;

    binOut.second.Width = input.EndWidth;
    binOut.second.StartCapStyle = input.StartCapStyle;
    binOut.second.JoinStyle = input.JoinStyle;
    binOut.second.HasFillFlag = input.HasFillFlag;
    binOut.second.NoHScaleFlag = input.NoHScaleFlag;
    binOut.second.NoVScaleFlag = input.NoVScaleFlag;
    binOut.second.PixelHintingFlag = input.PixelHintingFlag;
    binOut.second.NoClose = input.NoClose;
    binOut.second.EndCapStyle = input.EndCapStyle;

    if ( input.JoinStyle == 0x02 ) {

        binOut.first.MiterLimitFactor = input.MiterLimitFactor;

        binOut.second.MiterLimitFactor = input.MiterLimitFactor;

    }

    if ( !input.HasFillFlag ) {

        binOut.first.Red = input.StartColorRed;
        binOut.first.Green = input.StartColorGreen;
        binOut.first.Blue = input.StartColorBlue;
        binOut.first.Alpha = input.StartColorAlpha;

        binOut.second.Red = input.EndColorRed;
        binOut.second.Green = input.EndColorGreen;
        binOut.second.Blue = input.EndColorBlue;
        binOut.second.Alpha = input.EndColorAlpha;

    }

    else {

        binOut.first.FillType = MorphFillToFill(input.FillType).first;

        binOut.second.FillType = MorphFillToFill(input.FillType).second;

    }

    return binOut;

}

std::pair<Shape, Shape> MorphShapeToShape(SWFTag input, int morphShapeVersion) {

    std::pair<Shape, Shape> binOut;

    std::vector<std::pair<FILLSTYLE, FILLSTYLE>> Fills(input.DefineMorphShape.MorphFillStyles.FillStyleCount);
    std::vector<std::pair<LINESTYLE, LINESTYLE>> Lines(input.DefineMorphShape.MorphLineStyles.LineStyleCount);
    std::vector<std::pair<LINESTYLE2, LINESTYLE2>> Lines2(input.DefineMorphShape.MorphLineStyles.LineStyleCount);

    std::vector<FILLSTYLE> Fills1(input.DefineMorphShape.MorphFillStyles.FillStyleCount);
    std::vector<FILLSTYLE> Fills2(input.DefineMorphShape.MorphFillStyles.FillStyleCount);
    std::vector<LINESTYLE> Lines_1(input.DefineMorphShape.MorphLineStyles.LineStyleCount);
    std::vector<LINESTYLE> Lines_2(input.DefineMorphShape.MorphLineStyles.LineStyleCount);
    std::vector<LINESTYLE2> Lines2_1(input.DefineMorphShape.MorphLineStyles.LineStyleCount);
    std::vector<LINESTYLE2> Lines2_2(input.DefineMorphShape.MorphLineStyles.LineStyleCount);

    for (int i = 0; i < input.DefineMorphShape.MorphFillStyles.FillStyleCount; i++) {

        Fills[i] = MorphFillToFill(input.DefineMorphShape.MorphFillStyles.FillStyles[i]);

        Fills1[i] = Fills[i].first;
        Fills2[i] = Fills[i].second;

    }

    // Match StartEdges and EndEdges StyleChangeRecords sequentially and patch
    // any missing fill/line style assignments in EndEdges record-by-record.
    {
        auto& startRecs = input.DefineMorphShape.StartEdges.ShapeRecords;
        auto& endRecs   = input.DefineMorphShape.EndEdges.ShapeRecords;

        std::vector<SHAPERECORD*> startSCRs, endSCRs;
        for (auto& rec : startRecs)
            if (rec.TypeFlag == 0 && !rec.NonEdgeRecords.ISENDRECORD)
                startSCRs.push_back(&rec);
        for (auto& rec : endRecs)
            if (rec.TypeFlag == 0 && !rec.NonEdgeRecords.ISENDRECORD)
                endSCRs.push_back(&rec);

        int pairCount = (int)std::min(startSCRs.size(), endSCRs.size());
        for (int i = 0; i < pairCount; i++) {
            auto& sr = startSCRs[i]->NonEdgeRecords.STYLECHANGERECORD;
            auto& er = endSCRs[i]->NonEdgeRecords.STYLECHANGERECORD;
            if (sr.StateFillStyle1 && !er.StateFillStyle1) { er.StateFillStyle1 = true; er.FillStyle1 = sr.FillStyle1; }
            if (sr.StateFillStyle0 && !er.StateFillStyle0) { er.StateFillStyle0 = true; er.FillStyle0 = sr.FillStyle0; }
            if (sr.StateLineStyle  && !er.StateLineStyle)  { er.StateLineStyle  = true; er.LineStyle  = sr.LineStyle;  }
        }

        // If EndEdges has fewer StyleChangeRecords, inject the missing ones
        for (int i = pairCount; i < (int)startSCRs.size(); i++) {
            auto& sr = startSCRs[i]->NonEdgeRecords.STYLECHANGERECORD;
            SHAPERECORD synth;
            synth.TypeFlag = 0;
            synth.NonEdgeRecords.ISENDRECORD = false;
            auto& er = synth.NonEdgeRecords.STYLECHANGERECORD;
            if (sr.StateFillStyle1) { er.StateFillStyle1 = true; er.FillStyle1 = sr.FillStyle1; }
            if (sr.StateFillStyle0) { er.StateFillStyle0 = true; er.FillStyle0 = sr.FillStyle0; }
            if (sr.StateLineStyle)  { er.StateLineStyle  = true; er.LineStyle  = sr.LineStyle;  }
            endRecs.insert(endRecs.begin() + i, synth);
        }
    }


    if ( morphShapeVersion == 1 ) {

        for (int i = 0; i < input.DefineMorphShape.MorphLineStyles.LineStyleCount; i++) {

            Lines[i] = MorphLineToLine(input.DefineMorphShape.MorphLineStyles.LineStyles[i]);

            Lines_1[i] = Lines[i].first;
            Lines_2[i] = Lines[i].second;

        }

        binOut.first  = getShape(input.DefineMorphShape.StartBounds, input.DefineMorphShape.StartEdges, 1, Fills1, Lines_1, Lines2_1);
        binOut.second = getShape(input.DefineMorphShape.EndBounds,   input.DefineMorphShape.EndEdges,   1, Fills2, Lines_2, Lines2_2);

    }

    if ( morphShapeVersion == 2 ) {

        for (int i = 0; i < input.DefineMorphShape.MorphLineStyles.LineStyleCount; i++) {

            Lines2[i] = MorphLine2ToLine2(input.DefineMorphShape.MorphLineStyles.LineStyles2[i]);

            Lines2_1[i] = Lines2[i].first;
            Lines2_2[i] = Lines2[i].second;

        }

        binOut.first  = getShape(input.DefineMorphShape.StartBounds, input.DefineMorphShape.StartEdges, 2, Fills1, Lines_1, Lines2_1);
        binOut.second = getShape(input.DefineMorphShape.EndBounds,   input.DefineMorphShape.EndEdges,   2, Fills2, Lines_2, Lines2_2);

    }

    // std::cout << "MorphShapeToShape: first.FillPaths=" << binOut.first.FillPaths.size() << "\n";
    // std::cout << "MorphShapeToShape: second.FillPaths=" << binOut.second.FillPaths.size() << "\n";
    // std::cout << "MorphShapeToShape: first.LinePaths=" << binOut.first.LinePaths.size() << "\n";
    // std::cout << "MorphShapeToShape: second.LinePaths=" << binOut.second.LinePaths.size() << "\n";
    // std::cout << "MorphShapeToShape: StartEdges.ShapeRecords=" << input.DefineMorphShape.StartEdges.ShapeRecords.size() << "\n";
    // std::cout << "MorphShapeToShape: EndEdges.ShapeRecords=" << input.DefineMorphShape.EndEdges.ShapeRecords.size() << "\n";
    // std::cout << "MorphShapeToShape: StartEdges.NumFillBits=" << (int)input.DefineMorphShape.StartEdges.NumFillBits << "\n";
    // std::cout << "MorphShapeToShape: EndEdges.NumFillBits=" << (int)input.DefineMorphShape.EndEdges.NumFillBits << "\n";

    return binOut;

}

Shape getMorphFrame(std::pair<Shape, Shape> Shapes, int ratio) {

    Shape binOut;

    float t = ratio / 65535.0f;

    // std::cout << "getMorphFrame: ratio=" << ratio << "\n";
    // std::cout << "getMorphFrame: first.FillPaths=" << Shapes.first.FillPaths.size() << "\n";
    // std::cout << "getMorphFrame: second.FillPaths=" << Shapes.second.FillPaths.size() << "\n";
    // std::cout << "getMorphFrame: first.LinePaths=" << Shapes.first.LinePaths.size() << "\n";
    // std::cout << "getMorphFrame: second.LinePaths=" << Shapes.second.LinePaths.size() << "\n";

    binOut.Width  = (int)std::lerp((float)Shapes.first.Width,  (float)Shapes.second.Width,  t);
    binOut.Height = (int)std::lerp((float)Shapes.first.Height, (float)Shapes.second.Height, t);

    int fillCount = (int)std::min(Shapes.first.FillPaths.size(), Shapes.second.FillPaths.size());
    for (int i = 0; i < (int)Shapes.first.FillPaths.size(); i++) {

        SkPath result;
        if (i < fillCount && Shapes.second.FillPaths[i].isInterpolatable(Shapes.first.FillPaths[i])) {
            Shapes.second.FillPaths[i].interpolate(Shapes.first.FillPaths[i], t, &result);
        } else {
            result = Shapes.first.FillPaths[i];
        }
        binOut.FillPaths.push_back(result);

        SkColor sc = Shapes.first.Fills[i].getColor();
        SkColor ec = i < fillCount ? Shapes.second.Fills[i].getColor() : sc;
        SkPaint lerped = Shapes.first.Fills[i];
        lerped.setColor(SkColorSetARGB(
            (uint8_t)std::lerp((float)SkColorGetA(sc), (float)SkColorGetA(ec), t),
            (uint8_t)std::lerp((float)SkColorGetR(sc), (float)SkColorGetR(ec), t),
            (uint8_t)std::lerp((float)SkColorGetG(sc), (float)SkColorGetG(ec), t),
            (uint8_t)std::lerp((float)SkColorGetB(sc), (float)SkColorGetB(ec), t)
        ));
        binOut.Fills.push_back(lerped);

    }

    int lineCount = (int)std::min(Shapes.first.LinePaths.size(), Shapes.second.LinePaths.size());
    for (int i = 0; i < (int)Shapes.first.LinePaths.size(); i++) {

        SkPath result;
        if (i < lineCount && Shapes.second.LinePaths[i].isInterpolatable(Shapes.first.LinePaths[i])) {
            Shapes.second.LinePaths[i].interpolate(Shapes.first.LinePaths[i], t, &result);
        } else {
            result = Shapes.first.LinePaths[i];
        }
        binOut.LinePaths.push_back(result);

        SkColor sc = Shapes.first.Lines[i].getColor();
        SkColor ec = i < lineCount ? Shapes.second.Lines[i].getColor() : sc;
        SkPaint lerped = Shapes.first.Lines[i];
        lerped.setColor(SkColorSetARGB(
            (uint8_t)std::lerp((float)SkColorGetA(sc), (float)SkColorGetA(ec), t),
            (uint8_t)std::lerp((float)SkColorGetR(sc), (float)SkColorGetR(ec), t),
            (uint8_t)std::lerp((float)SkColorGetG(sc), (float)SkColorGetG(ec), t),
            (uint8_t)std::lerp((float)SkColorGetB(sc), (float)SkColorGetB(ec), t)
        ));
        float sw1 = Shapes.first.Lines[i].getStrokeWidth();
        float sw2 = i < lineCount ? Shapes.second.Lines[i].getStrokeWidth() : sw1;
        lerped.setStrokeWidth(std::lerp(sw1, sw2, t));
        binOut.Lines.push_back(lerped);

    }

    return binOut;

}

SkMatrix transformMorphShape(std::pair<Shape, Shape>&, MATRIX& TransformMatrix) {
    SkMatrix binOut;
    binOut.setAll(
        TransformMatrix.ScaleX,       TransformMatrix.RotateSkew1,    TransformMatrix.TranslateX / 20.0f,
        TransformMatrix.RotateSkew0,  TransformMatrix.ScaleY,          TransformMatrix.TranslateY / 20.0f,
        0,                             0,                               1
    );
    return binOut;
}