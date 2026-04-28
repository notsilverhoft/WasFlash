// shapeProcessor.cpp
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <unordered_map>
#include <chrono>
#include "shapeProcessor.h"
#include "../../include/skia/include/core/SkPaint.h"
#include "../../include/skia/include/core/SkPathBuilder.h"
#include "../../include/skia/include/effects/SkGradient.h"
#include "../../include/skia/include/effects/SkColorMatrix.h"
#include "../../include/skia/include/core/SkColorFilter.h"
#include "../base/rect.h"
#include "../base/shapeWithStyle.h"
#include "../base/shapeRecord.h"
#include "../base/colorTransformAlpha.h"

std::vector<SkPaint> getSkiaFills(std::vector<FILLSTYLE> rawFills, int shapeVersion) {

    std::vector<SkPaint> binOut;
    binOut.resize(rawFills.size());

    for (int i = 0; i < rawFills.size(); i++) {

        FILLSTYLE CurrentFillStyle = rawFills[i];

        if ( CurrentFillStyle.FillStyleType == 0x00 ) {

            if ( shapeVersion < 3 ) {

                binOut[i].setColor(SkColorSetRGB(CurrentFillStyle.Red, CurrentFillStyle.Green, CurrentFillStyle.Blue));

            }

            else {

                binOut[i].setColor(SkColorSetARGB(CurrentFillStyle.Alpha, CurrentFillStyle.Red, CurrentFillStyle.Green, CurrentFillStyle.Blue));

            }

        }

        else if ( CurrentFillStyle.FillStyleType == 0x10 || CurrentFillStyle.FillStyleType == 0x12 || CurrentFillStyle.FillStyleType == 0x13 ) {

            std::vector<GRADRECORD>& records = (CurrentFillStyle.FillStyleType == 0x13)
                ? CurrentFillStyle.FocalGradient.GradientRecords
                : CurrentFillStyle.Gradient.GradientRecords;

            uint8_t spreadMode = (CurrentFillStyle.FillStyleType == 0x13)
                ? CurrentFillStyle.FocalGradient.SpreadMode
                : CurrentFillStyle.Gradient.SpreadMode;

            if ( records.empty() ) continue;

            std::vector<SkColor4f> colors;
            std::vector<float>     positions;

            for (auto& stop : records) {

                SkColor4f color;

                if ( shapeVersion < 3 ) {
                    color = SkColor4f::FromColor(SkColorSetRGB(stop.Red, stop.Green, stop.Blue));
                } else {
                    color = SkColor4f::FromColor(SkColorSetARGB(stop.Alpha, stop.Red, stop.Green, stop.Blue));
                }

                colors.push_back(color);
                positions.push_back(stop.Ratio / 255.0f);

            }

            SkTileMode tileMode;

            switch ( spreadMode ) {
                case 1:  tileMode = SkTileMode::kMirror; break;
                case 2:  tileMode = SkTileMode::kRepeat; break;
                default: tileMode = SkTileMode::kClamp;  break;
            }

            SkGradient::Colors gradColors(
                SkSpan<const SkColor4f>(colors.data(), colors.size()),
                SkSpan<const float>(positions.data(), positions.size()),
                tileMode
            );

            SkGradient grad(gradColors, SkGradient::Interpolation{});

            MATRIX& gm = CurrentFillStyle.GradientMatrix;

            SkMatrix gradMatrix;
            gradMatrix.setAll(
                gm.ScaleX / 20.0f,      gm.RotateSkew1 / 20.0f,  gm.TranslateX / 20.0f,
                gm.RotateSkew0 / 20.0f, gm.ScaleY / 20.0f,        gm.TranslateY / 20.0f,
                0,                       0,                         1
            );

            sk_sp<SkShader> shader;

            if ( CurrentFillStyle.FillStyleType == 0x10 ) {

                SkPoint pts[2] = {{-1.0f, 0.0f}, {1.0f, 0.0f}};
                shader = SkShaders::LinearGradient(pts, grad, &gradMatrix);

            } else if ( CurrentFillStyle.FillStyleType == 0x12 ) {

                shader = SkShaders::RadialGradient({0.0f, 0.0f}, 1.0f, grad, &gradMatrix);

            } else {

                float fp = CurrentFillStyle.FocalGradient.FocalPoint;
                shader = SkShaders::TwoPointConicalGradient(
                    {fp, 0.0f}, 0.0f,
                    {0.0f, 0.0f}, 1.0f,
                    grad, &gradMatrix
                );

            }

            if ( shader ) {
                binOut[i].setShader(shader);
            }

        }

    }

    return binOut;

}

