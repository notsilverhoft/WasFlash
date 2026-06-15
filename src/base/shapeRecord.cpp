#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "shapeRecord.h"
#include "gradient.h"
#include "../utils/trackSWF.h"
#include "../utils/bitStream.hpp"

FILLSTYLE getFillStyle(std::vector<uint8_t>& data, int shapeVersion) {

    FILLSTYLE binOut;

    binOut.FillStyleType = data[0];
    SWFShift(data, 1);

    if ( binOut.FillStyleType == 0x00 ) {

        if ( shapeVersion == 3 || shapeVersion == 4 ) {

            binOut.Red = data[0];
            binOut.Green = data[1];
            binOut.Blue = data[2];
            binOut.Alpha = data[3];
            SWFShift(data, 4);

        }
        if ( shapeVersion == 1 || shapeVersion == 2 ) {

            binOut.Red = data[0];
            binOut.Green = data[1];
            binOut.Blue = data[2];
            SWFShift(data, 3);

        }

    }

    if ( binOut.FillStyleType == 0x10 || binOut.FillStyleType == 0x12 || binOut.FillStyleType == 0x13 ) {

        binOut.GradientMatrix = getMatrix(data);

        if ( binOut.FillStyleType == 0x10 || binOut.FillStyleType == 0x12 ) {
            
            binOut.Gradient = getGradient(data, shapeVersion);
            
        }

        if ( binOut.FillStyleType == 0x13 ) {

            binOut.FocalGradient = getFocalGradient(data, shapeVersion);

        }

    }

    if ( binOut.FillStyleType == 0x40 || binOut.FillStyleType == 0x41 || binOut.FillStyleType == 0x42 || binOut.FillStyleType == 0x43 ) {

        binOut.BitmapID = static_cast<uint16_t>((data[1] << 8) | data[0]);
        SWFShift(data, 2);

        binOut.BitmapMatrix = getMatrix(data);

    }

    return binOut;

}

FILLSTYLEARRAY getFillStyleArray(std::vector<uint8_t>& data, int shapeVersion) {

    FILLSTYLEARRAY binOut;
    
    binOut.FillStyleCount = data[0];
    SWFShift(data, 1);

    if ( ( shapeVersion == 2 || shapeVersion == 3 || shapeVersion == 4 ) && binOut.FillStyleCount == 0xFF ) {

        binOut.FillStyleCountExtended = static_cast<uint16_t>((data[1] << 8) | data[0]);
        binOut.FillStyles.resize(binOut.FillStyleCountExtended);
        SWFShift(data, 2);

        for ( int i = 0; i < binOut.FillStyleCountExtended; i++ ) {

            binOut.FillStyles[i] = getFillStyle(data, shapeVersion);

        }

        binOut.FillStyleCount = binOut.FillStyleCountExtended;

    }

    else {

        binOut.FillStyles.resize(binOut.FillStyleCount);

        for ( int i = 0; i < binOut.FillStyleCount; i++ ) {

            binOut.FillStyles[i] = getFillStyle(data, shapeVersion);

        }

    }

    return binOut;
    
}

LINESTYLE getLineStyle(std::vector<uint8_t>& data, int shapeVersion) {
    
    LINESTYLE binOut;

    binOut.Width = static_cast<uint16_t>((data[1] << 8) | data[0]);
    SWFShift(data, 2);

    binOut.Red = data[0];
    binOut.Green = data[1];
    binOut.Blue = data[2];

    if ( shapeVersion == 3 ) {

        binOut.Alpha = data[3];
        SWFShift(data, 4);

    }
    
    else {

        SWFShift(data, 3);

    }

    return binOut;

}

