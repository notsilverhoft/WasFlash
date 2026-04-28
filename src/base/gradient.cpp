#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "gradient.h"
#include "../utils/trackSWF.h"
 
GRADRECORD getGradientRecord(std::vector<uint8_t>& data, int shapeVersion) {

    GRADRECORD binOut;

    binOut.Ratio = data[0];
    std::cout << "GradientRecord: Ratio:" << (int)binOut.Ratio << "\n";
    binOut.Red = data[1];
    std::cout << "GradientRecord: Red:" << (int)binOut.Red << "\n";
    binOut.Green = data[2];
    std::cout << "GradientRecord: Green:" << (int)binOut.Green << "\n";
    binOut.Blue = data[3];
    std::cout << "GradientRecord: Blue:" << (int)binOut.Blue << "\n";
    SWFShift(data, 4);

    if ((shapeVersion == 3) || (shapeVersion == 4)) {

        binOut.Alpha = data[0];
        std::cout << "GradientRecord: Alpha:" << (int)binOut.Alpha << "\n";
        SWFShift(data, 1);

    }


    return binOut;

}

GRADIENT getGradient(std::vector<uint8_t>& data, int shapeVersion) {

    GRADIENT binOut;

    binOut.SpreadMode = (data[0] >> 6);
    std::cout << "Gradient: SpreadMode: " << (int)binOut.SpreadMode << "\n";
    binOut.InterpolationMode = ((data[0] >> 4) & 0x03);
    std::cout << "Gradient: InterpolationMode: " << (int)binOut.InterpolationMode << "\n";
    binOut.NumGradients = (data[0] & 0x0F);
    std::cout << "Gradient: NumGradients: " << (int)binOut.NumGradients << "\n";
    binOut.GradientRecords.resize(binOut.NumGradients);
    SWFShift(data, 1);
    
    for (int i = 0; i < (int)binOut.NumGradients; i++) {
        
        binOut.GradientRecords[i] = getGradientRecord(data, shapeVersion);

    }
    
    return binOut;

}

FOCALGRADIENT getFocalGradient(std::vector<uint8_t>& data, int shapeVersion) {

    FOCALGRADIENT binOut;

    binOut.SpreadMode = (data[0] >> 6 );
    std::cout << "FocalGradient: SpreadMode: " << (int)binOut.SpreadMode << "\n";
    binOut.InterpolationMode = ((data[0] >> 4) & 0x03);
    std::cout << "FocalGradient: InterpolationMode: " << (int)binOut.InterpolationMode << "\n";
    binOut.NumGradients = (data[0] & 0x0F);
    std::cout << "FocalGradient: NumGradients: " << (int)binOut.NumGradients << "\n";
    binOut.GradientRecords.resize(binOut.NumGradients);
    SWFShift(data, 1);

    
    for (int i = 0; i < (int)binOut.NumGradients; i++) {

        binOut.GradientRecords[i] = getGradientRecord(data, shapeVersion);

    }

    binOut.FocalPoint = static_cast<float>((static_cast<int16_t>((data[1] << 8) | data[0])) / 256.0f);
    std::cout << "FocalGradient: FocalPoint: " << (int)binOut.InterpolationMode << "\n";
    SWFShift(data, 2);

    return binOut;

}