std::vector<SkPaint> getSkiaLines(std::vector<LINESTYLE> rawLines, std::vector<LINESTYLE2> rawLines2, int shapeVersion) {

    std::vector<SkPaint> binOut;

    if ( shapeVersion < 4 ) {

        binOut.resize(rawLines.size());

        for (int i = 0; i < (int)rawLines.size(); i++) {

            LINESTYLE& ls = rawLines[i];

            if ( shapeVersion < 3 ) {
                binOut[i].setColor(SkColorSetRGB(ls.Red, ls.Green, ls.Blue));
            } else {
                binOut[i].setColor(SkColorSetARGB(ls.Alpha, ls.Red, ls.Green, ls.Blue));
            }

            binOut[i].setStyle(SkPaint::kStroke_Style);
            binOut[i].setStrokeWidth(ls.Width / 20.0f);
            binOut[i].setStrokeCap(SkPaint::kRound_Cap);
            binOut[i].setStrokeJoin(SkPaint::kRound_Join);

        }

    }

    else {

        binOut.resize(rawLines2.size());

        for (int i = 0; i < (int)rawLines2.size(); i++) {

            LINESTYLE2& ls = rawLines2[i];

            if ( ls.HasFillFlag ) {

                std::vector<FILLSTYLE> fillVec = { ls.FillType };
                std::vector<SkPaint> fillPaint = getSkiaFills(fillVec, shapeVersion);
                binOut[i] = fillPaint[0];

            } else {

                binOut[i].setColor(SkColorSetARGB(ls.Alpha, ls.Red, ls.Green, ls.Blue));

            }

            binOut[i].setStyle(SkPaint::kStroke_Style);
            binOut[i].setStrokeWidth(ls.Width / 20.0f);

            switch ( ls.StartCapStyle ) {
                case 0: binOut[i].setStrokeCap(SkPaint::kRound_Cap);  break;
                case 1: binOut[i].setStrokeCap(SkPaint::kButt_Cap);   break;
                case 2: binOut[i].setStrokeCap(SkPaint::kSquare_Cap); break;
                default: binOut[i].setStrokeCap(SkPaint::kRound_Cap); break;
            }

            switch ( ls.JoinStyle ) {
                case 0: binOut[i].setStrokeJoin(SkPaint::kRound_Join); break;
                case 1: binOut[i].setStrokeJoin(SkPaint::kBevel_Join); break;
                case 2:
                    binOut[i].setStrokeJoin(SkPaint::kMiter_Join);
                    binOut[i].setStrokeMiter(ls.MiterLimitFactor);
                break;
                default: binOut[i].setStrokeJoin(SkPaint::kRound_Join); break;
            }

        }

    }

    return binOut;

}

void applyColorTransform(Shape& shape, const CXFORMWITHALPHA& ct) {

    float multR = ct.HasMultTerms ? ct.RedMultTerm   / 256.0f : 1.0f;
    float multG = ct.HasMultTerms ? ct.GreenMultTerm / 256.0f : 1.0f;
    float multB = ct.HasMultTerms ? ct.BlueMultTerm  / 256.0f : 1.0f;
    float multA = ct.HasMultTerms ? ct.AlphaMultTerm / 256.0f : 1.0f;

    float addR = ct.HasAddTerms ? ct.RedAddTerm   / 255.0f : 0.0f;
    float addG = ct.HasAddTerms ? ct.GreenAddTerm / 255.0f : 0.0f;
    float addB = ct.HasAddTerms ? ct.BlueAddTerm  / 255.0f : 0.0f;
    float addA = ct.HasAddTerms ? ct.AlphaAddTerm / 255.0f : 0.0f;

    float matrix[20] = {
        multR, 0,     0,     0,     addR,
        0,     multG, 0,     0,     addG,
        0,     0,     multB, 0,     addB,
        0,     0,     0,     multA, addA
    };

    sk_sp<SkColorFilter> cf = SkColorFilters::Matrix(matrix);

    for (auto& paint : shape.Fills) paint.setColorFilter(cf);
    for (auto& paint : shape.Lines) paint.setColorFilter(cf);

}

// A single continuous series of points forming part of a path
struct PathSegment {
    std::vector<std::pair<int,int>> points;
    std::vector<bool> isBezierControl;

    void reset(int x, int y) {
        points.clear();
        isBezierControl.clear();
        points.push_back({x, y});
        isBezierControl.push_back(false);
    }