LINESTYLE2 getLineStyle2(std::vector<uint8_t>& data, int shapeVersion) {

    LINESTYLE2 binOut;

    binOut.Width = ((data[1] << 8) | data[0]);
    SWFShift(data, 2);

    binOut.StartCapStyle = (data[0] >> 6);
    binOut.JoinStyle = ((data[0] >> 4) & 0x03);
    binOut.HasFillFlag = ((data[0] >> 3) & 0x01);
    binOut.NoHScaleFlag = ((data[0] >> 2) & 0x01);
    binOut.NoVScaleFlag = ((data[0] >> 1) & 0x01);
    binOut.PixelHintingFlag = (data[0] & 0x01);
    SWFShift(data, 1);

    binOut.Reserved = ((data[0] >> 3) & 0x1F);
    binOut.NoClose = ((data[0] >> 2) & 0x01);
    binOut.EndCapStyle = (data[0] & 0x03);
    SWFShift(data, 1);

    if ( binOut.JoinStyle == 2 ) {

        binOut.MiterLimitFactor = static_cast<float>((static_cast<int16_t>((data[1] << 8) | data[0])) / 256.0f);
        SWFShift(data, 2);

    }

    if ( !binOut.HasFillFlag ) {

        binOut.Red = data[0];
        binOut.Green = data[1];
        binOut.Blue = data[2];
        binOut.Alpha = data[3];
        SWFShift(data, 4);

    }

    else {

        binOut.FillType = getFillStyle(data, shapeVersion);

    }

    return binOut;

}

LINESTYLEARRAY getLineStyleArray(std::vector<uint8_t>& data, int shapeVersion) {

    LINESTYLEARRAY binOut;

    binOut.LineStyleCount = data[0];
    SWFShift(data, 1);

    if ( binOut.LineStyleCount == 0 ) {

        return {};

    }

    if ( shapeVersion < 4 ) {

        if ( binOut.LineStyleCount == 0xFF ) {

            binOut.LineStyleCountExtended = static_cast<uint16_t>((data[1] << 8) | data[0]);
            binOut.LineStyles.resize(binOut.LineStyleCountExtended);
            SWFShift(data, 2);

            for ( int i = 0; i < binOut.LineStyleCountExtended; i++ ) {

                binOut.LineStyles[i] = getLineStyle(data, shapeVersion);

            }

            binOut.LineStyleCount = binOut.LineStyleCountExtended;

        }

        else {

            binOut.LineStyles.resize(binOut.LineStyleCount);

            for ( int i = 0; i < binOut.LineStyleCount; i++ ) {
                
                binOut.LineStyles[i] = getLineStyle(data, shapeVersion);

            }

        }

    }
     
    if ( shapeVersion == 4 ) {

        if ( binOut.LineStyleCount == 0xFF ) {

            binOut.LineStyleCountExtended = static_cast<uint16_t>((data[1] << 8) | data[0]);
            binOut.LineStyles2.resize(binOut.LineStyleCountExtended);
            SWFShift(data, 2);
            
            for ( int i = 0; i < binOut.LineStyleCountExtended; i++ ) {

                binOut.LineStyles2[i] = getLineStyle2(data, shapeVersion);

            }

        }

        else {

            binOut.LineStyles2.resize(binOut.LineStyleCount);

            for ( int i = 0; i < binOut.LineStyleCount; i++ ) {

                binOut.LineStyles2[i] = getLineStyle2(data, shapeVersion);

            }

        }

    }

    return binOut;

}


