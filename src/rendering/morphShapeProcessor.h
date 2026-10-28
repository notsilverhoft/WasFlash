#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <utility>
#include "../base/shape.h"
#include "../base/matrix.h"
#include "../tags/tags.h"
#include "../../include/skia/include/core/SkMatrix.h"

std::pair<Shape, Shape> MorphShapeToShape(SWFTag input, int morphShapeVersion, const std::unordered_map<uint16_t, BitmapStream>& bitmaps = {});
Shape getMorphFrame(std::pair<Shape, Shape> Shapes, int ratio);
SkMatrix transformMorphShape(std::pair<Shape, Shape>&, MATRIX& TransformMatrix);