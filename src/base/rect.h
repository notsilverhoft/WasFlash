#pragma once
#include <iostream>
#include <cstdint>
#include <vector>


struct RECT {
    int xMin;
    int xMax;
    int yMin;
    int yMax;
};

RECT getRect(std::vector<uint8_t>& SWFFile, bool headerRect = false);