#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "bitmaps.h"
#include "../utils/trackSWF.h"

COLORMAPDATA getColormapData(std::vector<uint8_t>& data, uint8_t BitmapColorTableSize, uint16_t Width, uint16_t Height) {

    COLORMAPDATA binOut;

    binOut.ColorTableRed.resize(BitmapColorTableSize + 1);
    binOut.ColorTableGreen.resize(BitmapColorTableSize + 1);
    binOut.ColorTableBlue.resize(BitmapColorTableSize + 1);

    for (int i = 0; i < BitmapColorTableSize + 1; i++) {

        binOut.ColorTableRed[i] = data[i * 3];
        binOut.ColorTableGreen[i] = data[i * 3 + 1];
        binOut.ColorTableBlue[i] = data[i * 3 + 2];

    }

    SWFShift(data, ((BitmapColorTableSize + 1) * 3));

    binOut.ColormapPixelData.resize(data.size());
    binOut.ColormapPixelData = data;

    return binOut;

}

ALPHACOLORMAPDATA getAlphaColormapData(std::vector<uint8_t>& data, uint8_t BitmapColorTableSize, uint16_t Width, uint16_t Height) {

    std::cout << "ENTERED getAlphaColormapData, data.size()=" << data.size() << "\n";

    ALPHACOLORMAPDATA binOut;

    binOut.ColorTableRed.resize(BitmapColorTableSize + 1);
    binOut.ColorTableGreen.resize(BitmapColorTableSize + 1);
    binOut.ColorTableBlue.resize(BitmapColorTableSize + 1);
    binOut.ColorTableAlpha.resize(BitmapColorTableSize + 1);

    for (int i = 0; i < BitmapColorTableSize + 1; i++) {

        binOut.ColorTableRed[i] = data[i * 4];
        binOut.ColorTableGreen[i] = data[i * 4 + 1];
        binOut.ColorTableBlue[i] = data[i * 4 + 2];
        binOut.ColorTableAlpha[i] = data[i * 4 + 2];

    }

    SWFShift(data, ((BitmapColorTableSize + 1) * 4));

    binOut.ColormapPixelData.resize(data.size());
    binOut.ColormapPixelData = data;

    return binOut;

}

PIX15 getPix15(uint16_t data) {

PIX15 binOut;

binOut.Pix15Reserved = (data >> 15);
binOut.Pix15Red = ((data >> 10) & 0x1F);
binOut.Pix15Green = ((data >> 5) & 0x1F);
binOut.Pix15Blue = (data & 0x1F);

return binOut;

}

PIX24 getPix24(std::vector<uint8_t>& data) {

PIX24 binOut;

binOut.Pix24Reserved = data[0];
binOut.Pix24Red = data[1];
binOut.Pix24Green = data[2];
binOut.Pix24Blue = data[3];
SWFShift(data, 4);

return binOut;

}

BITMAPDATA getBitmapData(std::vector<uint8_t>& data, uint16_t Width, uint16_t Height, int PixelFormat) {

    BITMAPDATA binOut;

    if ( PixelFormat == 4 ) {

        const int RowPaddingBytes = (((Width * 2) + 3) & ~3);
        const int PixelsPerRow = RowPaddingBytes / 2;
        const int TotalPixels = PixelsPerRow * Height;

        binOut.BitmapPixelData15.resize(TotalPixels);

        for (int i = 0; i < TotalPixels; i++) {

            uint16_t dataBuffer = ((data[i * 2] << 8) | data[i * 2 + 1]); // PIX15 is Big Endian

            binOut.BitmapPixelData15[i] = getPix15(dataBuffer);

        }

        return binOut;

    }

    if ( PixelFormat == 5 ) {

        const int TotalPixels = Width * Height;

        binOut.BitmapPixelData24.resize(TotalPixels);

        for (int i = 0; i < TotalPixels; i++) {

            binOut.BitmapPixelData24[i].Pix24Reserved = data[i * 4];
            binOut.BitmapPixelData24[i].Pix24Red      = data[i * 4 + 1];
            binOut.BitmapPixelData24[i].Pix24Green    = data[i * 4 + 2];
            binOut.BitmapPixelData24[i].Pix24Blue     = data[i * 4 + 3];

        }

        return binOut;

    }

    return binOut;

}

