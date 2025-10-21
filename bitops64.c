#include "bitops64.h"

// Set bit in a 64-bit integer using 32-bit SetBit twice
unsigned long long SetBit64(unsigned long long value, int position) {
    if (position < 32) {
        unsigned int low = (unsigned int)(value & 0xFFFFFFFFULL);
        low = SetBit(low, position);
        return (value & 0xFFFFFFFF00000000ULL) | low;
    } else {
        unsigned int high = (unsigned int)((value >> 32) & 0xFFFFFFFFULL);
        high = SetBit(high, position - 32);
        return ((unsigned long long)high << 32) | (value & 0xFFFFFFFFULL);
    }
}

unsigned long long ClearBit64(unsigned long long value, int position) {
    if (position < 32) {
        unsigned int low = (unsigned int)(value & 0xFFFFFFFFULL);
        low = ClearBit(low, position);
        return (value & 0xFFFFFFFF00000000ULL) | low;
    } else {
        unsigned int high = (unsigned int)((value >> 32) & 0xFFFFFFFFULL);
        high = ClearBit(high, position - 32);
        return ((unsigned long long)high << 32) | (value & 0xFFFFFFFFULL);
    }
}

int GetBit64(unsigned long long value, int position) {
    if (position < 32) {
        return GetBit((unsigned int)(value & 0xFFFFFFFFULL), position);
    } else {
        return GetBit((unsigned int)((value >> 32) & 0xFFFFFFFFULL), position - 32);
    }
}
int CountBits64(unsigned long long value) {
    unsigned int low = (unsigned int)(value & 0xFFFFFFFFULL);
    unsigned int high = (unsigned int)((value >> 32) & 0xFFFFFFFFULL);
    return CountBits(low) + CountBits(high);
}