    void addPoint(int x, int y, bool bezierControl) {
        points.push_back({x, y});
        isBezierControl.push_back(bezierControl);
    }

    void flip() {
        std::reverse(points.begin(), points.end());
        std::reverse(isBezierControl.begin(), isBezierControl.end());
    }

    bool isEmpty() const {
        return points.size() <= 1;
    }

    std::pair<int,int> start() const { return points.front(); }
    std::pair<int,int> end() const { return points.back(); }
};

struct PendingPath {
    std::vector<PathSegment> segments;

    void addSegment(PathSegment newSeg) {

        if (newSeg.isEmpty()) return;

        bool startOpen = true;
        bool endOpen = true;
        int i = 0;

        while ((startOpen || endOpen) && i < (int)segments.size()) {

            PathSegment& other = segments[i];

            if (startOpen && other.end() == newSeg.start()) {
                for (int j = 1; j < (int)newSeg.points.size(); j++) {
                    other.points.push_back(newSeg.points[j]);
                    other.isBezierControl.push_back(newSeg.isBezierControl[j]);
                }
                newSeg = segments[i];
                segments.erase(segments.begin() + i);
                startOpen = false;

            } else if (endOpen && newSeg.end() == other.start()) {
                for (int j = 1; j < (int)other.points.size(); j++) {
                    newSeg.points.push_back(other.points[j]);
                    newSeg.isBezierControl.push_back(other.isBezierControl[j]);
                }
                segments.erase(segments.begin() + i);
                endOpen = false;

            } else {
                i++;
            }

        }

        segments.push_back(newSeg);

    }

    void pushStroke(PathSegment seg) {
        segments.push_back(seg);
    }

    void buildFillPath(SkPathBuilder& builder) {
        for (auto& seg : segments) {
            if (seg.isEmpty()) continue;
            builder.moveTo(seg.points[0].first, seg.points[0].second);
            int j = 1;
            while (j < (int)seg.points.size()) {
                if (seg.isBezierControl[j]) {
                    int cx = seg.points[j].first;
                    int cy = seg.points[j].second;
                    j++;
                    if (j < (int)seg.points.size()) {
                        builder.quadTo(cx, cy, seg.points[j].first, seg.points[j].second);
                        j++;
                    }
                } else {
                    builder.lineTo(seg.points[j].first, seg.points[j].second);
                    j++;
                }
            }
            builder.close();
        }
    }

    void buildLinePath(SkPathBuilder& builder) {
        for (auto& seg : segments) {
            if (seg.isEmpty()) continue;
            builder.moveTo(seg.points[0].first, seg.points[0].second);
            int j = 1;
            while (j < (int)seg.points.size()) {
                if (seg.isBezierControl[j]) {
                    int cx = seg.points[j].first;
                    int cy = seg.points[j].second;
                    j++;
                    if (j < (int)seg.points.size()) {
                        builder.quadTo(cx, cy, seg.points[j].first, seg.points[j].second);
                        j++;
                    }
                } else {
                    builder.lineTo(seg.points[j].first, seg.points[j].second);
                    j++;
                }
            }
        }
    }
};

struct ActivePath {
    int styleId = 0;
    PathSegment segment;

    void reset(int x, int y) {
        segment.reset(x, y);
    }

    void addPoint(int x, int y, bool bezierControl) {
        segment.addPoint(x, y, bezierControl);
    }

    void flushFill(int curX, int curY, std::vector<PendingPath>& pending, bool flip) {
        if (styleId > 0 && styleId <= (int)pending.size() && !segment.isEmpty()) {
            if (flip) segment.flip();
            pending[styleId - 1].addSegment(segment);
        }
        segment.reset(curX, curY);
    }

    void flushStroke(int curX, int curY, std::vector<PendingPath>& pending) {
        if (styleId > 0 && styleId <= (int)pending.size() && !segment.isEmpty()) {
            pending[styleId - 1].pushStroke(segment);
        }
        segment.reset(curX, curY);
    }
};

