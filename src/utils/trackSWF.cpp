#include <iostream>
#include <cstdint>
#include <vector>

void SWFShift(std::vector<uint8_t>& buffer, int amount) {

    if (amount <= 0) {
        return;
    }

    if (static_cast<size_t>(amount) >= buffer.size()) {
        buffer.clear();
        return;
    }

    buffer.erase(buffer.begin(), buffer.begin() + amount);

    return;

}