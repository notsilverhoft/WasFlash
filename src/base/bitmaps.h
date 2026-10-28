#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

struct PIX15 {
    uint8_t Pix15Reserved;
    uint8_t Pix15Red;
    uint8_t Pix15Green;
    uint8_t Pix15Blue;
};

struct PIX24 {
    uint8_t Pix24Reserved;
    uint8_t Pix24Red;
    uint8_t Pix24Green;
    uint8_t Pix24Blue;
};

struct COLORMAPDATA {
    std::vector<uint8_t> ColorTableRed;
    std::vector<uint8_t> ColorTableGreen;
    std::vector<uint8_t> ColorTableBlue;
    std::vector<uint8_t> ColormapPixelData;
};

struct BITMAPDATA {
    std::vector<PIX15> BitmapPixelData15;
    std::vector<PIX24> BitmapPixelData24;
};

struct ALPHACOLORMAPDATA {
    std::vector<uint8_t> ColorTableRed;
    std::vector<uint8_t> ColorTableGreen;
    std::vector<uint8_t> ColorTableBlue;
    std::vector<uint8_t> ColorTableAlpha;
    std::vector<uint8_t> ColormapPixelData;
}; 

struct ALPHABITMAPDATA {
    std::vector<uint8_t> BitmapPixelDataAlpha;
    std::vector<uint8_t> BitmapPixelDataRed;
    std::vector<uint8_t> BitmapPixelDataGreen;
    std::vector<uint8_t> BitmapPixelDataBlue;
};

COLORMAPDATA getColormapData(std::vector<uint8_t>& data, uint8_t BitmapColorTableSize, uint16_t Width, uint16_t Height);
ALPHACOLORMAPDATA getAlphaColormapData(std::vector<uint8_t>& data, uint8_t BitmapColorTableSize, uint16_t Width, uint16_t Height);
BITMAPDATA getBitmapData(std::vector<uint8_t>& data, uint16_t Width, uint16_t Height, int PixelFormat);
ALPHABITMAPDATA getAlphaBitmapData(std::vector<uint8_t>& data, uint16_t Width, uint16_t Height, int PixelFormat);
std::vector<uint8_t> get8BitColorMappedImage(COLORMAPDATA rawData, uint16_t Width, uint16_t Height);
std::vector<uint8_t> get8BitAlphaColorMappedImage(ALPHACOLORMAPDATA rawData, uint16_t Width, uint16_t Height);
std::vector<uint8_t> get15BitImage(BITMAPDATA rawData, uint16_t Width, uint16_t Height);
std::vector<uint8_t> get24BitImage(BITMAPDATA rawData, uint16_t Width, uint16_t Height);
std::vector<uint8_t> get32BitAlphaImage(ALPHABITMAPDATA rawData, uint16_t Width, uint16_t Height);