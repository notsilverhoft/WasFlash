#include <iostream>
#include <cstdint>
#include <vector>

#pragma once
#include <vector>
#include <cstdint>

struct SWFBuffer {
    std::vector<uint8_t> data;
    size_t pos = 0;

    uint8_t operator[](size_t i) const { return data[pos + i]; }
    size_t size() const { return data.size() - pos; }
    bool empty() const { return pos >= data.size(); }
    const uint8_t* begin() const { return data.data() + pos; }
};

void SWFShift(std::vector<uint8_t>& buffer, int amount);
SWFBuffer SWFShift(SWFBuffer& buffer, int amount);
