#include <iostream>
#include <cstdint>
#include <vector>
#include "../../../include/wuffs/wuffs-v0.4.c"
#include "../tags.h"
#include "../../base/bitmaps.h"
#include "../../base/rect.h"
#include "../../utils/trackSWF.h"

std::vector<uint8_t> zlibDecompress(const uint8_t* data, size_t dataSize, size_t outSize) {

    std::vector<uint8_t> binOut(outSize);

    wuffs_zlib__decoder* dec = wuffs_zlib__decoder__alloc();

    if ( !dec ) {

        return {};

    }

    wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(
        const_cast<uint8_t*>(data), dataSize, true
    );

    wuffs_base__io_buffer dst = wuffs_base__ptr_u8__writer(
        binOut.data(), binOut.size()
    );

    size_t workbufSize = wuffs_zlib__decoder__workbuf_len(dec).max_incl;
    uint8_t* workbuf = static_cast<uint8_t*>(malloc(workbufSize));

    wuffs_base__status status = wuffs_zlib__decoder__transform_io(
        dec, &dst, &src, wuffs_base__make_slice_u8(workbuf, workbufSize)
    );

    free(workbuf);
    free(dec);

    std::cout << "zlibDecompress: requested=" << outSize << " actualDecompressed=" << dst.meta.wi << "\n";

    if ( !wuffs_base__status__is_ok(&status) ) {

        return {};

    }

    binOut.resize(dst.meta.wi);

    return binOut;

}

