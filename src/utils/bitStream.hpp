#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

struct BitStream {
    uint8_t* data;
    int bitPos;

    BitStream(uint8_t* arr) : data(arr), bitPos(0) {}

    uint64_t readUnsigned(int n) {
        uint64_t result = 0;
        for (int i = 0; i < n; ++i) {
            int byteIndex = (bitPos + i) / 8;
            int bitIndex  = 7 - ((bitPos + i) % 8);
            result = (result << 1) | ((data[byteIndex] >> bitIndex) & 1);
        }
        bitPos += n;
        return result;
    }

    int64_t readSigned(int n) {
        uint64_t raw = readUnsigned(n);
        if (raw & (1ULL << (n - 1))) {
            raw |= ~((1ULL << n) - 1);
        }
        return static_cast<int64_t>(raw);
    }
};