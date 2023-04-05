#pragma once

#include "LAB_attr.h"
#include "LAB_opt.h"
#include <math.h>

#define LAB_PI 3.14159265359


LAB_VALUE_CONST
LAB_INLINE float LAB_AbsModF(float num, float den)
{
    float t = fmod(num, den);
    if(num < 0) t+=den;
    if(!(fabs(t)<fabs(den))) t = 0;
    return t;
}

/**
 * Use this if the observable behavior is the same, when
 * x could also be x-1 for integral x
 *
 * WARNING: wrong values if x is integral and negative
 */
LAB_VALUE_CONST
LAB_INLINE int LAB_FastFloorF2I(float x)
{
    return (int)x - (x<0);
}

LAB_VALUE_CONST
LAB_INLINE int LAB_FastFloorD2I(double x)
{
    return (int)x - (x<0);
}


LAB_VALUE_CONST
LAB_INLINE int LAB_FloorF2I(float x)
{
    return (int)floorf(x);
}

LAB_VALUE_CONST
LAB_INLINE int LAB_FloorD2I(double x)
{
    return (int)floor(x);
}


LAB_VALUE_CONST
LAB_INLINE float LAB_FractF(float x)
{
    return x-(int)floorf(x);
}

LAB_VALUE_CONST
LAB_INLINE double LAB_FractD(double x)
{
    return x-(int)floor(x);
}


// Compile time Newton approximation
// [0, 255] =^= [0, 1]
#define LAB_SQRT_I8_ITER(n, x) ((x) ? ((x)+255*(n)/(x))/2 : 0)
#define LAB_SQRT_I8(n) LAB_SQRT_I8_ITER(n, \
                       LAB_SQRT_I8_ITER(n, \
                       LAB_SQRT_I8_ITER(n, \
                       LAB_SQRT_I8_ITER(n, n))))
