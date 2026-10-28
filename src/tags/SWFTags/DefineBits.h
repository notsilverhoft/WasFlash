#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "../tags.h"

SWFTag getDefineBitsTag(rawSWFTag rawTag, int bitsVersion, const std::vector<uint8_t>& JpegTables);