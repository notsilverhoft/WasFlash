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
#include "../../include/skia/include/core/SkPictureRecorder.h"
#include "../../include/skia/include/core/SkCanvas.h"
#include "../../include/skia/include/core/SkPathUtils.h"
#include "../base/rect.h"
#include "../base/shapeWithStyle.h"
#include "../base/shape.h"
#include "../base/shapeRecord.h"
#include "../base/colorTransformAlpha.h"

std::vector<SkPaint> getSkiaFills(const std::vector<FILLSTYLE>& rawFills, int shapeVersion) {

    std::vector<SkPaint> binOut;
    binOut.resize(rawFills.size());

    for (int i = 0; i < (int)rawFills.size(); i++) {

        FILLSTYLE CurrentFillStyle = rawFills[i];

        if ( CurrentFillStyle.FillStyleType == 0x00 ) {

            if ( shapeVersion < 3 ) {
                binOut[i].setColor(SkColorSetRGB(CurrentFillStyle.Red, CurrentFillStyle.Green, CurrentFillStyle.Blue));
            } else {
                binOut[i].setColor(SkColorSetARGB(CurrentFillStyle.Alpha, CurrentFillStyle.Red, CurrentFillStyle.Green, CurrentFillStyle.Blue));
            }

        } else if ( CurrentFillStyle.FillStyleType == 0x10 || CurrentFillStyle.FillStyleType == 0x12 || CurrentFillStyle.FillStyleType == 0x13 ) {

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

            uint8_t interpolationMode = (CurrentFillStyle.FillStyleType == 0x13)
                ? CurrentFillStyle.FocalGradient.InterpolationMode
                : CurrentFillStyle.Gradient.InterpolationMode;

            SkGradient::Interpolation interp{};
            if (interpolationMode == 1) {
                interp.fColorSpace = SkGradient::Interpolation::ColorSpace::kSRGBLinear;
            }

            SkGradient grad(gradColors, interp);

            MATRIX& gm = CurrentFillStyle.GradientMatrix;

            SkMatrix gradMatrix;
            gradMatrix.setAll(
                gm.ScaleX / 20.0f,       gm.RotateSkew1 / 20.0f,  gm.TranslateX / 20.0f,
                gm.RotateSkew0 / 20.0f,  gm.ScaleY / 20.0f,        gm.TranslateY / 20.0f,
                0,                        0,                          1
            );

            sk_sp<SkShader> shader;

            if ( CurrentFillStyle.FillStyleType == 0x10 ) {

                SkPoint pts[2] = {{-16384.0f, 0.0f}, {16384.0f, 0.0f}};
                gradMatrix.mapPoints(pts);
                shader = SkShaders::LinearGradient(pts, grad, nullptr);

            } else if ( CurrentFillStyle.FillStyleType == 0x12 ) {

                SkPoint radialPts[2] = {{0.0f, 0.0f}, {16384.0f, 0.0f}};
                gradMatrix.mapPoints(radialPts);
                float radius = SkPoint::Distance(radialPts[0], radialPts[1]);
                shader = SkShaders::RadialGradient(radialPts[0], radius, grad, nullptr);

            } else {

                float fp = CurrentFillStyle.FocalGradient.FocalPoint * 16384.0f;
                SkPoint focalPts[3] = {{fp, 0.0f}, {0.0f, 0.0f}, {16384.0f, 0.0f}};
                gradMatrix.mapPoints(focalPts);
                float radius = SkPoint::Distance(focalPts[1], focalPts[2]);
                shader = SkShaders::TwoPointConicalGradient(
                    focalPts[0], 0.0f,
                    focalPts[1], radius,
                    grad, nullptr
                );

            }

            if ( shader ) {
                binOut[i].setShader(shader);
            }

        }
        // Bitmap fills (0x40-0x43): not yet renderable — bitmap tags not parsed yet.

    }

    return binOut;

}

