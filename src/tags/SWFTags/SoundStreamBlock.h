#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../tags.h"

std::vector<uint8_t> getSoundStreamBlock(std::vector<uint8_t> block, int compression);