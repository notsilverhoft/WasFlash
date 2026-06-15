#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"
#include "../../base/rect.h"
#include "../../base/morphShapeRecord.h"
#include "../../base/shape.h"
#include "../../utils/trackSWF.h"


SWFTag getDefineMorphShapeTag(rawSWFTag& rawTag, int morphShapeVersion) {

    SWFTag binOut;

    binOut.DefineMorphShape.CharacterId = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": CharacterID: " << (int)(binOut.DefineMorphShape.CharacterId) << "\n";
    SWFShift(rawTag.tagData, 2);

    binOut.DefineMorphShape.StartBounds = getRect(rawTag.tagData);
    binOut.DefineMorphShape.EndBounds = getRect(rawTag.tagData);
    // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": StartBounds: Width: " << (int)((binOut.DefineMorphShape.StartBounds.xMax) - (binOut.DefineMorphShape.StartBounds.xMin) / 20) << "\n";
    // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": StartBounds: Height: " << (int)((binOut.DefineMorphShape.StartBounds.yMax) - (binOut.DefineMorphShape.StartBounds.yMin) / 20) << "\n";
    // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": EndBounds: Width: " << (int)((binOut.DefineMorphShape.EndBounds.xMax) - (binOut.DefineMorphShape.EndBounds.xMin) / 20) << "\n";
    // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": EndBounds: Height: " << (int)((binOut.DefineMorphShape.EndBounds.yMax) - (binOut.DefineMorphShape.EndBounds.yMin) / 20) << "\n";

    if ( morphShapeVersion == 2 ) {

        binOut.DefineMorphShape.DefineMorphShape2.StartEdgeBounds = getRect(rawTag.tagData);
        binOut.DefineMorphShape.DefineMorphShape2.EndEdgeBounds = getRect(rawTag.tagData);
        // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": StartEdgeBounds: Width: " << (int)((binOut.DefineMorphShape.DefineMorphShape2.StartEdgeBounds.xMax) - (binOut.DefineMorphShape.DefineMorphShape2.StartEdgeBounds.xMin) / 20) << "\n";
        // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": StartEdgeBounds: Height: " << (int)((binOut.DefineMorphShape.DefineMorphShape2.StartEdgeBounds.yMax) - (binOut.DefineMorphShape.DefineMorphShape2.StartEdgeBounds.yMin) / 20) << "\n";
        // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": EndEdgeBounds: Width: " << (int)((binOut.DefineMorphShape.DefineMorphShape2.EndEdgeBounds.xMax) - (binOut.DefineMorphShape.DefineMorphShape2.EndEdgeBounds.xMin) / 20) << "\n";
        // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": EndEdgeBounds: Height: " << (int)((binOut.DefineMorphShape.DefineMorphShape2.EndEdgeBounds.yMax) - (binOut.DefineMorphShape.DefineMorphShape2.EndEdgeBounds.yMin) / 20) << "\n";

        binOut.DefineMorphShape.DefineMorphShape2.Reserved = (rawTag.tagData[0] >> 2);
        // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": Reserved(Should Be 0): " << (int)(binOut.DefineMorphShape.DefineMorphShape2.Reserved) << "\n";
        binOut.DefineMorphShape.DefineMorphShape2.UsesNonScalingStrokes = ((rawTag.tagData[0] >> 1) & 0x01);
        // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": UsesNonScalingStrokes: " << (int)(binOut.DefineMorphShape.DefineMorphShape2.UsesNonScalingStrokes) << "\n";
        binOut.DefineMorphShape.DefineMorphShape2.UsesScalingStrokes = (rawTag.tagData[0] & 0x01);
        // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": UsesScalingStrokes: " << (int)(binOut.DefineMorphShape.DefineMorphShape2.UsesScalingStrokes) << "\n";
        SWFShift(rawTag.tagData, 1);

    }

    binOut.DefineMorphShape.Offset = ((rawTag.tagData[3] << 24) | (rawTag.tagData[2] << 16) | (rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    // std::cout << "DefineMorphShape: Version: " << morphShapeVersion << ": Offset: " << (int)(binOut.DefineMorphShape.Offset) << "\n";
    SWFShift(rawTag.tagData, 4);

    // Record size here — Offset is measured from this point
    size_t sizeAfterOffset = rawTag.tagData.size();

    binOut.DefineMorphShape.MorphFillStyles = getMorphFillStyleArray(rawTag.tagData);

    binOut.DefineMorphShape.MorphLineStyles = getMorphLineStyleArray(rawTag.tagData, morphShapeVersion);

    binOut.DefineMorphShape.StartEdges = getSHAPE(rawTag.tagData, morphShapeVersion == 2);

    // Seek to EndEdges using Offset, bypassing any BitStream alignment issues
    size_t consumed = sizeAfterOffset - rawTag.tagData.size();
    if (consumed < binOut.DefineMorphShape.Offset) {
        SWFShift(rawTag.tagData, binOut.DefineMorphShape.Offset - consumed);
    }

    // Read EndEdges NumFillBits/NumLineBits byte manually.
    // Some encoders write 0 here even though the spec says they should match
    // StartEdges — fall back to StartEdges' values in that case.
    uint8_t endFillBits = (rawTag.tagData[0] >> 4) & 0x0F;
    uint8_t endLineBits = rawTag.tagData[0] & 0x0F;
    SWFShift(rawTag.tagData, 1);

    if (endFillBits == 0) endFillBits = binOut.DefineMorphShape.StartEdges.NumFillBits;
    if (endLineBits == 0) endLineBits = binOut.DefineMorphShape.StartEdges.NumLineBits;

    binOut.DefineMorphShape.EndEdges.NumFillBits = endFillBits;
    binOut.DefineMorphShape.EndEdges.NumLineBits = endLineBits;
    binOut.DefineMorphShape.EndEdges.ShapeRecords = getShapeRecords(rawTag.tagData, morphShapeVersion == 2 ? 4 : 3, endFillBits, endLineBits);

    return binOut;

}