SWFTag getDefineBitsTag(rawSWFTag rawTag, int bitsVersion, const std::vector<uint8_t>& JpegTables) {

    SWFTag binOut;

    binOut.DefineBits.CharacterID = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
    SWFShift(rawTag.tagData, 2);

    if ( bitsVersion == 1 ) {

        binOut.DefineBits.DefineBits.JPEGData = JpegTables;

        if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD9 && rawTag.tagData[2] == 0xFF && rawTag.tagData[3] == 0xD8 ) {
            SWFShift(rawTag.tagData, 4);
        }

        if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD8 ) {
            SWFShift(rawTag.tagData, 2);
        }

        while (true) {

            if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD9 ) {
                binOut.DefineBits.DefineBits.JPEGData.push_back(rawTag.tagData[0]);
                binOut.DefineBits.DefineBits.JPEGData.push_back(rawTag.tagData[1]);
                SWFShift(rawTag.tagData, 2);
                break;
            }

            binOut.DefineBits.DefineBits.JPEGData.push_back(rawTag.tagData[0]);
            SWFShift(rawTag.tagData, 1);

        }

    }

    if ( bitsVersion == 2 ) {

        if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD9 && rawTag.tagData[2] == 0xFF && rawTag.tagData[3] == 0xD8 ) {

            SWFShift(rawTag.tagData, 4);

        }

        binOut.DefineBits.DefineBitsJPEG2.ImageData = rawTag.tagData;

    }

    if ( bitsVersion == 3 ) {

        binOut.DefineBits.DefineBitsJPEG3.AlphaDataOffset = ((rawTag.tagData[3] << 24) | (rawTag.tagData[2] << 16) | (rawTag.tagData[1] << 8) | (rawTag.tagData[0]));
        SWFShift(rawTag.tagData, 4);

        binOut.DefineBits.DefineBitsJPEG3.ImageData.resize(binOut.DefineBits.DefineBitsJPEG3.AlphaDataOffset);

        if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD9 && rawTag.tagData[2] == 0xFF && rawTag.tagData[3] == 0xD8 ) {

            SWFShift(rawTag.tagData, 4);
            
        }


        for (int i = 0; i < binOut.DefineBits.DefineBitsJPEG3.AlphaDataOffset; i++) {

            binOut.DefineBits.DefineBitsJPEG3.ImageData[i] = rawTag.tagData[i];
    
        }

        SWFShift(rawTag.tagData, binOut.DefineBits.DefineBitsJPEG3.AlphaDataOffset);

        binOut.DefineBits.DefineBitsJPEG3.BitmapAlphaData = rawTag.tagData;

    }

    

    if ( bitsVersion == 4 ) {

        binOut.DefineBits.DefineBitsJPEG4.AlphaDataOffset = ((rawTag.tagData[3] << 24) | (rawTag.tagData[2] << 16) | (rawTag.tagData[1] << 8) | (rawTag.tagData[0]));
        SWFShift(rawTag.tagData, 4);

        binOut.DefineBits.DefineBitsJPEG4.DeblockParam = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);

        SWFShift(rawTag.tagData, 2);

        binOut.DefineBits.DefineBitsJPEG4.ImageData.resize(binOut.DefineBits.DefineBitsJPEG4.AlphaDataOffset);

        if ( rawTag.tagData[0] == 0xFF && rawTag.tagData[1] == 0xD9 && rawTag.tagData[2] == 0xFF && rawTag.tagData[3] == 0xD8 ) {

            SWFShift(rawTag.tagData, 4);
            
        }

        for (int i = 0; i < binOut.DefineBits.DefineBitsJPEG4.AlphaDataOffset; i++) {

            binOut.DefineBits.DefineBitsJPEG4.ImageData[i] = rawTag.tagData[i];
    
        }

        SWFShift(rawTag.tagData, binOut.DefineBits.DefineBitsJPEG4.AlphaDataOffset);

        binOut.DefineBits.DefineBitsJPEG4.BitmapAlphaData = rawTag.tagData;

    }

    if ( bitsVersion == 5 ) {

        binOut.DefineBits.DefineBitsLossless.BitmapFormat = rawTag.tagData[0];
        SWFShift(rawTag.tagData, 1);

        binOut.DefineBits.DefineBitsLossless.BitmapWidth = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
        binOut.DefineBits.DefineBitsLossless.BitmapHeight = ((rawTag.tagData[3] << 8) | rawTag.tagData[2]);
        SWFShift(rawTag.tagData, 4);

        std::vector<uint8_t> decompressedData;
        uint32_t decompressedSize;

        if ( binOut.DefineBits.DefineBitsLossless.BitmapFormat == 3 ) {

            binOut.DefineBits.DefineBitsLossless.BitmapColorTableSize = rawTag.tagData[0];
            SWFShift(rawTag.tagData, 1);

            decompressedSize = (((binOut.DefineBits.DefineBitsLossless.BitmapColorTableSize + 1) * 3) + (binOut.DefineBits.DefineBitsLossless.BitmapHeight * ((binOut.DefineBits.DefineBitsLossless.BitmapWidth + 3) & ~3)));

            decompressedData.resize(decompressedSize);
            decompressedData = zlibDecompress(rawTag.tagData.data(), rawTag.tagData.size(), decompressedSize);

            std::vector<uint8_t> tempImgVector = get8BitColorMappedImage(getColormapData(decompressedData, binOut.DefineBits.DefineBitsLossless.BitmapColorTableSize, binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight), binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight);

            std::cout << "tempImgVector.size()=" << tempImgVector.size()
            << " expected=" << (binOut.DefineBits.DefineBitsLossless.BitmapWidth * binOut.DefineBits.DefineBitsLossless.BitmapHeight * 4) << "\n";

            binOut.DefineBits.DefineBitsLossless.RawImage = static_cast<uint8_t*>(malloc(tempImgVector.size()));
            memcpy(binOut.DefineBits.DefineBitsLossless.RawImage, tempImgVector.data(), tempImgVector.size());

        }

        if ( binOut.DefineBits.DefineBitsLossless.BitmapFormat == 4 ) {

            decompressedSize =  (binOut.DefineBits.DefineBitsLossless.BitmapHeight * ((binOut.DefineBits.DefineBitsLossless.BitmapWidth * 2 + 3) & ~3));

            decompressedData.resize(decompressedSize);
            decompressedData = zlibDecompress(rawTag.tagData.data(), rawTag.tagData.size(), decompressedSize);

            std::vector<uint8_t> tempImgVector = get15BitImage(getBitmapData(decompressedData, binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight, 4), binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight);

            std::cout << "tempImgVector.size()=" << tempImgVector.size()
            << " expected=" << (binOut.DefineBits.DefineBitsLossless.BitmapWidth * binOut.DefineBits.DefineBitsLossless.BitmapHeight * 4) << "\n";

            binOut.DefineBits.DefineBitsLossless.RawImage = static_cast<uint8_t*>(malloc(tempImgVector.size()));
            memcpy(binOut.DefineBits.DefineBitsLossless.RawImage, tempImgVector.data(), tempImgVector.size());
            
        }

        if ( binOut.DefineBits.DefineBitsLossless.BitmapFormat == 5 ) {

            decompressedSize =  (binOut.DefineBits.DefineBitsLossless.BitmapHeight * ((binOut.DefineBits.DefineBitsLossless.BitmapWidth * 4 + 3) & ~3));

            decompressedData.resize(decompressedSize);
            decompressedData = zlibDecompress(rawTag.tagData.data(), rawTag.tagData.size(), decompressedSize);

            std::vector<uint8_t> tempImgVector = get24BitImage(getBitmapData(decompressedData, binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight, 5), binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight);

            std::cout << "tempImgVector.size()=" << tempImgVector.size()
            << " expected=" << (binOut.DefineBits.DefineBitsLossless.BitmapWidth * binOut.DefineBits.DefineBitsLossless.BitmapHeight * 4) << "\n";

            binOut.DefineBits.DefineBitsLossless.RawImage = static_cast<uint8_t*>(malloc(tempImgVector.size()));
            memcpy(binOut.DefineBits.DefineBitsLossless.RawImage, tempImgVector.data(), tempImgVector.size());

        }

    }

    if ( bitsVersion == 6 ) {

        binOut.DefineBits.DefineBitsLossless.BitmapFormat = rawTag.tagData[0];
        SWFShift(rawTag.tagData, 1);

        binOut.DefineBits.DefineBitsLossless.BitmapWidth = ((rawTag.tagData[1] << 8) | rawTag.tagData[0]);
        binOut.DefineBits.DefineBitsLossless.BitmapHeight = ((rawTag.tagData[3] << 8) | rawTag.tagData[2]);
        SWFShift(rawTag.tagData, 4);

        std::vector<uint8_t> decompressedData;
        uint32_t decompressedSize;

        if ( binOut.DefineBits.DefineBitsLossless.BitmapFormat == 3 ) {

            binOut.DefineBits.DefineBitsLossless.BitmapColorTableSize = rawTag.tagData[0];
            SWFShift(rawTag.tagData, 1);

            decompressedSize = (((binOut.DefineBits.DefineBitsLossless.BitmapColorTableSize + 1) * 4) + (binOut.DefineBits.DefineBitsLossless.BitmapHeight * ((binOut.DefineBits.DefineBitsLossless.BitmapWidth + 3) & ~3)));

            decompressedData.resize(decompressedSize);
            decompressedData = zlibDecompress(rawTag.tagData.data(), rawTag.tagData.size(), decompressedSize);

            std::vector<uint8_t> tempImgVector = get8BitAlphaColorMappedImage(getAlphaColormapData(decompressedData, binOut.DefineBits.DefineBitsLossless.BitmapColorTableSize, binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight), binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight);

             std::cout << "tempImgVector.size()=" << tempImgVector.size()
            << " expected=" << (binOut.DefineBits.DefineBitsLossless.BitmapWidth * binOut.DefineBits.DefineBitsLossless.BitmapHeight * 4) << "\n";

            binOut.DefineBits.DefineBitsLossless.RawImage = static_cast<uint8_t*>(malloc(tempImgVector.size()));
            memcpy(binOut.DefineBits.DefineBitsLossless.RawImage, tempImgVector.data(), tempImgVector.size());

        }

        if ( binOut.DefineBits.DefineBitsLossless.BitmapFormat == 5 ) {

            decompressedSize =  (binOut.DefineBits.DefineBitsLossless.BitmapHeight * (binOut.DefineBits.DefineBitsLossless.BitmapWidth * 4));

            decompressedData.resize(decompressedSize);
            decompressedData = zlibDecompress(rawTag.tagData.data(), rawTag.tagData.size(), decompressedSize);

            std::vector<uint8_t> tempImgVector = get32BitAlphaImage(getAlphaBitmapData(decompressedData, binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight, 5), binOut.DefineBits.DefineBitsLossless.BitmapWidth, binOut.DefineBits.DefineBitsLossless.BitmapHeight);

            std::cout << "tempImgVector.size()=" << tempImgVector.size()
            << " expected=" << (binOut.DefineBits.DefineBitsLossless.BitmapWidth * binOut.DefineBits.DefineBitsLossless.BitmapHeight * 4) << "\n";

            binOut.DefineBits.DefineBitsLossless.RawImage = static_cast<uint8_t*>(malloc(tempImgVector.size()));
            memcpy(binOut.DefineBits.DefineBitsLossless.RawImage, tempImgVector.data(), tempImgVector.size());

        }

        

    }

    return binOut;

}