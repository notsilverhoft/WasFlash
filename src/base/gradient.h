#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

struct GRADRECORD {
    uint8_t Ratio = 0;
    uint8_t Red = 0;
    uint8_t Green = 0;
    uint8_t Blue = 0;
    uint8_t Alpha = 0;
};

struct GRADIENT {
    uint8_t SpreadMode = 0;
    uint8_t InterpolationMode = 0;
    uint8_t NumGradients = 0;
    std::vector<GRADRECORD> GradientRecords = {};
};

struct FOCALGRADIENT {
    uint8_t SpreadMode = 0;
    uint8_t InterpolationMode = 0;
    uint8_t NumGradients = 0;
    std::vector<GRADRECORD> GradientRecords {};
    float FocalPoint = 0;
};


GRADRECORD getGradientRecord(std::vector<uint8_t>& data, int shapeVersion);

GRADIENT getGradient(std::vector<uint8_t>& data, int shapeVersion);

FOCALGRADIENT getFocalGradient(std::vector<uint8_t>& data, int shapeVersion);