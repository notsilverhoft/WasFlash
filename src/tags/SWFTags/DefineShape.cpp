#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../base/rect.h"
#include "../../base/shapeWithStyle.h"
#include "../../utils/trackSWF.h"

SWFTag getDefineShapeTag(rawSWFTag rawTag, int shapeVersion) {

    SWFTag binOut;

    binOut.DefineShape.ShapeID = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    std::cout << "DefineShape: Version: " << shapeVersion << ": ShapeID: " << (int)binOut.DefineShape.ShapeID << "\n";
    SWFShift(rawTag.tagData, 2);
    

    binOut.DefineShape.ShapeBounds = getRect(rawTag.tagData);
    std::cout << "DefineShape: Version: " << shapeVersion << ": ShapeBounds: Width: " << (int)((binOut.DefineShape.ShapeBounds.xMax / 20) - (binOut.DefineShape.ShapeBounds.xMin / 20)) << "\n";
    std::cout << "DefineShape: Version: " << shapeVersion << ": ShapeBounds: Height: " << (int)((binOut.DefineShape.ShapeBounds.yMax / 20) - (binOut.DefineShape.ShapeBounds.yMin / 20)) << "\n";

    binOut.DefineShape.Shapes = getShapeWithStyle(rawTag.tagData, shapeVersion);

    return binOut;
    
}