#include <iostream>
#include <cstdint>
#include <vector>

void SWFShift(std::vector<uint8_t>& buffer, int amount) {

    buffer.erase(buffer.begin(), buffer.begin() + amount);

    return;

}