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

    if ( shapeVersion == 4 ) {

        binOut.DefineShape.DefineShape4.EdgeBounds = getRect(rawTag.tagData);
        std::cout << "DefineShape: Version: " << shapeVersion << ": EdgeBounds: Width: " << (int)((binOut.DefineShape.DefineShape4.EdgeBounds.xMax / 20) - (binOut.DefineShape.DefineShape4.EdgeBounds.xMin / 20)) << "\n";
        std::cout << "DefineShape: Version: " << shapeVersion << ": EdgeBounds: Height: " << (int)((binOut.DefineShape.DefineShape4.EdgeBounds.yMax / 20) - (binOut.DefineShape.DefineShape4.EdgeBounds.yMin / 20)) << "\n";
        binOut.DefineShape.DefineShape4.Reserved = (rawTag.tagData[0] >> 3);
        std::cout << "DefineShape: Version: " << shapeVersion << ": Reserved(Should Be 0):  " << (int)(binOut.DefineShape.DefineShape4.Reserved) << "\n";
        binOut.DefineShape.DefineShape4.UsesFillWindingRule = ((rawTag.tagData[0] >> 2) & 0x01);
        std::cout << "DefineShape: Version: " << shapeVersion << ": UsesFillWindingRule: " << (int)(binOut.DefineShape.DefineShape4.UsesFillWindingRule) << "\n";
        binOut.DefineShape.DefineShape4.UsesNonScalingStrokes = ((rawTag.tagData[0] >> 1) & 0x01);
        std::cout << "DefineShape: Version: " << shapeVersion << ": UsesNonScalingStrokes: " << (int)(binOut.DefineShape.DefineShape4.UsesNonScalingStrokes) << "\n";
        binOut.DefineShape.DefineShape4.UsesScalingStrokes = (rawTag.tagData[0] & 0x01);
        std::cout << "DefineShape: Version: " << shapeVersion << ": UsesScalingStrokes: " << (int)(binOut.DefineShape.DefineShape4.UsesScalingStrokes) << "\n";

        SWFShift(rawTag.tagData, 1);

    }

    binOut.DefineShape.Shapes = getShapeWithStyle(rawTag.tagData, shapeVersion);

    return binOut;
    
}