Shape getShape(RECT shapeBounds, SHAPEWITHSTYLE rawShape, int shapeVersion) {

    Shape binOut;

    binOut.Width = (shapeBounds.xMax - shapeBounds.xMin) / 20;
    binOut.Height = (shapeBounds.yMax - shapeBounds.yMin) / 20;

    std::vector<FILLSTYLE> FillStyles;
    std::vector<LINESTYLE> LineStyles;
    std::vector<LINESTYLE2> LineStyles2;

    FillStyles.resize(rawShape.FillStyles.FillStyleCount);
    for (int i = 0; i < (int)rawShape.FillStyles.FillStyleCount; i++) {
        FillStyles[i] = rawShape.FillStyles.FillStyles[i];
    }

    if (shapeVersion < 4) {
        LineStyles.resize(rawShape.LineStyles.LineStyleCount);
        for (int i = 0; i < (int)rawShape.LineStyles.LineStyleCount; i++) {
            LineStyles[i] = rawShape.LineStyles.LineStyles[i];
        }
    } else {
        LineStyles2.resize(rawShape.LineStyles.LineStyleCount);
        for (int i = 0; i < (int)rawShape.LineStyles.LineStyleCount; i++) {
            LineStyles2[i] = rawShape.LineStyles.LineStyles2[i];
        }
    }

    std::vector<PendingPath> fills(FillStyles.size());
    std::vector<PendingPath> strokes(shapeVersion < 4 ? LineStyles.size() : LineStyles2.size());

    std::vector<SkPath> FillPaths;
    std::vector<SkPath> LinePaths;
    std::vector<FILLSTYLE> FillStylesForPaths;
    std::vector<LINESTYLE> LineStylesForPaths;
    std::vector<LINESTYLE2> LineStyles2ForPaths;

    ActivePath fillStyle0;
    ActivePath fillStyle1;
    ActivePath lineStyle;

    int penX = 0;
    int penY = 0;

    auto flushPaths = [&]() {
        fillStyle1.flushFill(penX, penY, fills, false);
        fillStyle0.flushFill(penX, penY, fills, true);
        lineStyle.flushStroke(penX, penY, strokes);
    };

    auto flushLayer = [&]() {

        flushPaths();

        for (int i = 0; i < (int)fills.size(); i++) {
            if (fills[i].segments.empty()) continue;
            SkPathBuilder builder;
            builder.setFillType(SkPathFillType::kEvenOdd);
            fills[i].buildFillPath(builder);
            FillPaths.push_back(builder.detach());
            FillStylesForPaths.push_back(FillStyles[i]);
            fills[i].segments.clear();
        }

        for (int i = 0; i < (int)strokes.size(); i++) {
            if (strokes[i].segments.empty()) continue;
            SkPathBuilder builder;
            strokes[i].buildLinePath(builder);
            LinePaths.push_back(builder.detach());
            if (shapeVersion < 4) {
                LineStylesForPaths.push_back(LineStyles[i]);
            } else {
                LineStyles2ForPaths.push_back(LineStyles2[i]);
            }
            strokes[i].segments.clear();
        }

    };

    for (int recordIndex = 0; recordIndex < (int)rawShape.ShapeRecords.size(); recordIndex++) {

        SHAPERECORD& CurrentRecord = rawShape.ShapeRecords[recordIndex];

        if (CurrentRecord.NonEdgeRecords.ISENDRECORD) {
            break;
        }

        if (CurrentRecord.TypeFlag == 0) {

            if (CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.StateMoveTo) {
                flushPaths();
                penX = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.MoveDeltaX;
                penY = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.MoveDeltaY;
                fillStyle0.reset(penX, penY);
                fillStyle1.reset(penX, penY);
                lineStyle.reset(penX, penY);
            }

            if (CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.StateNewStyles) {
                flushLayer();

                FillStyles.clear();
                FillStyles.resize(CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.FillStyles.FillStyleCount);
                for (int i = 0; i < (int)CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.FillStyles.FillStyleCount; i++) {
                    FillStyles[i] = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.FillStyles.FillStyles[i];
                }

                if (shapeVersion < 4) {
                    LineStyles.clear();
                    LineStyles.resize(CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount);
                    for (int i = 0; i < (int)CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount; i++) {
                        LineStyles[i] = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyles[i];
                    }
                } else {
                    LineStyles2.clear();
                    LineStyles2.resize(CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount);
                    for (int i = 0; i < (int)CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount; i++) {
                        LineStyles2[i] = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyles2[i];
                    }
                }

                fills.assign(FillStyles.size(), PendingPath());
                strokes.assign(shapeVersion < 4 ? LineStyles.size() : LineStyles2.size(), PendingPath());

                fillStyle0.styleId = 0;
                fillStyle1.styleId = 0;
                lineStyle.styleId = 0;
            }

            if (CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle1) {
                fillStyle1.flushFill(penX, penY, fills, false);
                fillStyle1.styleId = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.FillStyle1;
            }

            if (CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle0) {
                fillStyle0.flushFill(penX, penY, fills, true);
                fillStyle0.styleId = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.FillStyle0;
            }

            if (CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.StateLineStyle) {
                lineStyle.flushStroke(penX, penY, strokes);
                lineStyle.styleId = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyle;
            }

        }

        if (CurrentRecord.TypeFlag == 1) {

            if (CurrentRecord.EdgeRecords.StraightFlag) {

                if (CurrentRecord.EdgeRecords.STRAIGHTEDGERECORD.GeneralLineFlag) {
                    penX += CurrentRecord.EdgeRecords.STRAIGHTEDGERECORD.GeneralLineDeltaX;
                    penY += CurrentRecord.EdgeRecords.STRAIGHTEDGERECORD.GeneralLineDeltaY;
                } else if (!CurrentRecord.EdgeRecords.STRAIGHTEDGERECORD.VertLineFlag) {
                    penX += CurrentRecord.EdgeRecords.STRAIGHTEDGERECORD.LineDeltaX;
                } else {
                    penY += CurrentRecord.EdgeRecords.STRAIGHTEDGERECORD.LineDeltaY;
                }

                if (fillStyle1.styleId > 0) fillStyle1.addPoint(penX, penY, false);
                if (fillStyle0.styleId > 0) fillStyle0.addPoint(penX, penY, false);
                if (lineStyle.styleId > 0)  lineStyle.addPoint(penX, penY, false);

            }

            if (!CurrentRecord.EdgeRecords.StraightFlag) {

                int cX = penX + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.ControlDeltaX;
                int cY = penY + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.ControlDeltaY;
                int eX = cX + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaX;
                int eY = cY + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaY;
                penX = eX;
                penY = eY;

                if (fillStyle1.styleId > 0) { fillStyle1.addPoint(cX, cY, true);  fillStyle1.addPoint(eX, eY, false); }
                if (fillStyle0.styleId > 0) { fillStyle0.addPoint(cX, cY, true);  fillStyle0.addPoint(eX, eY, false); }
                if (lineStyle.styleId > 0)  { lineStyle.addPoint(cX, cY, true);   lineStyle.addPoint(eX, eY, false); }

            }

        }

    }

    flushLayer();

    binOut.FillPaths = FillPaths;
    binOut.Fills = getSkiaFills(FillStylesForPaths, shapeVersion);
    binOut.LinePaths = LinePaths;
    binOut.Lines = getSkiaLines(LineStylesForPaths, LineStyles2ForPaths, shapeVersion);

    return binOut;

}

