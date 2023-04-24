#pragma once

#include "LAB_vec.h"

#define LAB_MAT_DEF LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST

// Column major order, as in OpenGL. It eases computation.
typedef struct LAB_Mat4F
{
    // x is the vector that (1, 0, 0, 0) gets mapped to, etc...
    LAB_Vec4F x, y, z, w;
} LAB_Mat4F;

#define LAB_Mat4F_AsArray(a) (&(a)->x.x)
#define LAB_Mat4F_AsCArray(a) ((const float*)&(a)->x.x)

// utility macro to define matrix non-transposed
#define LAB_MAT4_DEF(type, m00, m01, m02, m03, \
                           m10, m11, m12, m13, \
                           m20, m21, m22, m23, \
                           m30, m31, m32, m33, empty) \
    (type) \
    { \
        { m00, m10, m20, m30 }, \
        { m01, m11, m21, m31 }, \
        { m02, m12, m22, m32 }, \
        { m03, m13, m23, m33 }, \
    }

LAB_MAT_DEF
LAB_Vec4F LAB_Mat4F_RMul(LAB_Mat4F a, LAB_Vec4F b)
{
    return LAB_REDUCE_4(LAB_Vec4F_Add,
        LAB_Vec4F_RMul(a.x, b.x),
        LAB_Vec4F_RMul(a.y, b.y),
        LAB_Vec4F_RMul(a.z, b.z),
        LAB_Vec4F_RMul(a.w, b.w)
    );
}

LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Chain(LAB_Mat4F a, LAB_Mat4F b)
{
    // could be optimized
    return (LAB_Mat4F)
    {
        .x = LAB_Mat4F_RMul(a, b.x),
        .y = LAB_Mat4F_RMul(a, b.y),
        .z = LAB_Mat4F_RMul(a, b.z),
        .w = LAB_Mat4F_RMul(a, b.w),
    };
}



///// Higher Level Matrix Functions /////
// should replace OpenGL-calls with similar names


LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Scale3(float x, float y, float z)
{
    return LAB_MAT4_DEF(LAB_Mat4F,
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1,
    );
}

LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Scale1(float xyz)
{
    return LAB_Mat4F_Scale3(xyz, xyz, xyz);
}

LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Scale3V(LAB_Vec3F v)
{
    return LAB_Mat4F_Scale3(v.x, v.y, v.z);
}

LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Translate3(float x, float y, float z)
{
    return LAB_MAT4_DEF(LAB_Mat4F,
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1,
    );
}

LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Translate3V(LAB_Vec3F v)
{
    return LAB_Mat4F_Translate3(v.x, v.y, v.z);
}



// assume (x, y, z) is normalized
LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Rotate3Rad(float angle, float x, float y, float z)
{
    float s = sinf(angle);
    float c = cosf(angle);
    float c1 = 1-c;
    return LAB_MAT4_DEF(LAB_Mat4F,
        x*x*c1 +   c,   x*y*c1 - z*s,   x*z*c1 + y*s,   0,
        y*x*c1 + z*s,   y*y*c1 +   c,   y*z*c1 - x*s,   0,
        z*x*c1 - y*s,   z*y*c1 + x*s,   z*z*c1 +   c,   0,
                   0,              0,              0,   1,
    );
}

// assume (x, y, z) is normalized
LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Rotate3Deg(float angle, float x, float y, float z)
{
    return LAB_Mat4F_Rotate3Rad(angle * (float)(M_PI/180.f), x, y, z);
}




LAB_MAT_DEF
LAB_Mat4F LAB_Mat4F_Frustum(float left, float right,
                            float bottom, float top,
                            float near_val, float far_val)
{
    float x = (2*near_val)/(right-left);
    float y = (2*near_val)/(top-bottom);
    float a = (right+left)/(right-left);
    float b = (top+bottom)/(top-bottom);
    float c = -(far_val+near_val)/(far_val-near_val);
    float d = -(2*far_val*near_val)/(far_val-near_val);

    return LAB_MAT4_DEF(LAB_Mat4F,
        x, 0, a, 0,
        0, y, b, 0,
        0, 0, c, d,
        0, 0,-1, 0,
    );
}