std::vector<SkPaint> getSkiaLines(const std::vector<LINESTYLE>& rawLines, const std::vector<LINESTYLE2>& rawLines2, int shapeVersion) {

    std::vector<SkPaint> binOut;

    if ( shapeVersion < 4 ) {

        binOut.resize(rawLines.size());

        for (int i = 0; i < (int)rawLines.size(); i++) {

            const LINESTYLE& ls = rawLines[i];

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

    } else {

        binOut.resize(rawLines2.size());

        for (int i = 0; i < (int)rawLines2.size(); i++) {

            const LINESTYLE2& ls = rawLines2[i];

            if ( ls.HasFillFlag ) {
                std::vector<FILLSTYLE> fillVec = { ls.FillType };
                std::vector<SkPaint> fillPaint = getSkiaFills(fillVec, shapeVersion);
                binOut[i] = fillPaint[0];
            } else {
                binOut[i].setColor(SkColorSetARGB(ls.Alpha, ls.Red, ls.Green, ls.Blue));
            }

            binOut[i].setStyle(SkPaint::kStroke_Style);
            binOut[i].setStrokeWidth(ls.Width / 20.0f);

            if (ls.StartCapStyle == ls.EndCapStyle) {
                switch ( ls.StartCapStyle ) {
                    case 0: binOut[i].setStrokeCap(SkPaint::kRound_Cap);  break;
                    case 1: binOut[i].setStrokeCap(SkPaint::kButt_Cap);   break;
                    case 2: binOut[i].setStrokeCap(SkPaint::kSquare_Cap); break;
                    default: binOut[i].setStrokeCap(SkPaint::kRound_Cap); break;
                }
            } else {
                binOut[i].setStrokeCap(SkPaint::kRound_Cap);
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

struct Point {
    int  x, y;
    bool isBezierControl;
};

struct PathSegment {
    std::vector<Point> points;

    void reset(int x, int y) {
        points.clear();
        points.push_back({x, y, false});
    }

    void addPoint(int x, int y, bool bezierControl) {
        points.push_back({x, y, bezierControl});
    }

    void flip() { std::reverse(points.begin(), points.end()); }

    bool isEmpty() const { return points.size() <= 1; }

    std::pair<int,int> start() const { return {points.front().x, points.front().y}; }
    std::pair<int,int> end()   const { return {points.back().x,  points.back().y};  }
};

struct PendingPath {
    std::vector<PathSegment> segments;

    void addSegment(PathSegment newSeg) {

        if (newSeg.isEmpty()) return;

        bool startOpen = true;
        bool endOpen   = true;
        int  i         = 0;

        while ((startOpen || endOpen) && i < (int)segments.size()) {

            PathSegment& other = segments[i];

            if (startOpen && other.end() == newSeg.start()) {
                other.points.insert(other.points.end(),
                                    newSeg.points.begin() + 1, newSeg.points.end());
                newSeg = std::move(segments[i]);
                std::swap(segments[i], segments.back());
                segments.pop_back();
                startOpen = false;
            } else if (endOpen && newSeg.end() == other.start()) {
                newSeg.points.insert(newSeg.points.end(),
                                     other.points.begin() + 1, other.points.end());
                std::swap(segments[i], segments.back());
                segments.pop_back();
                endOpen = false;
            } else {
                i++;
            }
        }

        segments.push_back(std::move(newSeg));
    }

    void pushStroke(PathSegment seg) { segments.push_back(std::move(seg)); }

    void buildFillPath(SkPathBuilder& builder) {
        for (auto& seg : segments) {
            if (seg.isEmpty()) continue;
            builder.moveTo(seg.points[0].x / 20.0f, seg.points[0].y / 20.0f);
            int j = 1;
            while (j < (int)seg.points.size()) {
                if (seg.points[j].isBezierControl) {
                    float cx = seg.points[j].x / 20.0f;
                    float cy = seg.points[j].y / 20.0f;
                    j++;
                    if (j < (int)seg.points.size()) {
                        builder.quadTo(cx, cy,
                                       seg.points[j].x / 20.0f,
                                       seg.points[j].y / 20.0f);
                        j++;
                    }
                } else {
                    builder.lineTo(seg.points[j].x / 20.0f, seg.points[j].y / 20.0f);
                    j++;
                }
            }
            builder.close();
        }
    }

    // noClose mirrors LINESTYLE2.NoClose — suppresses closing even on geometrically
    // closed segments. A segment is only closed if start==end AND !noClose (matching Ruffle).
    void buildLinePath(SkPathBuilder& builder, bool noClose = false) {
        for (auto& seg : segments) {
            if (seg.isEmpty()) continue;
            builder.moveTo(seg.points[0].x / 20.0f, seg.points[0].y / 20.0f);
            int j = 1;
            while (j < (int)seg.points.size()) {
                if (seg.points[j].isBezierControl) {
                    float cx = seg.points[j].x / 20.0f;
                    float cy = seg.points[j].y / 20.0f;
                    j++;
                    if (j < (int)seg.points.size()) {
                        builder.quadTo(cx, cy,
                                       seg.points[j].x / 20.0f,
                                       seg.points[j].y / 20.0f);
                        j++;
                    }
                } else {
                    builder.lineTo(seg.points[j].x / 20.0f, seg.points[j].y / 20.0f);
                    j++;
                }
            }
            // Close only if geometrically closed (start == end) and not suppressed by NoClose
            bool geometricallyClosed = (seg.start() == seg.end());
            if (geometricallyClosed && !noClose) builder.close();
        }
    }
};

struct ActivePath {
    int         styleId = 0;
    PathSegment segment;

    void reset(int x, int y) { segment.reset(x, y); }
    void addPoint(int x, int y, bool bezierControl) { segment.addPoint(x, y, bezierControl); }

    void flushFill(int curX, int curY, std::vector<PendingPath>& pending, bool flip, bool stitch = true) {
        if (styleId > 0 && styleId <= (int)pending.size() && !segment.isEmpty()) {
            if (flip) segment.flip();
            if (stitch) pending[styleId - 1].addSegment(std::move(segment));
            else        pending[styleId - 1].pushStroke(std::move(segment));
        }
        segment.reset(curX, curY);
    }

    void flushStroke(int curX, int curY, std::vector<PendingPath>& pending) {
        if (styleId > 0 && styleId <= (int)pending.size() && !segment.isEmpty()) {
            pending[styleId - 1].pushStroke(std::move(segment));
        }
        segment.reset(curX, curY);
    }
};

Shape getShape(RECT shapeBounds, const SHAPEWITHSTYLE& rawShape, int shapeVersion, bool usesFillWindingRule) {

    Shape binOut;

    if (rawShape.FillStyles.FillStyleCount > 255 ||
        rawShape.LineStyles.LineStyleCount > 255) {
        std::cerr << "getShape: corrupt style counts ("
                  << (int)rawShape.FillStyles.FillStyleCount << " fills, "
                  << (int)rawShape.LineStyles.LineStyleCount << " lines), bailing out\n";
        return binOut;
    }

    binOut.Width  = (shapeBounds.xMax - shapeBounds.xMin) / 20;
    binOut.Height = (shapeBounds.yMax - shapeBounds.yMin) / 20;

    std::vector<FILLSTYLE>  FillStyles;
    std::vector<LINESTYLE>  LineStyles;
    std::vector<LINESTYLE2> LineStyles2;

    FillStyles.resize(rawShape.FillStyles.FillStyleCount);
    for (int i = 0; i < (int)rawShape.FillStyles.FillStyleCount; i++)
        FillStyles[i] = rawShape.FillStyles.FillStyles[i];

    if (shapeVersion < 4) {
        LineStyles.resize(rawShape.LineStyles.LineStyleCount);
        for (int i = 0; i < (int)rawShape.LineStyles.LineStyleCount; i++)
            LineStyles[i] = rawShape.LineStyles.LineStyles[i];
    } else {
        LineStyles2.resize(rawShape.LineStyles.LineStyleCount);
        for (int i = 0; i < (int)rawShape.LineStyles.LineStyleCount; i++)
            LineStyles2[i] = rawShape.LineStyles.LineStyles2[i];
    }

    std::vector<PendingPath> fills(FillStyles.size());
    std::vector<PendingPath> strokes(shapeVersion < 4 ? LineStyles.size() : LineStyles2.size());

    ActivePath fillStyle0;
    ActivePath fillStyle1;
    ActivePath lineStyle;

    int penX = 0;
    int penY = 0;

    SkPictureRecorder recorder;
    SkRect picBounds = SkRect::MakeXYWH(
        shapeBounds.xMin / 20.0f, shapeBounds.yMin / 20.0f,
        binOut.Width,             binOut.Height
    );
    auto* recCanvas = recorder.beginRecording(picBounds);

    auto flushPaths = [&]() {
        fillStyle1.flushFill(penX, penY, fills, false);
        fillStyle0.flushFill(penX, penY, fills, true);
        lineStyle.flushStroke(penX, penY, strokes);
    };

    auto flushLayer = [&]() {

        flushPaths();

        // Fills for this layer
        std::vector<SkPath>    layerFillPaths;
        std::vector<FILLSTYLE> layerFillStyles;

        for (int i = 0; i < (int)fills.size(); i++) {
            if (fills[i].segments.empty()) continue;
            SkPathBuilder builder;
            builder.setFillType(usesFillWindingRule ? SkPathFillType::kWinding : SkPathFillType::kEvenOdd);
            fills[i].buildFillPath(builder);
            SkPath fp = builder.detach();
            layerFillPaths.push_back(fp);
            layerFillStyles.push_back(FillStyles[i]);
            fills[i].segments.clear();
        }

        auto layerFillPaints = getSkiaFills(layerFillStyles, shapeVersion);

        // Strokes for this layer
        std::vector<SkPath>    layerRawStrokePaths;
        std::vector<LINESTYLE>  layerLineStyles;
        std::vector<LINESTYLE2> layerLineStyles2;

        for (int i = 0; i < (int)strokes.size(); i++) {
            if (strokes[i].segments.empty()) continue;
            SkPathBuilder builder;
            bool noClose = (shapeVersion >= 4 && i < (int)LineStyles2.size())
                              ? LineStyles2[i].NoClose : false;
            strokes[i].buildLinePath(builder, noClose);
            SkPath sp = builder.detach();
            layerRawStrokePaths.push_back(sp);
            if (shapeVersion < 4) {
                layerLineStyles.push_back(LineStyles[i]);
            } else {
                layerLineStyles2.push_back(LineStyles2[i]);
            }
            strokes[i].segments.clear();
        }

        auto layerStrokePaints = getSkiaLines(layerLineStyles, layerLineStyles2, shapeVersion);

        // Tessellate strokes into filled paths
        std::vector<SkPath>  layerFilledStrokePaths;
        std::vector<SkPaint> layerFilledStrokePaints;

        for (int i = 0; i < (int)layerRawStrokePaths.size(); i++) {
            SkPath filled = skpathutils::FillPathWithPaint(layerRawStrokePaths[i], layerStrokePaints[i]);
            SkPaint fp    = layerStrokePaints[i];
            fp.setStyle(SkPaint::kFill_Style);
            layerFilledStrokePaths.push_back(std::move(filled));
            layerFilledStrokePaints.push_back(std::move(fp));
        }

        for (int i = 0; i < (int)layerFillPaths.size(); i++)
            recCanvas->drawPath(layerFillPaths[i], layerFillPaints[i]);
        for (int i = 0; i < (int)layerFilledStrokePaths.size(); i++)
            recCanvas->drawPath(layerFilledStrokePaths[i], layerFilledStrokePaints[i]);
    };

    // Shape records processing loop

    for (int recordIndex = 0; recordIndex < (int)rawShape.ShapeRecords.size(); recordIndex++) {

        const SHAPERECORD& CurrentRecord = rawShape.ShapeRecords[recordIndex];

        if (CurrentRecord.NonEdgeRecords.ISENDRECORD) break;

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
                for (int i = 0; i < (int)CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.FillStyles.FillStyleCount; i++)
                    FillStyles[i] = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.FillStyles.FillStyles[i];

                if (shapeVersion < 4) {
                    LineStyles.clear();
                    LineStyles.resize(CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount);
                    for (int i = 0; i < (int)CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount; i++)
                        LineStyles[i] = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyles[i];
                } else {
                    LineStyles2.clear();
                    LineStyles2.resize(CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount);
                    for (int i = 0; i < (int)CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyleCount; i++)
                        LineStyles2[i] = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.LineStyles.LineStyles2[i];
                }

                fills.assign(FillStyles.size(), PendingPath());
                strokes.assign(shapeVersion < 4 ? LineStyles.size() : LineStyles2.size(), PendingPath());

                fillStyle0.styleId = 0;
                fillStyle1.styleId = 0;
                lineStyle.styleId  = 0;
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
                if (lineStyle.styleId  > 0) lineStyle.addPoint(penX, penY, false);

            }

            if (!CurrentRecord.EdgeRecords.StraightFlag) {

                int cX = penX + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.ControlDeltaX;
                int cY = penY + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.ControlDeltaY;
                int eX = cX   + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaX;
                int eY = cY   + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaY;
                penX = eX;
                penY = eY;

                if (fillStyle1.styleId > 0) { fillStyle1.addPoint(cX, cY, true); fillStyle1.addPoint(eX, eY, false); }
                if (fillStyle0.styleId > 0) { fillStyle0.addPoint(cX, cY, true); fillStyle0.addPoint(eX, eY, false); }
                if (lineStyle.styleId  > 0) { lineStyle.addPoint(cX, cY, true);  lineStyle.addPoint(eX, eY, false);  }
            }
        }
    }

    // Final flush for the last layer
    flushLayer();

    binOut.picture = recorder.finishRecordingAsPicture();

    return binOut;
}

Shape getShape(RECT shapeBounds, const SHAPE& rawShape,
               int version,
               const std::vector<FILLSTYLE>& inFillStyles,
               const std::vector<LINESTYLE>& inLineStyles,
               const std::vector<LINESTYLE2>& inLineStyles2) {

    Shape binOut;

    binOut.Width  = (shapeBounds.xMax - shapeBounds.xMin) / 20;
    binOut.Height = (shapeBounds.yMax - shapeBounds.yMin) / 20;

    std::vector<FILLSTYLE>  FillStyles  = inFillStyles;
    std::vector<LINESTYLE>  LineStyles  = inLineStyles;
    std::vector<LINESTYLE2> LineStyles2 = inLineStyles2;

    std::vector<PendingPath> fills(FillStyles.size());
    std::vector<PendingPath> strokes(version < 2 ? LineStyles.size() : LineStyles2.size());

    ActivePath fillStyle0;
    ActivePath fillStyle1;
    ActivePath lineStyle;

    int penX = 0;
    int penY = 0;

    auto flushPaths = [&]() {
        fillStyle1.flushFill(penX, penY, fills, false, false);
        fillStyle0.flushFill(penX, penY, fills, true,  false);
        lineStyle.flushStroke(penX, penY, strokes);
    };

    auto flushLayer = [&]() {

        flushPaths();

        std::vector<FILLSTYLE> layerFillStyles;

        for (int i = 0; i < (int)fills.size(); i++) {
            if (fills[i].segments.empty()) continue;
            SkPathBuilder builder;
            builder.setFillType(SkPathFillType::kEvenOdd);
            fills[i].buildFillPath(builder);
            binOut.FillPaths.push_back(builder.detach());
            layerFillStyles.push_back(FillStyles[i]);
            fills[i].segments.clear();
        }

        auto layerFillPaints = getSkiaFills(layerFillStyles, 3);
        for (auto& p : layerFillPaints) binOut.Fills.push_back(p);

        std::vector<LINESTYLE>  layerLineStyles;
        std::vector<LINESTYLE2> layerLineStyles2;

        for (int i = 0; i < (int)strokes.size(); i++) {
            if (strokes[i].segments.empty()) continue;
            SkPathBuilder builder;
            bool noClose = (version >= 2 && i < (int)LineStyles2.size())
                              ? LineStyles2[i].NoClose : false;
            strokes[i].buildLinePath(builder, noClose);
            binOut.LinePaths.push_back(builder.detach());
            if (version < 2) layerLineStyles.push_back(LineStyles[i]);
            else             layerLineStyles2.push_back(LineStyles2[i]);
            strokes[i].segments.clear();
        }

        auto layerStrokePaints = getSkiaLines(layerLineStyles, layerLineStyles2, version < 2 ? 3 : 4);
        for (auto& p : layerStrokePaints) binOut.Lines.push_back(p);
    };

    for (int recordIndex = 0; recordIndex < (int)rawShape.ShapeRecords.size(); recordIndex++) {

        const SHAPERECORD& CurrentRecord = rawShape.ShapeRecords[recordIndex];

        if (CurrentRecord.NonEdgeRecords.ISENDRECORD) break;

        if (CurrentRecord.TypeFlag == 0) {

            if (CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.StateMoveTo) {
                flushPaths();
                penX = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.MoveDeltaX;
                penY = CurrentRecord.NonEdgeRecords.STYLECHANGERECORD.MoveDeltaY;
                fillStyle0.reset(penX, penY);
                fillStyle1.reset(penX, penY);
                lineStyle.reset(penX, penY);
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
                if (lineStyle.styleId  > 0) lineStyle.addPoint(penX, penY, false);
            }

            if (!CurrentRecord.EdgeRecords.StraightFlag) {

                int cX = penX + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.ControlDeltaX;
                int cY = penY + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.ControlDeltaY;
                int eX = cX   + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaX;
                int eY = cY   + CurrentRecord.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaY;
                penX = eX;
                penY = eY;

                if (fillStyle1.styleId > 0) { fillStyle1.addPoint(cX, cY, true); fillStyle1.addPoint(eX, eY, false); }
                if (fillStyle0.styleId > 0) { fillStyle0.addPoint(cX, cY, true); fillStyle0.addPoint(eX, eY, false); }
                if (lineStyle.styleId  > 0) { lineStyle.addPoint(cX, cY, true);  lineStyle.addPoint(eX, eY, false);  }
            }
        }
    }

    flushLayer();

    return binOut;
}

SkMatrix transformShape(Shape&, MATRIX& TransformMatrix) {
    SkMatrix binOut;
    binOut.setAll(
        TransformMatrix.ScaleX,       TransformMatrix.RotateSkew1,    TransformMatrix.TranslateX / 20.0f,
        TransformMatrix.RotateSkew0,  TransformMatrix.ScaleY,          TransformMatrix.TranslateY / 20.0f,
        0,                             0,                               1
    );
    return binOut;
}