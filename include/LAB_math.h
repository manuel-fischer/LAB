#pragma once

#include "LAB_attr.h"
#include <math.h>

#define LAB_PI 3.14159265359


LAB_INLINE float LAB_AbsModF(float num, float den)
{
    float t = fmod(num, den);
    if(num < 0) t+=den;
    if(!(abs(t)<abs(den))) t = 0;
    return t;
}

/**
 * Use this if the observable behavior is the same, when
 * x could also be x-1 for integral x
 */
LAB_PURE
LAB_INLINE int LAB_FastFloorF2I(float x)
{
    // TODO: wrong values if x is integral and negative
    return (int)x - (x<0);
}

LAB_PURE
LAB_INLINE int LAB_FastFloorD2I(double x)
{
    // TODO: wrong values if x is integral and negative
    return (int)x - (x<0);
}


LAB_PURE
LAB_INLINE int LAB_FloorF2I(float x)
{
    return (int)floorf(x);
}

LAB_PURE
LAB_INLINE int LAB_FloorD2I(double x)
{
    return (int)floor(x);
}
