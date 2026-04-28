#pragma once
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "trackSWF.h"

struct BitStream {
    const uint8_t* data;
    size_t sizeBytes;
    bool bounded;
    int bitPos;

    BitStream(uint8_t* arr) : data(arr), sizeBytes(0), bounded(false), bitPos(0) {}
    BitStream(const uint8_t* arr) : data(arr), sizeBytes(0), bounded(false), bitPos(0) {}
    BitStream(const std::vector<uint8_t>& arr) : data(arr.data()), sizeBytes(arr.size()), bounded(true), bitPos(0) {}

    void reset(const std::vector<uint8_t>& arr) {
        data = arr.data();
        sizeBytes = arr.size();
        bounded = true;
        bitPos = 0;
    }

    uint64_t readUnsigned(int n) {
        if (n <= 0) {
            return 0;
        }

        uint64_t result = 0;
        for (int i = 0; i < n; ++i) {
            size_t byteIndex = static_cast<size_t>((bitPos + i) / 8);
            if (bounded && byteIndex >= sizeBytes) {
                result <<= (n - i);
                bitPos += n;
                return result;
            }

            int bitIndex  = 7 - ((bitPos + i) % 8);
            result = (result << 1) | ((data[byteIndex] >> bitIndex) & 1);
        }
        bitPos += n;
        return result;
    }

    int64_t readSigned(int n) {
        if (n <= 0) {
            return 0;
        }

        uint64_t raw = readUnsigned(n);
        if (raw & (1ULL << (n - 1))) {
            raw |= ~((1ULL << n) - 1);
        }
        return static_cast<int64_t>(raw);
    }

    void align() {
        int rem = bitPos % 8;
        if (rem != 0) {
            bitPos += 8 - rem;
        }     
    }

    int align(int pos) {
        int rem = pos % 8;
        if (rem != 0) {
            pos += 8 - rem;
        }     
        return pos;
    }
    
    void shiftArrayToAlignment(std::vector<uint8_t>& data) {

        align();
        int bytesToShift = bitPos / 8;
        if (bytesToShift > 0) {
            SWFShift(data, bytesToShift);
            reset(data);
        }

    }
};