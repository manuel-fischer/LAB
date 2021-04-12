#pragma once
/** \file LAB_bits.h
 *
 *  Some bit utility functions.
 */

#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_debug.h"

#ifndef DOXYGEN

#ifdef __GNUC__

// Count set bits
#define LAB_PopCnt(intnum) __builtin_popcount(intnum)

// Count leading zeros
//#define LAB_Clz(intnum) __builtin_clz(intnum)

// Count trailing zeros
// UB if intnum is 0
#define LAB_Ctz(intnum) __builtin_ctz(intnum)

#else
LAB_CONST
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

LAB_CONST
LAB_INLINE int LAB_Ctz(uint32_t v)
{
    LAB_ASSUME(v != 0);
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

LAB_CONST
LAB_INLINE int LAB_IsPow2(int v)
{
    LAB_ASSUME(v != 0);
    return (v & (v - 1)) == 0;
}

#if (-1 >> 1) == -1
#define LAB_Sar(x, n) ((x) >> (n))
#else

LAB_CONST
LAB_INLINE int LAB_Sar(int x, int n)
{
    return x < 0 ? ~(~x >> n) : x >> n;
}
#endif

LAB_CONST
LAB_INLINE uint32_t LAB_CeilPow2(uint32_t x)
{
    LAB_ASSUME(x != 0);
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}


LAB_CONST
LAB_INLINE uint32_t LAB_CeilPow2z(size_t x)
{
    LAB_ASSUME(x != 0);
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    if(sizeof(x)*8 == 64) x |= x >> 32;
    _Static_assert(sizeof(x)*8 <= 64, "");
    x++;
    return x;
}

LAB_CONST
LAB_INLINE uint32_t LAB_Capacity(size_t x)
{
    LAB_ASSUME(x != 0);
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    if(sizeof(x)*8 == 64) x |= x >> 32;
    _Static_assert(sizeof(x)*8 <= 64, "");
    x++;
    return x;
}

#else /* DOXYGEN SECTION */



/**
 *  Count set bits,
 *
 *  Used to get the cardinality of a bitset V
 *
 *  For the condition `|V| > 0` simply use `v != 0` or `v` instead
 *
 *  `LAB_PopCnt(v) == 1` checks if v is a pow of two, use
 *  `v != 0 &&` \ref LAB_IsPow2 `(v)` instead
 */
int LAB_PopCnt(uint32_t v);

// Count leading zeros
//#define LAB_Clz(intnum) __builtin_clz(intnum)

/**
 *  Count trailing zeros
 *
 *  Used to get smallest set bit of a bitset
 *
 *  UB if v is 0
 *
 *  Instead of `LAB_MIN(LAB_Ctz(v), c)` use `LAB_Ctz(v|1<<c)`, to
 *  allow for v being 0.
 */
int LAB_Ctz(uint32_t v);

/**
 *  Check if a number is a power of two
 *
 *  UB if v is 0
 */
int LAB_IsPow2(int v);


/**
 *  Shift arithmetic right, the same as `x >> n` for positive `x`.
 *  For negative `x`, the sign is used to fill the upper bits.
 *
 *  Because C does not guarantee that x >> n sign extends for
 *  signed numbers, this is an implementation that does this
 *
 *  Note: if `(-1 >> 1) == -1` is true at preprocessing time
 *        the expression `x >> n` is also used for negative x
 */
int LAB_Sar(int x, int n);

/**
 *  Round integer to the next power of 2
 *
 *  UB if x is 0
 */

uint32_t LAB_CeilPow2(uint32_t x);
/**
 *  Round integer to the next power of 2
 *
 *  UB if x is 0
 */
uint32_t LAB_CeilPow2z(size_t x

/**
 *  Round integer to the next power of 2
 *
 *  0 if x is 0
 */
uint32_t LAB_Capacity(size_t size);

#endif
