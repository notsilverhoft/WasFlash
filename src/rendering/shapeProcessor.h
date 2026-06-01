#pragma once
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
#include "../../include/skia/include/core/SkPaint.h"
#include "../../include/skia/include/core/SkPathBuilder.h"
#include "../../include/skia/include/core/SkPicture.h"
#include "../base/rect.h"
#include "../base/shapeWithStyle.h"
#include "../base/shapeRecord.h"
#include "../base/colorTransformAlpha.h"


struct Shape {
    uint16_t shapeID;
    int Width;
    int Height;
    std::vector<SkPath> FillPaths;
    std::vector<SkPath> LinePaths;
    std::vector<SkPaint> Fills;
    std::vector<SkPaint> Lines;
    std::vector<FILLSTYLE> FillStylesForPaths;
    std::vector<LINESTYLE> LineStylesForPaths;
    std::vector<LINESTYLE2> LineStyles2ForPaths;
    sk_sp<SkPicture> picture;
};

struct Edge {
    int sX = 0;
    int sY = 0;
    int eX = 0;
    int eY = 0;
    int cX = 0;
    int cY = 0;
    bool useFs0 = false;
    int fs0 = 0;
    bool useFs1 = false;
    int fs1 = 0;
    bool useLs = false;
    int ls = 0;
    int type = 0;
};

Shape getShape(RECT shapeBounds, const SHAPEWITHSTYLE& rawShape, int shapeVersion, bool usesFillWindingRule = false);
SkMatrix transformShape(Shape& ShapeIn, MATRIX& TransformMatrix);
void applyColorTransform(Shape& shape, const CXFORMWITHALPHA& ct);