Shape transformShape(Shape ShapeIn, MATRIX TransformMatrix, RECT FrameSize) {

    Shape binOut;

    binOut.shapeID = ShapeIn.shapeID;
    binOut.FillPaths.resize(ShapeIn.FillPaths.size());
    binOut.LinePaths.resize(ShapeIn.LinePaths.size());
    binOut.Fills.resize(ShapeIn.Fills.size());
    binOut.Lines.resize(ShapeIn.Lines.size());

    int FrameX = ((FrameSize.xMax - FrameSize.xMin) / 20);
    int FrameY = ((FrameSize.yMax - FrameSize.yMin) / 20);

    SkMatrix SkiaTransformMatrix;
    SkiaTransformMatrix.setAll(

        TransformMatrix.ScaleX / 20, TransformMatrix.RotateSkew1 / 20, TransformMatrix.TranslateX / 20,
        TransformMatrix.RotateSkew0 / 20, TransformMatrix.ScaleY / 20, TransformMatrix.TranslateY / 20, 
        0,                           0,                      1

    );

    // Fills

    for (int i = 0; i < (int)ShapeIn.FillPaths.size(); i++) {

        binOut.FillPaths[i] = ShapeIn.FillPaths[i].makeTransform(SkiaTransformMatrix);

        binOut.Fills[i] = ShapeIn.Fills[i];

        sk_sp<SkShader> shader = sk_ref_sp(ShapeIn.Fills[i].getShader());

        if ( shader ) {
            binOut.Fills[i].setShader(shader->makeWithLocalMatrix(SkiaTransformMatrix));
        }

    }

    // Lines

    for (int i = 0; i < (int)ShapeIn.LinePaths.size(); i++) {

        binOut.LinePaths[i] = ShapeIn.LinePaths[i].makeTransform(SkiaTransformMatrix);

        binOut.Lines[i] = ShapeIn.Lines[i];

        sk_sp<SkShader> shader = sk_ref_sp(ShapeIn.Lines[i].getShader());

        if ( shader ) {
            binOut.Lines[i].setShader(shader->makeWithLocalMatrix(SkiaTransformMatrix));
        }

    }

    return binOut;

}