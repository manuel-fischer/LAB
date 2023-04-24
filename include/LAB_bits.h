#pragma once
/** \file LAB_bits.h
 *
 *  Some bit utility functions.
 */

#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_debug.h"
#include "LAB_stdinc.h"
#include <assert.h>

#ifndef DOXYGEN

#ifdef __GNUC__

// Count set bits
#define LAB_PopCnt(intnum) __builtin_popcountl(intnum)

// Count leading zeros
//#define LAB_Clz(intnum) __builtin_clz(intnum)

// Count trailing zeros
// UB if intnum is 0
#define LAB_Ctz(intnum) __builtin_ctzl(intnum)


#define LAB_Log2Floor(v) (8*sizeof(unsigned long) - 1 - __builtin_clzl((unsigned long)(v)))

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


LAB_CONST
LAB_INLINE int LAB_Log2Floor0(uint32_t v)
{
    int r, shift;

    r =     (v > 0xFFFF) << 4; v >>= r;
    shift = (v > 0xFF  ) << 3; v >>= shift; r |= shift;
    shift = (v > 0xF   ) << 2; v >>= shift; r |= shift;
    shift = (v > 0x3   ) << 1; v >>= shift; r |= shift;
                                            r |= (v >> 1);

    return v;
}

LAB_CONST
LAB_INLINE int LAB_Log2Floor(uint32_t v)
{
    LAB_ASSERT(v != 0);
    return LAB_Log2Floor0(v);
}


#endif

LAB_CONST
LAB_INLINE int LAB_IsPow2(int v)
{
    LAB_ASSUME(v != 0);
    return (v & (v - 1)) == 0;
}

LAB_CONST
LAB_INLINE int LAB_Log2OfPow2(int v)
{
    LAB_ASSERT(LAB_IsPow2(v));
    return LAB_Ctz(v);
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
LAB_INLINE size_t LAB_CeilPow2z(size_t x)
{
    LAB_ASSUME(x != 0);
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    if(sizeof(x)*8 == 64) x |= x >> 32;
    static_assert(sizeof(x)*8 <= 64, "");
    x++;
    return x;
}

LAB_CONST
LAB_INLINE size_t LAB_Capacity(size_t x)
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    if(sizeof(x)*8 == 64) x |= x >> 32;
    static_assert(sizeof(x)*8 <= 64, "");
    x++;
    return x;
}

// only values up to 2**21-1 are allowed
/*LAB_CONST
LAB_INLINE uint64_t LAB_StretchBits3xNaive(uint32_t x)
{
    LAB_ASSERT(x < (1<<21));

    uint64_t bits = 0;
    uint64_t mask1 = 1;
    uint64_t mask3 = 1;
    for(int i = 0; i < 21; ++i, mask1<<=1, mask3<<=3)
    {
        bits |= LAB_SELECT0(x&mask1, mask3);
    }
    return bits;
}*/

// only values up to 2**21-1 are allowed
LAB_CONST
LAB_INLINE uint64_t LAB_StretchBits3x(uint32_t x)
{
    LAB_ASSERT(x < (1<<21));

    // each byte of bit_array contains a value,
    // where a subset ob bits is set as in 0b01001001,
    // thus 00,01,08,09,40,41,48,49
    uint64_t bit_array = 0x4948414009080100ull;

    //uint64_t bits00 = ((bit_array >> ((x&000000007ul)<< 3)) & 0xffull)      ;
    //uint64_t bits03 = ((bit_array >> ((x&000000070ul)    )) & 0xffull) <<  9;
    //uint64_t bits06 = ((bit_array >> ((x&000000700ul)>> 3)) & 0xffull) << 18;
    //uint64_t bits09 = ((bit_array >> ((x&000007000ul)>> 6)) & 0xffull) << 27;
    //uint64_t bits12 = ((bit_array >> ((x&000070000ul)>> 9)) & 0xffull) << 36;
    //uint64_t bits15 = ((bit_array >> ((x&000700000ul)>>12)) & 0xffull) << 45;
    //uint64_t bits18 = ((bit_array >> ((x&007000000ul)>>15)) & 0xffull) << 54;

    uint64_t bits00 = ((bit_array >> ((x<< 3) & 070)) & 0xffull)      ;
    uint64_t bits03 = ((bit_array >> ((x    ) & 070)) & 0xffull) <<  9;
    uint64_t bits06 = ((bit_array >> ((x>> 3) & 070)) & 0xffull) << 18;
    uint64_t bits09 = ((bit_array >> ((x>> 6) & 070)) & 0xffull) << 27;
    uint64_t bits12 = ((bit_array >> ((x>> 9) & 070)) & 0xffull) << 36;
    uint64_t bits15 = ((bit_array >> ((x>>12) & 070)) & 0xffull) << 45;
    uint64_t bits18 = ((bit_array >> ((x>>15) & 070)) & 0xffull) << 54;

    return bits00 + bits03 + bits06 + bits09 + bits12 + bits15 + bits18;
}

// only values up to 2**21-1 are allowed
LAB_CONST
LAB_INLINE uint64_t LAB_IntermangleBits3(uint32_t x, uint32_t y, uint32_t z)
{
    return LAB_StretchBits3x(x) + LAB_StretchBits3x(y)*2 + LAB_StretchBits3x(z)*4;
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
size_t LAB_CeilPow2z(size_t x);

/**
 *  Round integer to the next power of 2
 *
 *  0 if x is 0
 */
size_t LAB_Capacity(size_t x);

#endif
