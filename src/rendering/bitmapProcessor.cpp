#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <csetjmp>
#include <zlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include "../../include/wuffs/wuffs-v0.4.c"
#include "../tags/tags.h"
#include "bitmapProcessor.h"

struct JpegErrorMgr {
    jpeg_error_mgr pub;
    jmp_buf setjmpBuffer;
};

void jpegErrorExit(j_common_ptr cinfo) {

    JpegErrorMgr* err = reinterpret_cast<JpegErrorMgr*>(cinfo->err);

    longjmp(err->setjmpBuffer, 1);

}

bool decodeJpeg(const uint8_t* data, size_t dataSize, int& outWidth, int& outHeight, uint8_t*& outPixels) {

    std::cout << "decodeJpeg called, dataSize=" << dataSize << "\n";

    jpeg_decompress_struct cinfo;
    JpegErrorMgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = jpegErrorExit;

    if ( setjmp(jerr.setjmpBuffer) ) {

        std::cout << "decodeJpeg: libjpeg error_exit triggered\n";
        jpeg_destroy_decompress(&cinfo);

        return false;

    }

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, data, dataSize);

    if ( jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK ) {

        std::cout << "decodeJpeg: jpeg_read_header failed\n";
        jpeg_destroy_decompress(&cinfo);

        return false;

    }

    std::cout << "decodeJpeg: header read OK\n";

    cinfo.out_color_space = JCS_EXT_RGBA;

    jpeg_start_decompress(&cinfo);

    outWidth  = cinfo.output_width;
    outHeight = cinfo.output_height;

    std::cout << "DefineBits: Width: "  << outWidth  << "\n";
    std::cout << "DefineBits: Height: " << outHeight << "\n";

    int rowStride = outWidth * cinfo.output_components;
    outPixels = static_cast<uint8_t*>(malloc(rowStride * outHeight));

    uint8_t* rowPointer = outPixels;

    while ( cinfo.output_scanline < cinfo.output_height ) {

        jpeg_read_scanlines(&cinfo, &rowPointer, 1);
        rowPointer += rowStride;

    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return true;

}

bool decodeWuffs(const uint8_t* data, size_t dataSize, int& outWidth, int& outHeight, uint8_t*& outPixels) {

    wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(
        const_cast<uint8_t*>(data), dataSize, true
    );

    wuffs_base__image_config ic;

    wuffs_png__decoder* dec = wuffs_png__decoder__alloc();

    if ( !dec ) {

        return false;

    }

    wuffs_base__status status = wuffs_png__decoder__decode_image_config(dec, &ic, &src);

    if ( !wuffs_base__status__is_ok(&status) ) {

        free(dec);

        return false;

    }

    outWidth  = (int)wuffs_base__pixel_config__width(&ic.pixcfg);
    outHeight = (int)wuffs_base__pixel_config__height(&ic.pixcfg);

    wuffs_base__pixel_config__set(
        &ic.pixcfg,
        WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL,
        WUFFS_BASE__PIXEL_SUBSAMPLING__NONE,
        (uint32_t)outWidth,
        (uint32_t)outHeight
    );

    size_t pixelDataSize = (size_t)outWidth * (size_t)outHeight * 4;
    outPixels = static_cast<uint8_t*>(malloc(pixelDataSize));

    wuffs_base__pixel_buffer pb;
    status = wuffs_base__pixel_buffer__set_from_slice(
        &pb, &ic.pixcfg,
        wuffs_base__make_slice_u8(outPixels, pixelDataSize)
    );

    if ( !wuffs_base__status__is_ok(&status) ) {

        free(outPixels);
        free(dec);
        outPixels = nullptr;

        return false;

    }

    size_t workbufSize = wuffs_png__decoder__workbuf_len(dec).max_incl;
    uint8_t* workbuf = static_cast<uint8_t*>(malloc(workbufSize));

    status = wuffs_png__decoder__decode_frame(
        dec, &pb, &src,
        WUFFS_BASE__PIXEL_BLEND__SRC,
        wuffs_base__make_slice_u8(workbuf, workbufSize),
        nullptr
    );

    free(workbuf);
    free(dec);

    if ( !wuffs_base__status__is_ok(&status) ) {

        free(outPixels);
        outPixels = nullptr;

        return false;

    }

    return true;

}

