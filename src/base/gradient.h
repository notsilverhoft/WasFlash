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

struct MORPHGRADRECORD {
    uint8_t StartRatio;
    uint8_t StartColorRed, StartColorGreen, StartColorBlue, StartColorAlpha;
    uint8_t EndRatio;
    uint8_t EndColorRed, EndColorGreen, EndColorBlue, EndColorAlpha;
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
    std::vector<GRADRECORD> GradientRecords = {};
    float FocalPoint = 0;
};

struct MORPHGRADIENT {
    uint8_t NumGradients = 0;    
    std::vector<MORPHGRADRECORD> GradientRecords = {};
};


GRADRECORD getGradientRecord(std::vector<uint8_t>& data, int shapeVersion);

GRADIENT getGradient(std::vector<uint8_t>& data, int shapeVersion);

FOCALGRADIENT getFocalGradient(std::vector<uint8_t>& data, int shapeVersion);

MORPHGRADRECORD getMorphGradientRecord(std::vector<uint8_t>& data);

MORPHGRADIENT getMorphGradient(std::vector<uint8_t>& data);