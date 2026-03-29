#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include "../utils/trackSWF.h"

std::string getString(std::vector<uint8_t>& data) {
    std::string binOut;
    while (data[0] != 0) {
        binOut += (char)data[0];
        SWFShift(data, 1);
    }
    SWFShift(data, 1);
    return binOut;
}