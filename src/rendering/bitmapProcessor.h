#pragma once

#include <cstdint>
#include <cstddef>

struct SWFTag;

enum class BitmapAlphaMode {
    Opaque,
    Premultiplied,
    Unpremultiplied
};

struct BitmapStream {
    int             width  = 0;
    int             height = 0;
    uint8_t*        pixels = nullptr;
    BitmapAlphaMode alphaMode = BitmapAlphaMode::Opaque;
};

bool decodeJpeg(const uint8_t* data, size_t dataSize, int& outWidth, int& outHeight, uint8_t*& outPixels);

BitmapStream getBitmapStream(SWFTag DefineBitsTag, int bitsVersion);