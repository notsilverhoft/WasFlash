#include <iostream>
#include <cstdint>
#include <vector>

void SWFShift(std::vector<uint8_t>& SWFFile, int amount) {

    SWFFile.erase(SWFFile.begin(), SWFFile.begin() + amount);

    return;

}