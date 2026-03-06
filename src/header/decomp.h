#include <iostream>
#include <cstdint>
#include <vector>
#include <string>

std::vector<uint8_t> decompressSWF(std::vector<uint8_t>& SWFFile, uint32_t decompressedFileLength, int compressionType);