bool decodeWuffsGif(const uint8_t* data, size_t dataSize, int& outWidth, int& outHeight, uint8_t*& outPixels) {

    wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(
        const_cast<uint8_t*>(data), dataSize, true
    );

    wuffs_base__image_config ic;

    wuffs_gif__decoder* dec = wuffs_gif__decoder__alloc();

    if ( !dec ) {

        return false;

    }

    wuffs_base__status status = wuffs_gif__decoder__decode_image_config(dec, &ic, &src);

    if ( !wuffs_base__status__is_ok(&status) ) {

        free(dec);

        return false;

    }

    outWidth  = (int)wuffs_base__pixel_config__width(&ic.pixcfg);
    outHeight = (int)wuffs_base__pixel_config__height(&ic.pixcfg);

    wuffs_base__pixel_config__set(
        &ic.pixcfg,
        WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL,
        WUFFS_BASE__PIXEL_SUBSAMPLING__NONE,
        (uint32_t)outWidth,
        (uint32_t)outHeight
    );

    size_t pixelDataSize = (size_t)outWidth * (size_t)outHeight * 4;
    outPixels = static_cast<uint8_t*>(malloc(pixelDataSize));

    wuffs_base__pixel_buffer pb;
    status = wuffs_base__pixel_buffer__set_from_slice(
        &pb, &ic.pixcfg,
        wuffs_base__make_slice_u8(outPixels, pixelDataSize)
    );

    if ( !wuffs_base__status__is_ok(&status) ) {

        free(outPixels);
        free(dec);
        outPixels = nullptr;

        return false;

    }

    size_t workbufSize = wuffs_gif__decoder__workbuf_len(dec).max_incl;
    uint8_t* workbuf = static_cast<uint8_t*>(malloc(workbufSize));

    wuffs_base__frame_config fc;
    status = wuffs_gif__decoder__decode_frame_config(dec, &fc, &src);

    if ( !wuffs_base__status__is_ok(&status) ) {

        free(workbuf);
        free(outPixels);
        free(dec);
        outPixels = nullptr;

        return false;

    }

    status = wuffs_gif__decoder__decode_frame(
        dec, &pb, &src,
        WUFFS_BASE__PIXEL_BLEND__SRC,
        wuffs_base__make_slice_u8(workbuf, workbufSize),
        nullptr
    );

    free(workbuf);
    free(dec);

    if ( !wuffs_base__status__is_ok(&status) ) {

        free(outPixels);
        outPixels = nullptr;

        return false;

    }

    return true;

}