std::vector<SHAPERECORD> getShapeRecords(std::vector<uint8_t>& data, int shapeVersion, uint8_t& NumFillBits, uint8_t& NumLineBits) {

    std::vector<SHAPERECORD> binOut;

    BitStream bs(data);

    while(true) {
        
        SHAPERECORD out;

        out.TypeFlag = bs.readUnsigned(1);

        if ( out.TypeFlag == 0 ) { // Non-Edge
                
            out.NonEdgeRecords.STYLECHANGERECORD.StateNewStyles  = bs.readUnsigned(1);
            out.NonEdgeRecords.STYLECHANGERECORD.StateLineStyle  = bs.readUnsigned(1);
            out.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle1 = bs.readUnsigned(1);
            out.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle0 = bs.readUnsigned(1);
            out.NonEdgeRecords.STYLECHANGERECORD.StateMoveTo     = bs.readUnsigned(1);

            if ( !out.NonEdgeRecords.STYLECHANGERECORD.StateNewStyles &&
                 !out.NonEdgeRecords.STYLECHANGERECORD.StateLineStyle &&
                 !out.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle1 &&
                 !out.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle0 &&
                 !out.NonEdgeRecords.STYLECHANGERECORD.StateMoveTo ) {

                out.NonEdgeRecords.ISENDRECORD = true;
                binOut.push_back(out);
                break;

            }

            else {

                if ( out.NonEdgeRecords.STYLECHANGERECORD.StateMoveTo ) {

                    out.NonEdgeRecords.STYLECHANGERECORD.MoveBits   = bs.readUnsigned(5);
                    out.NonEdgeRecords.STYLECHANGERECORD.MoveDeltaX = bs.readSigned(out.NonEdgeRecords.STYLECHANGERECORD.MoveBits);
                    out.NonEdgeRecords.STYLECHANGERECORD.MoveDeltaY = bs.readSigned(out.NonEdgeRecords.STYLECHANGERECORD.MoveBits);

                }

                if ( out.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle0 ) {

                    out.NonEdgeRecords.STYLECHANGERECORD.FillStyle0 = bs.readUnsigned(NumFillBits);

                }

                if ( out.NonEdgeRecords.STYLECHANGERECORD.StateFillStyle1 ) {

                    out.NonEdgeRecords.STYLECHANGERECORD.FillStyle1 = bs.readUnsigned(NumFillBits);

                }

                if ( out.NonEdgeRecords.STYLECHANGERECORD.StateLineStyle ) {

                    out.NonEdgeRecords.STYLECHANGERECORD.LineStyle = bs.readUnsigned(NumLineBits);

                }

                if ( out.NonEdgeRecords.STYLECHANGERECORD.StateNewStyles ) {

                    bs.shiftArrayToAlignment(data);

                    out.NonEdgeRecords.STYLECHANGERECORD.FillStyles = getFillStyleArray(data, shapeVersion);
                    out.NonEdgeRecords.STYLECHANGERECORD.LineStyles = getLineStyleArray(data, shapeVersion);

                    NumFillBits = ((data[0] >> 4) & 0x0F);
                    NumLineBits = (data[0] & 0x0F);
                    out.NonEdgeRecords.STYLECHANGERECORD.NumFillBits = NumFillBits;
                    out.NonEdgeRecords.STYLECHANGERECORD.NumLineBits = NumLineBits;
                    SWFShift(data, 1);
                    bs.reset(data);

                }

                binOut.push_back(out);

            }
            
        }

        if ( out.TypeFlag == 1 ) { // Edge

            out.EdgeRecords.StraightFlag = bs.readUnsigned(1);

            if ( out.EdgeRecords.StraightFlag ) { // Straight
                
                out.EdgeRecords.STRAIGHTEDGERECORD.NumBits = (bs.readUnsigned(4) + 2);
                out.EdgeRecords.STRAIGHTEDGERECORD.GeneralLineFlag = bs.readUnsigned(1);
                
                if ( !out.EdgeRecords.STRAIGHTEDGERECORD.GeneralLineFlag ) {

                    out.EdgeRecords.STRAIGHTEDGERECORD.VertLineFlag = bs.readUnsigned(1);

                    if ( !out.EdgeRecords.STRAIGHTEDGERECORD.VertLineFlag ) {

                        out.EdgeRecords.STRAIGHTEDGERECORD.LineDeltaX = bs.readSigned(out.EdgeRecords.STRAIGHTEDGERECORD.NumBits);

                    }
                    else {

                        out.EdgeRecords.STRAIGHTEDGERECORD.LineDeltaY = bs.readSigned(out.EdgeRecords.STRAIGHTEDGERECORD.NumBits);

                    }

                }

                else {

                    out.EdgeRecords.STRAIGHTEDGERECORD.GeneralLineDeltaX = bs.readSigned(out.EdgeRecords.STRAIGHTEDGERECORD.NumBits);
                    out.EdgeRecords.STRAIGHTEDGERECORD.GeneralLineDeltaY = bs.readSigned(out.EdgeRecords.STRAIGHTEDGERECORD.NumBits);

                } 
                binOut.push_back(out);

            }

            else { // Curved

                out.EdgeRecords.CURVEDEDGERECORD.NumBits = (bs.readUnsigned(4) + 2);
                out.EdgeRecords.CURVEDEDGERECORD.ControlDeltaX = bs.readSigned(out.EdgeRecords.CURVEDEDGERECORD.NumBits);
                out.EdgeRecords.CURVEDEDGERECORD.ControlDeltaY = bs.readSigned(out.EdgeRecords.CURVEDEDGERECORD.NumBits);
                out.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaX = bs.readSigned(out.EdgeRecords.CURVEDEDGERECORD.NumBits);
                out.EdgeRecords.CURVEDEDGERECORD.AnchorDeltaY = bs.readSigned(out.EdgeRecords.CURVEDEDGERECORD.NumBits);
                binOut.push_back(out);

            }

        }
    
    }

    bs.shiftArrayToAlignment(data);

    return binOut;

}