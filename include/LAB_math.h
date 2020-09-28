#pragma once

#include <math.h>

#define LAB_PI 3.14159265359


static inline float LAB_AbsModF(float num, float den)
{
    float t = fmod(num, den);
    if(num < 0) t+=den;
    if(!(abs(t)<abs(den))) t = 0;
    return t;
}
