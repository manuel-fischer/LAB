#pragma once

#include "LAB_opt.h"
#include "LAB_attr.h"

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
LAB_INLINE int LAB_PopCnt(uint32_t v)
{
    // Slightly modified:  https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan


    // v: count the number of bits set in v
    uint32_t c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
    {
        v &= v - 1; // clear the least significant bit set
    }
    return c;
}

LAB_PURE
LAB_INLINE int LAB_Ctz(uint32_t v)
{
    // Slightly modified:  https://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightParallel
    // v: 32-bit word input to count zero bits on right
    uint32_t c = 32; // c will be the number of zero bits on the right
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
LAB_INLINE int LAB_Sar(int x, int n)
{
    return x < 0 ? ~(~x >> n) : x >> n;
}
#endif

LAB_PURE
LAB_INLINE uint32_t LAB_CeilPow2(uint32_t x)
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}
