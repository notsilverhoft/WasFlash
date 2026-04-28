#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "shapeWithStyle.h"
#include "shapeRecord.h"
#include "../utils/trackSWF.h"

SHAPEWITHSTYLE getShapeWithStyle(std::vector<uint8_t>& data, int shapeVersion) {

    SHAPEWITHSTYLE binOut;

    binOut.FillStyles = getFillStyleArray(data, shapeVersion);
    binOut.LineStyles = getLineStyleArray(data, shapeVersion);

    binOut.NumFillBits = ((data[0] >> 4) & 0x0F);
    std::cout << "DefineShape: Version: " << shapeVersion << ": Shapes: NumFillBits: " << (int)binOut.NumFillBits<<"\n";
    binOut.NumLineBits = (data[0] & 0x0F);
    std::cout << "DefineShape: Version: " << shapeVersion << ": Shapes: NumLineBits: " << (int)binOut.NumLineBits<<"\n";
    SWFShift(data, 1);

    binOut.ShapeRecords = getShapeRecords(data, shapeVersion, binOut.NumFillBits, binOut.NumLineBits);

    return binOut;

}