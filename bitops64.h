#ifndef BITOPS64_H
#define BITOPS64_H

#include <stdint.h>
#include "bitops.h"  // original 32-bit functions

// 64-bit wrappers built 32-bit bitops
unsigned long long SetBit64(unsigned long long value, int position);
unsigned long long ClearBit64(unsigned long long value, int position);
int GetBit64(unsigned long long value, int position);
int CountBits64(unsigned long long value);
#endif
