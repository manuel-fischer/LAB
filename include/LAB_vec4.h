#pragma once

#include "LAB_math.h"
#include "LAB_select.h"
#include "LAB_direction.h"
#include "LAB_bits.h"

#define LAB_V_MIN_INDEX4(x, y, z, w) LAB_SELECT_MIN3(x,0, y,1, LAB_MIN(z,w),LAB_SELECT_MIN(z,2, w,3))
#define LAB_V_MAX_INDEX4(x, y, z, w) LAB_SELECT_MAX3(x,0, y,1, LAB_MAX(z,w),LAB_SELECT_MAX(z,2, w,3))

#define LAB_VEC4_FROM(vtype, ptr) ((vtype) { (ptr)->x, (ptr)->y, (ptr)->z, (ptr)->w })

// export LAB_Vec4*
// export LAB_Box4*


typedef struct LAB_Vec4I { int x, y, z, w; } LAB_Vec4I;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Add(LAB_Vec4I a, LAB_Vec4I b) { return (LAB_Vec4I) { ((a.x) + (b.x)), ((a.y) + (b.y)), ((a.z) + (b.z)), ((a.w) + (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Sub(LAB_Vec4I a, LAB_Vec4I b) { return (LAB_Vec4I) { ((a.x) - (b.x)), ((a.y) - (b.y)), ((a.z) - (b.z)), ((a.w) - (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Neg(LAB_Vec4I a) { return (LAB_Vec4I) { (-(a.x)), (-(a.y)), (-(a.z)), (-(a.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec4I_Dot(LAB_Vec4I a, LAB_Vec4I b) { return ((((((a.x) * (b.x))) + (((a.y) * (b.y))))) + (((((a.z) * (b.z))) + (((a.w) * (b.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec4I_LengthSq(LAB_Vec4I a) { return ((((((a.x) * (a.x))) + (((a.y) * (a.y))))) + (((((a.z) * (a.z))) + (((a.w) * (a.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec4I_DistanceSq(LAB_Vec4I a, LAB_Vec4I b) { return LAB_Vec4I_LengthSq(LAB_Vec4I_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Mul(int a, LAB_Vec4I b) { return (LAB_Vec4I) { ((a) * (b.x)), ((a) * (b.y)), ((a) * (b.z)), ((a) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_RMul(LAB_Vec4I a, int b) { return (LAB_Vec4I) { ((a.x) * (b)), ((a.y) * (b)), ((a.z) * (b)), ((a.w) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Div(LAB_Vec4I a, int b) { return (LAB_Vec4I) { ((a.x) / (b)), ((a.y) / (b)), ((a.z) / (b)), ((a.w) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_HdMul(LAB_Vec4I a, LAB_Vec4I b) { return (LAB_Vec4I) { ((a.x) * (b.x)), ((a.y) * (b.y)), ((a.z) * (b.z)), ((a.w) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_HdDiv(LAB_Vec4I a, LAB_Vec4I b) { return (LAB_Vec4I) { ((a.x) / (b.x)), ((a.y) / (b.y)), ((a.z) / (b.z)), ((a.w) / (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_HdMin(LAB_Vec4I a, LAB_Vec4I b) { return (LAB_Vec4I) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y), LAB_MIN(a.z, b.z), LAB_MIN(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_HdMax(LAB_Vec4I a, LAB_Vec4I b) { return (LAB_Vec4I) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y), LAB_MAX(a.z, b.z), LAB_MAX(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec4I_GetMin(LAB_Vec4I a) { return LAB_MIN(LAB_MIN(a.x, a.y), LAB_MIN(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec4I_GetMax(LAB_Vec4I a) { return LAB_MAX(LAB_MAX(a.x, a.y), LAB_MAX(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4I_MinIndex(LAB_Vec4I a) { return LAB_V_MIN_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4I_MaxIndex(LAB_Vec4I a) { return LAB_V_MAX_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Abs(LAB_Vec4I a) { return (LAB_Vec4I) { abs(a.x), abs(a.y), abs(a.z), abs(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec4I LAB_Vec4I_FromArray(const int* array) { return (LAB_Vec4I) { array[0], array[1], array[2], array[3] }; }
LAB_INLINE LAB_ALWAYS_INLINE int* LAB_Vec4I_AsArray(LAB_Vec4I* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const int* LAB_Vec4I_AsCArray(const LAB_Vec4I* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE int* LAB_Vec4I_ToArray(int* array, LAB_Vec4I a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; array[3] = a.w; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec4I_Unpack(int* px, int* py, int* pz, int* pw, LAB_Vec4I a) { *px = a.x; *py = a.y; *pz = a.z; *pw = a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec4I_Get(LAB_Vec4I a, size_t i) { return LAB_Vec4I_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int* LAB_Vec4I_Ref(LAB_Vec4I* a, size_t i) { return &LAB_Vec4I_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const int* LAB_Vec4I_CRef(const LAB_Vec4I* a, size_t i) { return &LAB_Vec4I_AsCArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Sar(LAB_Vec4I a, int b) { return (LAB_Vec4I) { LAB_Sar(a.x, b), LAB_Sar(a.y, b), LAB_Sar(a.z, b), LAB_Sar(a.w, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Shr(LAB_Vec4I a, int b) { return (LAB_Vec4I) { ((a.x) >> (b)), ((a.y) >> (b)), ((a.z) >> (b)), ((a.w) >> (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4I_Shl(LAB_Vec4I a, int b) { return (LAB_Vec4I) { ((a.x) << (b)), ((a.y) << (b)), ((a.z) << (b)), ((a.w) << (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Vec4I_Equals(LAB_Vec4I a, LAB_Vec4I b) { return ((((((a.x) == (b.x))) && (((a.y) == (b.y))))) && (((((a.z) == (b.z))) && (((a.w) == (b.w)))))); }

typedef struct LAB_Vec4Z { size_t x, y, z, w; } LAB_Vec4Z;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Add(LAB_Vec4Z a, LAB_Vec4Z b) { return (LAB_Vec4Z) { ((a.x) + (b.x)), ((a.y) + (b.y)), ((a.z) + (b.z)), ((a.w) + (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Sub(LAB_Vec4Z a, LAB_Vec4Z b) { return (LAB_Vec4Z) { ((a.x) - (b.x)), ((a.y) - (b.y)), ((a.z) - (b.z)), ((a.w) - (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Neg(LAB_Vec4Z a) { return (LAB_Vec4Z) { (-(a.x)), (-(a.y)), (-(a.z)), (-(a.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_Dot(LAB_Vec4Z a, LAB_Vec4Z b) { return ((((((a.x) * (b.x))) + (((a.y) * (b.y))))) + (((((a.z) * (b.z))) + (((a.w) * (b.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_LengthSq(LAB_Vec4Z a) { return ((((((a.x) * (a.x))) + (((a.y) * (a.y))))) + (((((a.z) * (a.z))) + (((a.w) * (a.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_DistanceSq(LAB_Vec4Z a, LAB_Vec4Z b) { return LAB_Vec4Z_LengthSq(LAB_Vec4Z_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Mul(size_t a, LAB_Vec4Z b) { return (LAB_Vec4Z) { ((a) * (b.x)), ((a) * (b.y)), ((a) * (b.z)), ((a) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_RMul(LAB_Vec4Z a, size_t b) { return (LAB_Vec4Z) { ((a.x) * (b)), ((a.y) * (b)), ((a.z) * (b)), ((a.w) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Div(LAB_Vec4Z a, size_t b) { return (LAB_Vec4Z) { ((a.x) / (b)), ((a.y) / (b)), ((a.z) / (b)), ((a.w) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_HdMul(LAB_Vec4Z a, LAB_Vec4Z b) { return (LAB_Vec4Z) { ((a.x) * (b.x)), ((a.y) * (b.y)), ((a.z) * (b.z)), ((a.w) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_HdDiv(LAB_Vec4Z a, LAB_Vec4Z b) { return (LAB_Vec4Z) { ((a.x) / (b.x)), ((a.y) / (b.y)), ((a.z) / (b.z)), ((a.w) / (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_HdMin(LAB_Vec4Z a, LAB_Vec4Z b) { return (LAB_Vec4Z) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y), LAB_MIN(a.z, b.z), LAB_MIN(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_HdMax(LAB_Vec4Z a, LAB_Vec4Z b) { return (LAB_Vec4Z) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y), LAB_MAX(a.z, b.z), LAB_MAX(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_GetMin(LAB_Vec4Z a) { return LAB_MIN(LAB_MIN(a.x, a.y), LAB_MIN(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_GetMax(LAB_Vec4Z a) { return LAB_MAX(LAB_MAX(a.x, a.y), LAB_MAX(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_MinIndex(LAB_Vec4Z a) { return LAB_V_MIN_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_MaxIndex(LAB_Vec4Z a) { return LAB_V_MAX_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Abs(LAB_Vec4Z a) { return (LAB_Vec4Z) { (a.x), (a.y), (a.z), (a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec4Z LAB_Vec4Z_FromArray(const size_t* array) { return (LAB_Vec4Z) { array[0], array[1], array[2], array[3] }; }
LAB_INLINE LAB_ALWAYS_INLINE size_t* LAB_Vec4Z_AsArray(LAB_Vec4Z* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const size_t* LAB_Vec4Z_AsCArray(const LAB_Vec4Z* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE size_t* LAB_Vec4Z_ToArray(size_t* array, LAB_Vec4Z a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; array[3] = a.w; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec4Z_Unpack(size_t* px, size_t* py, size_t* pz, size_t* pw, LAB_Vec4Z a) { *px = a.x; *py = a.y; *pz = a.z; *pw = a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4Z_Get(LAB_Vec4Z a, size_t i) { return LAB_Vec4Z_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t* LAB_Vec4Z_Ref(LAB_Vec4Z* a, size_t i) { return &LAB_Vec4Z_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const size_t* LAB_Vec4Z_CRef(const LAB_Vec4Z* a, size_t i) { return &LAB_Vec4Z_AsCArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Sar(LAB_Vec4Z a, size_t b) { return (LAB_Vec4Z) { LAB_Sar(a.x, b), LAB_Sar(a.y, b), LAB_Sar(a.z, b), LAB_Sar(a.w, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Shr(LAB_Vec4Z a, size_t b) { return (LAB_Vec4Z) { ((a.x) >> (b)), ((a.y) >> (b)), ((a.z) >> (b)), ((a.w) >> (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4Z_Shl(LAB_Vec4Z a, size_t b) { return (LAB_Vec4Z) { ((a.x) << (b)), ((a.y) << (b)), ((a.z) << (b)), ((a.w) << (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Vec4Z_Equals(LAB_Vec4Z a, LAB_Vec4Z b) { return ((((((a.x) == (b.x))) && (((a.y) == (b.y))))) && (((((a.z) == (b.z))) && (((a.w) == (b.w)))))); }

typedef struct LAB_Vec4F { float x, y, z, w; } LAB_Vec4F;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_Add(LAB_Vec4F a, LAB_Vec4F b) { return (LAB_Vec4F) { ((a.x) + (b.x)), ((a.y) + (b.y)), ((a.z) + (b.z)), ((a.w) + (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_Sub(LAB_Vec4F a, LAB_Vec4F b) { return (LAB_Vec4F) { ((a.x) - (b.x)), ((a.y) - (b.y)), ((a.z) - (b.z)), ((a.w) - (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_Neg(LAB_Vec4F a) { return (LAB_Vec4F) { (-(a.x)), (-(a.y)), (-(a.z)), (-(a.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec4F_Dot(LAB_Vec4F a, LAB_Vec4F b) { return ((((((a.x) * (b.x))) + (((a.y) * (b.y))))) + (((((a.z) * (b.z))) + (((a.w) * (b.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec4F_LengthSq(LAB_Vec4F a) { return ((((((a.x) * (a.x))) + (((a.y) * (a.y))))) + (((((a.z) * (a.z))) + (((a.w) * (a.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec4F_DistanceSq(LAB_Vec4F a, LAB_Vec4F b) { return LAB_Vec4F_LengthSq(LAB_Vec4F_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_Mul(float a, LAB_Vec4F b) { return (LAB_Vec4F) { ((a) * (b.x)), ((a) * (b.y)), ((a) * (b.z)), ((a) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_RMul(LAB_Vec4F a, float b) { return (LAB_Vec4F) { ((a.x) * (b)), ((a.y) * (b)), ((a.z) * (b)), ((a.w) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_Div(LAB_Vec4F a, float b) { return (LAB_Vec4F) { ((a.x) / (b)), ((a.y) / (b)), ((a.z) / (b)), ((a.w) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_HdMul(LAB_Vec4F a, LAB_Vec4F b) { return (LAB_Vec4F) { ((a.x) * (b.x)), ((a.y) * (b.y)), ((a.z) * (b.z)), ((a.w) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_HdDiv(LAB_Vec4F a, LAB_Vec4F b) { return (LAB_Vec4F) { ((a.x) / (b.x)), ((a.y) / (b.y)), ((a.z) / (b.z)), ((a.w) / (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_HdMin(LAB_Vec4F a, LAB_Vec4F b) { return (LAB_Vec4F) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y), LAB_MIN(a.z, b.z), LAB_MIN(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_HdMax(LAB_Vec4F a, LAB_Vec4F b) { return (LAB_Vec4F) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y), LAB_MAX(a.z, b.z), LAB_MAX(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec4F_GetMin(LAB_Vec4F a) { return LAB_MIN(LAB_MIN(a.x, a.y), LAB_MIN(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec4F_GetMax(LAB_Vec4F a) { return LAB_MAX(LAB_MAX(a.x, a.y), LAB_MAX(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4F_MinIndex(LAB_Vec4F a) { return LAB_V_MIN_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4F_MaxIndex(LAB_Vec4F a) { return LAB_V_MAX_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4F_Abs(LAB_Vec4F a) { return (LAB_Vec4F) { fabsf(a.x), fabsf(a.y), fabsf(a.z), fabsf(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec4F LAB_Vec4F_FromArray(const float* array) { return (LAB_Vec4F) { array[0], array[1], array[2], array[3] }; }
LAB_INLINE LAB_ALWAYS_INLINE float* LAB_Vec4F_AsArray(LAB_Vec4F* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const float* LAB_Vec4F_AsCArray(const LAB_Vec4F* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE float* LAB_Vec4F_ToArray(float* array, LAB_Vec4F a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; array[3] = a.w; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec4F_Unpack(float* px, float* py, float* pz, float* pw, LAB_Vec4F a) { *px = a.x; *py = a.y; *pz = a.z; *pw = a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec4F_Get(LAB_Vec4F a, size_t i) { return LAB_Vec4F_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float* LAB_Vec4F_Ref(LAB_Vec4F* a, size_t i) { return &LAB_Vec4F_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const float* LAB_Vec4F_CRef(const LAB_Vec4F* a, size_t i) { return &LAB_Vec4F_AsCArray(a)[i]; }

typedef struct LAB_Vec4D { double x, y, z, w; } LAB_Vec4D;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_Add(LAB_Vec4D a, LAB_Vec4D b) { return (LAB_Vec4D) { ((a.x) + (b.x)), ((a.y) + (b.y)), ((a.z) + (b.z)), ((a.w) + (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_Sub(LAB_Vec4D a, LAB_Vec4D b) { return (LAB_Vec4D) { ((a.x) - (b.x)), ((a.y) - (b.y)), ((a.z) - (b.z)), ((a.w) - (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_Neg(LAB_Vec4D a) { return (LAB_Vec4D) { (-(a.x)), (-(a.y)), (-(a.z)), (-(a.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec4D_Dot(LAB_Vec4D a, LAB_Vec4D b) { return ((((((a.x) * (b.x))) + (((a.y) * (b.y))))) + (((((a.z) * (b.z))) + (((a.w) * (b.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec4D_LengthSq(LAB_Vec4D a) { return ((((((a.x) * (a.x))) + (((a.y) * (a.y))))) + (((((a.z) * (a.z))) + (((a.w) * (a.w)))))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec4D_DistanceSq(LAB_Vec4D a, LAB_Vec4D b) { return LAB_Vec4D_LengthSq(LAB_Vec4D_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_Mul(double a, LAB_Vec4D b) { return (LAB_Vec4D) { ((a) * (b.x)), ((a) * (b.y)), ((a) * (b.z)), ((a) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_RMul(LAB_Vec4D a, double b) { return (LAB_Vec4D) { ((a.x) * (b)), ((a.y) * (b)), ((a.z) * (b)), ((a.w) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_Div(LAB_Vec4D a, double b) { return (LAB_Vec4D) { ((a.x) / (b)), ((a.y) / (b)), ((a.z) / (b)), ((a.w) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_HdMul(LAB_Vec4D a, LAB_Vec4D b) { return (LAB_Vec4D) { ((a.x) * (b.x)), ((a.y) * (b.y)), ((a.z) * (b.z)), ((a.w) * (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_HdDiv(LAB_Vec4D a, LAB_Vec4D b) { return (LAB_Vec4D) { ((a.x) / (b.x)), ((a.y) / (b.y)), ((a.z) / (b.z)), ((a.w) / (b.w)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_HdMin(LAB_Vec4D a, LAB_Vec4D b) { return (LAB_Vec4D) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y), LAB_MIN(a.z, b.z), LAB_MIN(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_HdMax(LAB_Vec4D a, LAB_Vec4D b) { return (LAB_Vec4D) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y), LAB_MAX(a.z, b.z), LAB_MAX(a.w, b.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec4D_GetMin(LAB_Vec4D a) { return LAB_MIN(LAB_MIN(a.x, a.y), LAB_MIN(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec4D_GetMax(LAB_Vec4D a) { return LAB_MAX(LAB_MAX(a.x, a.y), LAB_MAX(a.z, a.w)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4D_MinIndex(LAB_Vec4D a) { return LAB_V_MIN_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec4D_MaxIndex(LAB_Vec4D a) { return LAB_V_MAX_INDEX4(a.x, a.y, a.z, a.w); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4D_Abs(LAB_Vec4D a) { return (LAB_Vec4D) { fabs(a.x), fabs(a.y), fabs(a.z), fabs(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec4D LAB_Vec4D_FromArray(const double* array) { return (LAB_Vec4D) { array[0], array[1], array[2], array[3] }; }
LAB_INLINE LAB_ALWAYS_INLINE double* LAB_Vec4D_AsArray(LAB_Vec4D* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const double* LAB_Vec4D_AsCArray(const LAB_Vec4D* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE double* LAB_Vec4D_ToArray(double* array, LAB_Vec4D a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; array[3] = a.w; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec4D_Unpack(double* px, double* py, double* pz, double* pw, LAB_Vec4D a) { *px = a.x; *py = a.y; *pz = a.z; *pw = a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec4D_Get(LAB_Vec4D a, size_t i) { return LAB_Vec4D_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double* LAB_Vec4D_Ref(LAB_Vec4D* a, size_t i) { return &LAB_Vec4D_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const double* LAB_Vec4D_CRef(const LAB_Vec4D* a, size_t i) { return &LAB_Vec4D_AsCArray(a)[i]; }

LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4I2F(LAB_Vec4I a) { return (LAB_Vec4F) { (float)(a.x), (float)(a.y), (float)(a.z), (float)(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4I2D(LAB_Vec4I a) { return (LAB_Vec4D) { (double)(a.x), (double)(a.y), (double)(a.z), (double)(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4F2D(LAB_Vec4F a) { return (LAB_Vec4D) { (double)(a.x), (double)(a.y), (double)(a.z), (double)(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4D2F_Cast(LAB_Vec4D a) { return (LAB_Vec4F) { (float)(a.x), (float)(a.y), (float)(a.z), (float)(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4F2I_FastFloor(LAB_Vec4F a) { return (LAB_Vec4I) { LAB_FastFloorF2I(a.x), LAB_FastFloorF2I(a.y), LAB_FastFloorF2I(a.z), LAB_FastFloorF2I(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4D2I_FastFloor(LAB_Vec4D a) { return (LAB_Vec4I) { LAB_FastFloorD2I(a.x), LAB_FastFloorD2I(a.y), LAB_FastFloorD2I(a.z), LAB_FastFloorD2I(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Vec4I2Z(LAB_Vec4I a) { return (LAB_Vec4Z) { (size_t)(a.x), (size_t)(a.y), (size_t)(a.z), (size_t)(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Vec4Z2I(LAB_Vec4Z a) { return (LAB_Vec4I) { (int)(a.x), (int)(a.y), (int)(a.z), (int)(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Vec4Z2F(LAB_Vec4Z a) { return (LAB_Vec4F) { (float)(a.x), (float)(a.y), (float)(a.z), (float)(a.w) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Vec4Z2D(LAB_Vec4Z a) { return (LAB_Vec4D) { (double)(a.x), (double)(a.y), (double)(a.z), (double)(a.w) }; }

typedef struct LAB_Box4I { LAB_Vec4I a, b; } LAB_Box4I;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4I_Add(LAB_Box4I a, LAB_Vec4I b) { return (LAB_Box4I) { LAB_Vec4I_Add(a.a, b), LAB_Vec4I_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4I_Expand(LAB_Box4I box, LAB_Vec4I vec) { return (LAB_Box4I) { LAB_Vec4I_Sub(box.a, vec), LAB_Vec4I_Add(box.b, vec) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4I_Mul(int factor, LAB_Box4I box) { return (LAB_Box4I) { LAB_Vec4I_Mul(factor, box.a), LAB_Vec4I_Mul(factor, box.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4I_Contains_Inc(LAB_Box4I box, LAB_Vec4I v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y && box.a.z <= v.z && v.z <= box.b.z && box.a.w <= v.w && v.w <= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4I_IsEmpty(LAB_Box4I box) { return box.a.x >= box.b.x || box.a.y >= box.b.y || box.a.z >= box.b.z || box.a.w >= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4I_Intersection(LAB_Box4I a, LAB_Box4I b) { return (LAB_Box4I) { LAB_Vec4I_HdMax(a.a, b.a), LAB_Vec4I_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4I_Intersects(LAB_Box4I a, LAB_Box4I b) { return !LAB_Box4I_IsEmpty(LAB_Box4I_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4I LAB_Box4I_Size(LAB_Box4I box) { return LAB_Vec4I_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4I_FromOriginAndSize(LAB_Vec4I origin, LAB_Vec4I size) { return (LAB_Box4I) { origin, LAB_Vec4I_Add(origin, size) }; }
LAB_INLINE LAB_ALWAYS_INLINE int (*LAB_Box4I_AsMDArray(LAB_Box4I* a))[4] { return (int (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE const int (*LAB_Box4I_AsCMDArray(const LAB_Box4I* a))[4] { return (const int (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_Vec4I* LAB_Box4I_AsArray(LAB_Box4I* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE const LAB_Vec4I* LAB_Box4I_AsCArray(const LAB_Box4I* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4I_Unpack(int* pax, int* pay, int* paz, int* paw, int* pbx, int* pby, int* pbz, int* pbw, LAB_Box4I a) { *pax = a.a.x; *pay = a.a.y; *paz = a.a.z; *paw = a.a.w; *pbx = a.b.x; *pby = a.b.y; *pbz = a.b.z; *pbw = a.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4I_Unpack_Sized(int* px, int* py, int* pz, int* pw, int* pdx, int* pdy, int* pdz, int* pdw, LAB_Box4I a) { *px = a.a.x; *py = a.a.y; *pz = a.a.z; *pw = a.a.w; *pdx = a.b.x-a.a.x; *pdy = a.b.y-a.a.y; *pdz = a.b.z-a.a.z; *pdw = a.b.w-a.a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4I_New_Sized(int x, int y, int z, int w, int dx, int dy, int dz, int dw) { return (LAB_Box4I) { { x, y, z, w }, { x + dx, y + dy, z + dz, w + dw } }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Box4I_DX(LAB_Box4I a) { return a.b.x - a.a.x; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Box4I_DY(LAB_Box4I a) { return a.b.y - a.a.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Box4I_DZ(LAB_Box4I a) { return a.b.z - a.a.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Box4I_DW(LAB_Box4I a) { return a.b.w - a.a.w; }

typedef struct LAB_Box4Z { LAB_Vec4Z a, b; } LAB_Box4Z;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4Z LAB_Box4Z_Add(LAB_Box4Z a, LAB_Vec4Z b) { return (LAB_Box4Z) { LAB_Vec4Z_Add(a.a, b), LAB_Vec4Z_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4Z LAB_Box4Z_Expand(LAB_Box4Z box, LAB_Vec4Z vec) { return (LAB_Box4Z) { LAB_Vec4Z_Sub(box.a, vec), LAB_Vec4Z_Add(box.b, vec) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4Z LAB_Box4Z_Mul(size_t factor, LAB_Box4Z box) { return (LAB_Box4Z) { LAB_Vec4Z_Mul(factor, box.a), LAB_Vec4Z_Mul(factor, box.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4Z_Contains_Inc(LAB_Box4Z box, LAB_Vec4Z v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y && box.a.z <= v.z && v.z <= box.b.z && box.a.w <= v.w && v.w <= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4Z_IsEmpty(LAB_Box4Z box) { return box.a.x >= box.b.x || box.a.y >= box.b.y || box.a.z >= box.b.z || box.a.w >= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4Z LAB_Box4Z_Intersection(LAB_Box4Z a, LAB_Box4Z b) { return (LAB_Box4Z) { LAB_Vec4Z_HdMax(a.a, b.a), LAB_Vec4Z_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4Z_Intersects(LAB_Box4Z a, LAB_Box4Z b) { return !LAB_Box4Z_IsEmpty(LAB_Box4Z_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4Z LAB_Box4Z_Size(LAB_Box4Z box) { return LAB_Vec4Z_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4Z LAB_Box4Z_FromOriginAndSize(LAB_Vec4Z origin, LAB_Vec4Z size) { return (LAB_Box4Z) { origin, LAB_Vec4Z_Add(origin, size) }; }
LAB_INLINE LAB_ALWAYS_INLINE size_t (*LAB_Box4Z_AsMDArray(LAB_Box4Z* a))[4] { return (size_t (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE const size_t (*LAB_Box4Z_AsCMDArray(const LAB_Box4Z* a))[4] { return (const size_t (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_Vec4Z* LAB_Box4Z_AsArray(LAB_Box4Z* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE const LAB_Vec4Z* LAB_Box4Z_AsCArray(const LAB_Box4Z* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4Z_Unpack(size_t* pax, size_t* pay, size_t* paz, size_t* paw, size_t* pbx, size_t* pby, size_t* pbz, size_t* pbw, LAB_Box4Z a) { *pax = a.a.x; *pay = a.a.y; *paz = a.a.z; *paw = a.a.w; *pbx = a.b.x; *pby = a.b.y; *pbz = a.b.z; *pbw = a.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4Z_Unpack_Sized(size_t* px, size_t* py, size_t* pz, size_t* pw, size_t* pdx, size_t* pdy, size_t* pdz, size_t* pdw, LAB_Box4Z a) { *px = a.a.x; *py = a.a.y; *pz = a.a.z; *pw = a.a.w; *pdx = a.b.x-a.a.x; *pdy = a.b.y-a.a.y; *pdz = a.b.z-a.a.z; *pdw = a.b.w-a.a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4Z LAB_Box4Z_New_Sized(size_t x, size_t y, size_t z, size_t w, size_t dx, size_t dy, size_t dz, size_t dw) { return (LAB_Box4Z) { { x, y, z, w }, { x + dx, y + dy, z + dz, w + dw } }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Box4Z_DX(LAB_Box4Z a) { return a.b.x - a.a.x; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Box4Z_DY(LAB_Box4Z a) { return a.b.y - a.a.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Box4Z_DZ(LAB_Box4Z a) { return a.b.z - a.a.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Box4Z_DW(LAB_Box4Z a) { return a.b.w - a.a.w; }

typedef struct LAB_Box4F { LAB_Vec4F a, b; } LAB_Box4F;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4F_Add(LAB_Box4F a, LAB_Vec4F b) { return (LAB_Box4F) { LAB_Vec4F_Add(a.a, b), LAB_Vec4F_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4F_Expand(LAB_Box4F box, LAB_Vec4F vec) { return (LAB_Box4F) { LAB_Vec4F_Sub(box.a, vec), LAB_Vec4F_Add(box.b, vec) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4F_Mul(float factor, LAB_Box4F box) { return (LAB_Box4F) { LAB_Vec4F_Mul(factor, box.a), LAB_Vec4F_Mul(factor, box.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4F_Contains_Inc(LAB_Box4F box, LAB_Vec4F v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y && box.a.z <= v.z && v.z <= box.b.z && box.a.w <= v.w && v.w <= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4F_IsEmpty(LAB_Box4F box) { return box.a.x >= box.b.x || box.a.y >= box.b.y || box.a.z >= box.b.z || box.a.w >= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4F_Intersection(LAB_Box4F a, LAB_Box4F b) { return (LAB_Box4F) { LAB_Vec4F_HdMax(a.a, b.a), LAB_Vec4F_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4F_Intersects(LAB_Box4F a, LAB_Box4F b) { return !LAB_Box4F_IsEmpty(LAB_Box4F_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Box4F_Size(LAB_Box4F box) { return LAB_Vec4F_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4F LAB_Box4F_MapPoint(LAB_Box4F box, LAB_Vec4F v) { return (LAB_Vec4F) { box.a.x + (box.b.x - box.a.x) * v.x, box.a.y + (box.b.y - box.a.y) * v.y, box.a.z + (box.b.z - box.a.z) * v.z, box.a.w + (box.b.w - box.a.w) * v.w }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4F_Map(LAB_Box4F box, LAB_Box4F fraction) { return (LAB_Box4F) { LAB_Box4F_MapPoint(box, fraction.a), LAB_Box4F_MapPoint(box, fraction.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4F_FromOriginAndSize(LAB_Vec4F origin, LAB_Vec4F size) { return (LAB_Box4F) { origin, LAB_Vec4F_Add(origin, size) }; }
LAB_INLINE LAB_ALWAYS_INLINE float (*LAB_Box4F_AsMDArray(LAB_Box4F* a))[4] { return (float (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE const float (*LAB_Box4F_AsCMDArray(const LAB_Box4F* a))[4] { return (const float (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_Vec4F* LAB_Box4F_AsArray(LAB_Box4F* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE const LAB_Vec4F* LAB_Box4F_AsCArray(const LAB_Box4F* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4F_Unpack(float* pax, float* pay, float* paz, float* paw, float* pbx, float* pby, float* pbz, float* pbw, LAB_Box4F a) { *pax = a.a.x; *pay = a.a.y; *paz = a.a.z; *paw = a.a.w; *pbx = a.b.x; *pby = a.b.y; *pbz = a.b.z; *pbw = a.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4F_Unpack_Sized(float* px, float* py, float* pz, float* pw, float* pdx, float* pdy, float* pdz, float* pdw, LAB_Box4F a) { *px = a.a.x; *py = a.a.y; *pz = a.a.z; *pw = a.a.w; *pdx = a.b.x-a.a.x; *pdy = a.b.y-a.a.y; *pdz = a.b.z-a.a.z; *pdw = a.b.w-a.a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4F_New_Sized(float x, float y, float z, float w, float dx, float dy, float dz, float dw) { return (LAB_Box4F) { { x, y, z, w }, { x + dx, y + dy, z + dz, w + dw } }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Box4F_DX(LAB_Box4F a) { return a.b.x - a.a.x; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Box4F_DY(LAB_Box4F a) { return a.b.y - a.a.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Box4F_DZ(LAB_Box4F a) { return a.b.z - a.a.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Box4F_DW(LAB_Box4F a) { return a.b.w - a.a.w; }

typedef struct LAB_Box4D { LAB_Vec4D a, b; } LAB_Box4D;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4D_Add(LAB_Box4D a, LAB_Vec4D b) { return (LAB_Box4D) { LAB_Vec4D_Add(a.a, b), LAB_Vec4D_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4D_Expand(LAB_Box4D box, LAB_Vec4D vec) { return (LAB_Box4D) { LAB_Vec4D_Sub(box.a, vec), LAB_Vec4D_Add(box.b, vec) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4D_Mul(double factor, LAB_Box4D box) { return (LAB_Box4D) { LAB_Vec4D_Mul(factor, box.a), LAB_Vec4D_Mul(factor, box.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4D_Contains_Inc(LAB_Box4D box, LAB_Vec4D v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y && box.a.z <= v.z && v.z <= box.b.z && box.a.w <= v.w && v.w <= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4D_IsEmpty(LAB_Box4D box) { return box.a.x >= box.b.x || box.a.y >= box.b.y || box.a.z >= box.b.z || box.a.w >= box.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4D_Intersection(LAB_Box4D a, LAB_Box4D b) { return (LAB_Box4D) { LAB_Vec4D_HdMax(a.a, b.a), LAB_Vec4D_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box4D_Intersects(LAB_Box4D a, LAB_Box4D b) { return !LAB_Box4D_IsEmpty(LAB_Box4D_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Box4D_Size(LAB_Box4D box) { return LAB_Vec4D_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec4D LAB_Box4D_MapPoint(LAB_Box4D box, LAB_Vec4D v) { return (LAB_Vec4D) { box.a.x + (box.b.x - box.a.x) * v.x, box.a.y + (box.b.y - box.a.y) * v.y, box.a.z + (box.b.z - box.a.z) * v.z, box.a.w + (box.b.w - box.a.w) * v.w }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4D_Map(LAB_Box4D box, LAB_Box4D fraction) { return (LAB_Box4D) { LAB_Box4D_MapPoint(box, fraction.a), LAB_Box4D_MapPoint(box, fraction.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4D_FromOriginAndSize(LAB_Vec4D origin, LAB_Vec4D size) { return (LAB_Box4D) { origin, LAB_Vec4D_Add(origin, size) }; }
LAB_INLINE LAB_ALWAYS_INLINE double (*LAB_Box4D_AsMDArray(LAB_Box4D* a))[4] { return (double (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE const double (*LAB_Box4D_AsCMDArray(const LAB_Box4D* a))[4] { return (const double (*)[4])(&a->a.x); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_Vec4D* LAB_Box4D_AsArray(LAB_Box4D* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE const LAB_Vec4D* LAB_Box4D_AsCArray(const LAB_Box4D* a) { return &a->a; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4D_Unpack(double* pax, double* pay, double* paz, double* paw, double* pbx, double* pby, double* pbz, double* pbw, LAB_Box4D a) { *pax = a.a.x; *pay = a.a.y; *paz = a.a.z; *paw = a.a.w; *pbx = a.b.x; *pby = a.b.y; *pbz = a.b.z; *pbw = a.b.w; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Box4D_Unpack_Sized(double* px, double* py, double* pz, double* pw, double* pdx, double* pdy, double* pdz, double* pdw, LAB_Box4D a) { *px = a.a.x; *py = a.a.y; *pz = a.a.z; *pw = a.a.w; *pdx = a.b.x-a.a.x; *pdy = a.b.y-a.a.y; *pdz = a.b.z-a.a.z; *pdw = a.b.w-a.a.w; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4D_New_Sized(double x, double y, double z, double w, double dx, double dy, double dz, double dw) { return (LAB_Box4D) { { x, y, z, w }, { x + dx, y + dy, z + dz, w + dw } }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Box4D_DX(LAB_Box4D a) { return a.b.x - a.a.x; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Box4D_DY(LAB_Box4D a) { return a.b.y - a.a.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Box4D_DZ(LAB_Box4D a) { return a.b.z - a.a.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Box4D_DW(LAB_Box4D a) { return a.b.w - a.a.w; }

LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4I2F(LAB_Box4I a) { return (LAB_Box4F) { LAB_Vec4I2F(a.a), LAB_Vec4I2F(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4I2D(LAB_Box4I a) { return (LAB_Box4D) { LAB_Vec4I2D(a.a), LAB_Vec4I2D(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4F2D(LAB_Box4F a) { return (LAB_Box4D) { LAB_Vec4F2D(a.a), LAB_Vec4F2D(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4D2F_Cast(LAB_Box4D a) { return (LAB_Box4F) { LAB_Vec4D2F_Cast(a.a), LAB_Vec4D2F_Cast(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4F2I_FastFloor(LAB_Box4F a) { return (LAB_Box4I) { LAB_Vec4F2I_FastFloor(a.a), LAB_Vec4F2I_FastFloor(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4D2I_FastFloor(LAB_Box4D a) { return (LAB_Box4I) { LAB_Vec4D2I_FastFloor(a.a), LAB_Vec4D2I_FastFloor(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4Z LAB_Box4I2Z(LAB_Box4I a) { return (LAB_Box4Z) { LAB_Vec4I2Z(a.a), LAB_Vec4I2Z(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4I LAB_Box4Z2I(LAB_Box4Z a) { return (LAB_Box4I) { LAB_Vec4Z2I(a.a), LAB_Vec4Z2I(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4F LAB_Box4Z2F(LAB_Box4Z a) { return (LAB_Box4F) { LAB_Vec4Z2F(a.a), LAB_Vec4Z2F(a.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box4D LAB_Box4Z2D(LAB_Box4Z a) { return (LAB_Box4D) { LAB_Vec4Z2D(a.a), LAB_Vec4Z2D(a.b) }; }
