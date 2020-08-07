#pragma once

#ifdef __GNUC__

// Count set bits
#define LAB_PopCnt(intnum) __builtin_popcount(intnum)

// Count leading zeros
//#define LAB_Clz(intnum) __builtin_clz(intnum)

// Count trailing zeros
// UB if intnum is 0
#define LAB_Ctz(intnum) __builtin_ctz(intnum)

#else
LAB_PURE
static inline int LAB_PopCnt(unsigned v)
{
    // Slightly modified:  https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan


    // v: count the number of bits set in v
    unsigned int c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
    {
        v &= v - 1; // clear the least significant bit set
    }
    return c;
}

LAB_PURE
static inline int LAB_Ctz(unsigned v)
{
    // Slightly modified:  https://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightParallel
    // v: 32-bit word input to count zero bits on right
    unsigned c = 32; // c will be the number of zero bits on the right
    v &= -v;
    if (v) c--;
    if (v & 0x0000FFFF) c -= 16;
    if (v & 0x00FF00FF) c -= 8;
    if (v & 0x0F0F0F0F) c -= 4;
    if (v & 0x33333333) c -= 2;
    if (v & 0x55555555) c -= 1;
    return c;
}

#endif


#if (-1 >> 1) == -1
#define LAB_Sar(x, n) ((x) >> (n))
#else
// Because C does not guarantee that x >> n sign extends for
// signed numbers, here is an implementation that does this
LAB_PURE
static inline int LAB_Sar(int x, int n)
{
    return x < 0 ? ~(~x >> n) : x >> n;
}
#endif