ALPHABITMAPDATA getAlphaBitmapData(std::vector<uint8_t>& data, uint16_t Width, uint16_t Height, int PixelFormat) {

    ALPHABITMAPDATA binOut;

    binOut.BitmapPixelDataAlpha.resize(Width * Height);
    binOut.BitmapPixelDataRed.resize(Width * Height);
    binOut.BitmapPixelDataGreen.resize(Width * Height);
    binOut.BitmapPixelDataBlue.resize(Width * Height);

    for (int i = 0; i < Width * Height; i++) {

        binOut.BitmapPixelDataAlpha[i] = data[i * 4];
        binOut.BitmapPixelDataRed[i] = data[i * 4 + 1];
        binOut.BitmapPixelDataGreen[i] = data[i * 4 + 2];
        binOut.BitmapPixelDataBlue[i] = data[i * 4 + 3];

    }

    return binOut;

}

std::vector<uint8_t> get8BitColorMappedImage(COLORMAPDATA rawData, uint16_t Width, uint16_t Height) {

    std::vector<uint8_t> binOut;
    binOut.reserve((size_t)Width * Height * 4);

    const int RowPaddingWidth = ((Width + 3) & ~3);

    for (int i = Height; i > 0; i--) {

        for (int x = 0; x < Width; x++) {

            int colorIndex = rawData.ColormapPixelData[x];

            binOut.push_back(rawData.ColorTableRed[colorIndex]);
            binOut.push_back(rawData.ColorTableGreen[colorIndex]);
            binOut.push_back(rawData.ColorTableBlue[colorIndex]);
            binOut.push_back(225); // Alpha

        }

        SWFShift(rawData.ColormapPixelData, RowPaddingWidth);

    }

    return binOut;

}

std::vector<uint8_t> get8BitAlphaColorMappedImage(ALPHACOLORMAPDATA rawData, uint16_t Width, uint16_t Height) {

    std::vector<uint8_t> binOut;
    binOut.reserve((size_t)Width * Height * 4);

    const int RowPaddingWidth = ((Width + 3) & ~3);

    for (int i = Height; i > 0; i--) {

        for (int x = 0; x < Width; x++) {

            int colorIndex = rawData.ColormapPixelData[x];

            binOut.push_back(rawData.ColorTableRed[colorIndex]);
            binOut.push_back(rawData.ColorTableGreen[colorIndex]);
            binOut.push_back(rawData.ColorTableBlue[colorIndex]);
            binOut.push_back(rawData.ColorTableAlpha[colorIndex]);

        }

        SWFShift(rawData.ColormapPixelData, RowPaddingWidth);

    }

    return binOut;

}

std::vector<uint8_t> get15BitImage(BITMAPDATA rawData, uint16_t Width, uint16_t Height) {

std::vector<uint8_t> binOut;
binOut.reserve((size_t)Width * Height * 4);

const int RowPaddingBytes = (((Width * 2) + 3) & ~3);
const int PixelsPerRow = RowPaddingBytes / 2;

for (int i = 0; i < Height; i++) {

for (int x = 0; x < Width; x++) {

int idx = i * PixelsPerRow + x;

binOut.push_back(rawData.BitmapPixelData15[idx].Pix15Red << 3);
binOut.push_back(rawData.BitmapPixelData15[idx].Pix15Green << 3);
binOut.push_back(rawData.BitmapPixelData15[idx].Pix15Blue << 3);
binOut.push_back(255); // Alpha

        }

    }

return binOut;

}

std::vector<uint8_t> get24BitImage(BITMAPDATA rawData, uint16_t Width, uint16_t Height) {

    std::vector<uint8_t> binOut;
    binOut.reserve((size_t)Width * Height * 4);

    for (int i = 0; i < Height; i++) {

        for (int x = 0; x < Width; x++) {

            int idx = i * Width + x;

            binOut.push_back(rawData.BitmapPixelData24[idx].Pix24Red);
            binOut.push_back(rawData.BitmapPixelData24[idx].Pix24Green);
            binOut.push_back(rawData.BitmapPixelData24[idx].Pix24Blue);
            binOut.push_back(255); // Alpha

        }

    }

    return binOut;

}

std::vector<uint8_t> get32BitAlphaImage(ALPHABITMAPDATA rawData, uint16_t Width, uint16_t Height) {

    std::vector<uint8_t> binOut;
    binOut.reserve((size_t)Width * Height * 4);

    for (int i = 0; i < Height; i++) {

        for (int x = 0; x < Width; x++) {

            int idx = i * Width + x;

            binOut.push_back(rawData.BitmapPixelDataRed[idx]);
            binOut.push_back(rawData.BitmapPixelDataGreen[idx]);
            binOut.push_back(rawData.BitmapPixelDataBlue[idx]);
            binOut.push_back(rawData.BitmapPixelDataAlpha[idx]);

        }

    }

    return binOut;

}