#pragma once

#include "LAB_attr.h"
#include "LAB_opt.h"
#include "LAB_stdinc.h"
#include "LAB_debug.h"
#include "LAB_bits.h"


// export LAB_MultOverflow
// export LAB_Sign
// export LAB_CompareZ
// export LAB_FloorDiv
// export LAB_CeilDiv
// export LAB_FloorMultiplePow2
// export LAB_CeilMultiplePow2
// export LAB_FloorDivPow2
// export LAB_CeilDivPow2
// export LAB_RoundUpAlign


LAB_ALWAYS_INLINE LAB_INLINE
LAB_VALUE_CONST
bool LAB_MultOverflow(size_t a, size_t b)
{
#ifdef __GNUC__
    size_t unused_result;
    return __builtin_mul_overflow(a, b, &unused_result);
#else
    return (a*b)/b < a;
#endif
}



LAB_INLINE
LAB_VALUE_CONST
int LAB_Sign(int i)
{
    return (i > 0) - (i < 0);
}

// return sign of difference
LAB_INLINE
LAB_VALUE_CONST
int LAB_CompareZ(size_t a, size_t b)
{
    return (a > b) - (a < b);
}

LAB_INLINE
LAB_VALUE_CONST
int LAB_FloorDiv(int n, int d)
{
    int q = n/d;
    int r = n%d;

    if ((r!=0) && ((r<0) != (d<0))) --q;
    return q;
}

LAB_INLINE
LAB_VALUE_CONST
int LAB_CeilDiv(int n, int d)
{
    return -LAB_FloorDiv(-n, d);
}


LAB_INLINE
LAB_VALUE_CONST
int LAB_FloorMultiplePow2(int n, int d_pow2)
{
    LAB_ASSERT(LAB_IsPow2(d_pow2));
    return n & ~(d_pow2-1);
}

LAB_INLINE
LAB_VALUE_CONST
int LAB_CeilMultiplePow2(int n, int d_pow2)
{
    LAB_ASSERT(LAB_IsPow2(d_pow2));
    return (n+d_pow2-1) & ~(d_pow2-1);
}


LAB_INLINE
LAB_VALUE_CONST
int LAB_FloorDivPow2(int n, int d_pow2)
{
    LAB_ASSERT(LAB_IsPow2(d_pow2));
    return LAB_FloorMultiplePow2(n, d_pow2)/d_pow2;
}

LAB_INLINE
LAB_VALUE_CONST
int LAB_CeilDivPow2(int n, int d_pow2)
{
    LAB_ASSERT(LAB_IsPow2(d_pow2));
    return LAB_CeilMultiplePow2(n, d_pow2)/d_pow2;
}



LAB_INLINE
LAB_VALUE_CONST
size_t LAB_RoundUpAlign(size_t i, size_t align)
{
    //return ((i+align-1) / align) * align;
    LAB_ASSERT(LAB_IsPow2(align));
    return ((i+align-1)) & ~(align-1);
}
