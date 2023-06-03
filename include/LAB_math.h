#pragma once

#include "LAB_attr.h"
#include "LAB_opt.h"
#include <math.h>

#include "LAB_select.h"

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



LAB_INLINE
LAB_VALUE_CONST
float LAB_fMix(float a, float b, float mult)
{
    return a + (b-a)*mult;
}


LAB_INLINE
LAB_VALUE_CONST
float LAB_fSmoothMin(float a, float b, float k)
{
    float mult = 0.5f + 0.5f*(a-b)/k;
    mult = LAB_CLAMP(mult, 0.f, 1.f);
    return LAB_fMix(a, b, mult) - k*mult*(1.f-mult);
}

#define LAB_fSmoothMax(a, b, c) (-LAB_fSmoothMin(-(a), -(b), c))



LAB_INLINE
LAB_VALUE_CONST
float LAB_fSmoothStep(float x, float a, float b)
{
    float f = (x - a) / (b - a);
    f = LAB_CLAMP(f, 0.0, 1.0);
    return f * f * (3 - 2 * f);
}

LAB_INLINE
LAB_VALUE_CONST
float LAB_fSmoothClamp(float x, float a, float b)
{
    return LAB_fSmoothStep(x, a, b) * (b-a) + a;
}






#define LAB_EPSILON (1e-5)
#define LAB_APPROX_EQ(a, b) (fabs((a) - (b)) < LAB_EPSILON)
