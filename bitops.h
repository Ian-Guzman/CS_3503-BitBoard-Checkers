#ifndef BITOPS_H
#define BITOPS_H

unsigned int SetBit(unsigned int value, int position);
unsigned int ClearBit(unsigned int value, int position);
unsigned int ToggleBit(unsigned int value, int position);
int GetBit(unsigned int value, int position);
int CountBits(unsigned int value);
unsigned int ShiftLeft(unsigned int value, int positions);
unsigned int ShiftRight(unsigned int value, int positions);
void PrintBinary(unsigned int value);
void PrintHex(unsigned int value);
#endif