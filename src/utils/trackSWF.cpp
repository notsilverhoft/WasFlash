#include <iostream>
#include <cstdint>
#include <vector>
#include "trackSWF.h"

void SWFShift(std::vector<uint8_t>& buffer, int amount) {
    if (amount <= 0) return;
    if (static_cast<size_t>(amount) >= buffer.size()) {
        buffer.clear();
        return;
    }
    buffer.erase(buffer.begin(), buffer.begin() + amount);
}

SWFBuffer SWFShift(SWFBuffer& buffer, int amount) {
    if (amount <= 0) return buffer;
    if (static_cast<size_t>(amount) >= buffer.size()) {
        buffer.pos = buffer.data.size();
        return buffer;
    }
    buffer.pos += amount;
    return buffer;
}