BitmapStream getBitmapStream(SWFTag DefineBitsTag, int bitsVersion) {

    BitmapStream binOut;

    if ( bitsVersion == 1 ) {

        std::vector<uint8_t>& jpegData = DefineBitsTag.DefineBits.DefineBits.JPEGData;

        decodeJpeg(jpegData.data(), jpegData.size(), binOut.width, binOut.height, binOut.pixels);

    }

    if ( bitsVersion == 2 ) {

        std::vector<uint8_t>& imageData = DefineBitsTag.DefineBits.DefineBitsJPEG2.ImageData;

        std::cout << "First 16 bytes: " << std::hex << std::uppercase;
        for (int b = 0; b < 16 && b < (int)imageData.size(); b++) {
            std::cout << (int)imageData[b] << " ";
        }
        std::cout << std::dec << "\n";

        if ( imageData.size() >= 4 &&
             imageData[0] == 0x89 && imageData[1] == 0x50 &&
             imageData[2] == 0x4E && imageData[3] == 0x47 ) {

            decodeWuffs(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);
            binOut.alphaMode = BitmapAlphaMode::Unpremultiplied;

        }

        else if ( imageData.size() >= 6 &&
                  imageData[0] == 0x47 && imageData[1] == 0x49 &&
                  imageData[2] == 0x46 && imageData[3] == 0x38 &&
                  imageData[4] == 0x39 && imageData[5] == 0x61 ) {

            decodeWuffsGif(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);
            binOut.alphaMode = BitmapAlphaMode::Unpremultiplied;

        }

        else {

            decodeJpeg(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);

        }

    }

    if ( bitsVersion == 3 ) { 

        std::vector<uint8_t>& imageData = DefineBitsTag.DefineBits.DefineBitsJPEG3.ImageData;
        std::vector<uint8_t>& alphaData = DefineBitsTag.DefineBits.DefineBitsJPEG3.BitmapAlphaData;

        if ( imageData.size() >= 4 &&
             imageData[0] == 0x89 && imageData[1] == 0x50 &&
             imageData[2] == 0x4E && imageData[3] == 0x47 ) {

            decodeWuffs(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);
            binOut.alphaMode = BitmapAlphaMode::Unpremultiplied;

        }

        else if ( imageData.size() >= 6 &&
                  imageData[0] == 0x47 && imageData[1] == 0x49 &&
                  imageData[2] == 0x46 && imageData[3] == 0x38 &&
                  imageData[4] == 0x39 && imageData[5] == 0x61 ) {

            decodeWuffsGif(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);
            binOut.alphaMode = BitmapAlphaMode::Unpremultiplied;

        }

        else {

            decodeJpeg(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);

            if ( binOut.pixels && !alphaData.empty() ) {

                uLongf alphaSize = (uLongf)(binOut.width * binOut.height);
                uint8_t* alphaPixels = static_cast<uint8_t*>(malloc(alphaSize));

                int result = uncompress(alphaPixels, &alphaSize, alphaData.data(), (uLong)alphaData.size());

                std::cout << "DefineBitsJPEG3: alpha uncompress result=" << result << " alphaSize=" << alphaSize << " expected=" << (binOut.width * binOut.height) << " alphaData.size()=" << alphaData.size() << "\n";

                if ( result == Z_OK ) {

                    for (int i = 0; i < binOut.width * binOut.height; i++) {

                        uint8_t a = alphaPixels[i];

                        binOut.pixels[i * 4 + 0] = (uint8_t)((binOut.pixels[i * 4 + 0] * a) / 255);
                        binOut.pixels[i * 4 + 1] = (uint8_t)((binOut.pixels[i * 4 + 1] * a) / 255);
                        binOut.pixels[i * 4 + 2] = (uint8_t)((binOut.pixels[i * 4 + 2] * a) / 255);
                        binOut.pixels[i * 4 + 3] = a;

                    }

                    binOut.alphaMode = BitmapAlphaMode::Premultiplied;

                    std::cout << "DefineBitsJPEG3: final alpha bytes at row 0 -> x=0:" << (int)binOut.pixels[3]
                              << " x=250:" << (int)binOut.pixels[250 * 4 + 3]
                              << " x=500:" << (int)binOut.pixels[500 * 4 + 3]
                              << " x=750:" << (int)binOut.pixels[750 * 4 + 3]
                              << " x=999:" << (int)binOut.pixels[999 * 4 + 3] << "\n";

                }

                free(alphaPixels);

            }

        }

    }

    if ( bitsVersion == 4 ) { 

        std::vector<uint8_t>& imageData = DefineBitsTag.DefineBits.DefineBitsJPEG4.ImageData;
        std::vector<uint8_t>& alphaData = DefineBitsTag.DefineBits.DefineBitsJPEG4.BitmapAlphaData;

        if ( imageData.size() >= 4 &&
             imageData[0] == 0x89 && imageData[1] == 0x50 &&
             imageData[2] == 0x4E && imageData[3] == 0x47 ) {

            decodeWuffs(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);
            binOut.alphaMode = BitmapAlphaMode::Unpremultiplied;

        }

        else if ( imageData.size() >= 6 &&
                  imageData[0] == 0x47 && imageData[1] == 0x49 &&
                  imageData[2] == 0x46 && imageData[3] == 0x38 &&
                  imageData[4] == 0x39 && imageData[5] == 0x61 ) {

            decodeWuffsGif(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);
            binOut.alphaMode = BitmapAlphaMode::Unpremultiplied;

        }

        else {

            decodeJpeg(imageData.data(), imageData.size(), binOut.width, binOut.height, binOut.pixels);

            if ( binOut.pixels && !alphaData.empty() ) {

                uLongf alphaSize = (uLongf)(binOut.width * binOut.height);
                uint8_t* alphaPixels = static_cast<uint8_t*>(malloc(alphaSize));

                int result = uncompress(alphaPixels, &alphaSize, alphaData.data(), (uLong)alphaData.size());

                std::cout << "DefineBitsJPEG4: alpha uncompress result=" << result << " alphaSize=" << alphaSize << " expected=" << (binOut.width * binOut.height) << " alphaData.size()=" << alphaData.size() << "\n";

                if ( result == Z_OK ) {

                    for (int i = 0; i < binOut.width * binOut.height; i++) {

                        uint8_t a = alphaPixels[i];

                        binOut.pixels[i * 4 + 0] = (uint8_t)((binOut.pixels[i * 4 + 0] * a) / 255);
                        binOut.pixels[i * 4 + 1] = (uint8_t)((binOut.pixels[i * 4 + 1] * a) / 255);
                        binOut.pixels[i * 4 + 2] = (uint8_t)((binOut.pixels[i * 4 + 2] * a) / 255);
                        binOut.pixels[i * 4 + 3] = a;

                    }

                    binOut.alphaMode = BitmapAlphaMode::Premultiplied;

                    std::cout << "DefineBitsJPEG4: final alpha bytes at row 0 -> x=0:" << (int)binOut.pixels[3]
                              << " x=250:" << (int)binOut.pixels[250 * 4 + 3]
                              << " x=500:" << (int)binOut.pixels[500 * 4 + 3]
                              << " x=750:" << (int)binOut.pixels[750 * 4 + 3]
                              << " x=999:" << (int)binOut.pixels[999 * 4 + 3] << "\n";

                }

                free(alphaPixels);

            }

        }

    }

    if ( bitsVersion == 5 ) {

        binOut.width = DefineBitsTag.DefineBits.DefineBitsLossless.BitmapWidth;
        binOut.height = DefineBitsTag.DefineBits.DefineBitsLossless.BitmapHeight;
        binOut.pixels = DefineBitsTag.DefineBits.DefineBitsLossless.RawImage;

    }

    if ( bitsVersion == 6 ) {

        binOut.width = DefineBitsTag.DefineBits.DefineBitsLossless.BitmapWidth;
        binOut.height = DefineBitsTag.DefineBits.DefineBitsLossless.BitmapHeight;
        binOut.pixels = DefineBitsTag.DefineBits.DefineBitsLossless.RawImage;
        binOut.alphaMode = BitmapAlphaMode::Premultiplied;

    }


    return binOut;

}