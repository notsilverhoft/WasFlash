#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

struct GRADRECORD {
    uint8_t Ratio;
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Alpha = 0;
};

struct GRADIENT {
    uint8_t SpreadMode;
    uint8_t InterpolationMode;
    uint8_t NumGradients;
    std::vector<GRADRECORD> GradientRecords;
};

struct FOCALGRADIENT {
    uint8_t SpreadMode;
    uint8_t InterpolationMode;
    uint8_t NumGradients;
    std::vector<GRADRECORD> GradientRecords;
    float FocalPoint;
};


GRADRECORD getGradientRecord(std::vector<uint8_t>& data, int shapeVersion);

GRADIENT getGradient(std::vector<uint8_t>& data, int shapeVersion);

FOCALGRADIENT getFocalGradient(std::vector<uint8_t>& data, int shapeVersion);