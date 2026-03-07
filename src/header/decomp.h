#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "header.h"
static LZMA NULLZMA;
std::vector<uint8_t> decompressSWF(std::vector<uint8_t>& SWFFile, uint32_t decompressedFileLength, int compressionType, uint32_t SWFCompressedLength = NULL, LZMA lzProperties = NULLZMA);
