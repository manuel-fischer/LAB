#pragma once

#include "LAB_math.h"
#include "LAB_util.h"
#include "LAB_direction.h"
#include "LAB_bits.h"

#define LAB_V_MIN_INDEX2(x, y) LAB_SELECT_MIN(x,0, y,1)
#define LAB_V_MAX_INDEX2(x, y) LAB_SELECT_MAX(x,0, y,1)

#define LAB_VEC2_FROM(vtype, ptr) ((vtype) { (ptr)->x, (ptr)->y })


typedef struct LAB_Vec2I { int x, y; } LAB_Vec2I;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Add(LAB_Vec2I a, LAB_Vec2I b) { return (LAB_Vec2I) { ((a.x) + (b.x)), ((a.y) + (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Sub(LAB_Vec2I a, LAB_Vec2I b) { return (LAB_Vec2I) { ((a.x) - (b.x)), ((a.y) - (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Neg(LAB_Vec2I a) { return (LAB_Vec2I) { (-(a.x)), (-(a.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec2I_Dot(LAB_Vec2I a, LAB_Vec2I b) { return ((((a.x) * (b.x))) + (((a.y) * (b.y)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec2I_LengthSq(LAB_Vec2I a) { return ((((a.x) * (a.x))) + (((a.y) * (a.y)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec2I_DistanceSq(LAB_Vec2I a, LAB_Vec2I b) { return LAB_Vec2I_LengthSq(LAB_Vec2I_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Mul(int a, LAB_Vec2I b) { return (LAB_Vec2I) { ((a) * (b.x)), ((a) * (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_RMul(LAB_Vec2I a, int b) { return (LAB_Vec2I) { ((a.x) * (b)), ((a.y) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Div(LAB_Vec2I a, int b) { return (LAB_Vec2I) { ((a.x) / (b)), ((a.y) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_HdMul(LAB_Vec2I a, LAB_Vec2I b) { return (LAB_Vec2I) { ((a.x) * (b.x)), ((a.y) * (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_HdDiv(LAB_Vec2I a, LAB_Vec2I b) { return (LAB_Vec2I) { ((a.x) / (b.x)), ((a.y) / (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_HdMin(LAB_Vec2I a, LAB_Vec2I b) { return (LAB_Vec2I) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_HdMax(LAB_Vec2I a, LAB_Vec2I b) { return (LAB_Vec2I) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec2I_GetMin(LAB_Vec2I a) { return LAB_MIN(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec2I_GetMax(LAB_Vec2I a) { return LAB_MAX(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec2I_MinIndex(LAB_Vec2I a) { return LAB_V_MIN_INDEX2(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec2I_MaxIndex(LAB_Vec2I a) { return LAB_V_MAX_INDEX2(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Abs(LAB_Vec2I a) { return (LAB_Vec2I) { abs(a.x), abs(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec2I LAB_Vec2I_FromArray(const int* array) { return (LAB_Vec2I) { array[0], array[1] }; }
LAB_INLINE LAB_ALWAYS_INLINE int* LAB_Vec2I_AsArray(LAB_Vec2I* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const int* LAB_Vec2I_AsCArray(const LAB_Vec2I* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE int* LAB_Vec2I_ToArray(int* array, LAB_Vec2I a) { array[0] = a.x; array[1] = a.y; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec2I_Unpack(int* px, int* py, LAB_Vec2I a) { *px = a.x; *py = a.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec2I_Get(LAB_Vec2I a, size_t i) { return LAB_Vec2I_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int* LAB_Vec2I_Ref(LAB_Vec2I* a, size_t i) { return &LAB_Vec2I_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const int* LAB_Vec2I_CRef(const LAB_Vec2I* a, size_t i) { return &LAB_Vec2I_AsCArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Sar(LAB_Vec2I a, int b) { return (LAB_Vec2I) { LAB_Sar(a.x, b), LAB_Sar(a.y, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Shr(LAB_Vec2I a, int b) { return (LAB_Vec2I) { ((a.x) >> (b)), ((a.y) >> (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2I_Shl(LAB_Vec2I a, int b) { return (LAB_Vec2I) { ((a.x) << (b)), ((a.y) << (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Vec2I_Equals(LAB_Vec2I a, LAB_Vec2I b) { return ((((a.x) == (b.x))) && (((a.y) == (b.y)))); }

typedef struct LAB_Vec2F { float x, y; } LAB_Vec2F;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_Add(LAB_Vec2F a, LAB_Vec2F b) { return (LAB_Vec2F) { ((a.x) + (b.x)), ((a.y) + (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_Sub(LAB_Vec2F a, LAB_Vec2F b) { return (LAB_Vec2F) { ((a.x) - (b.x)), ((a.y) - (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_Neg(LAB_Vec2F a) { return (LAB_Vec2F) { (-(a.x)), (-(a.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec2F_Dot(LAB_Vec2F a, LAB_Vec2F b) { return ((((a.x) * (b.x))) + (((a.y) * (b.y)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec2F_LengthSq(LAB_Vec2F a) { return ((((a.x) * (a.x))) + (((a.y) * (a.y)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec2F_DistanceSq(LAB_Vec2F a, LAB_Vec2F b) { return LAB_Vec2F_LengthSq(LAB_Vec2F_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_Mul(float a, LAB_Vec2F b) { return (LAB_Vec2F) { ((a) * (b.x)), ((a) * (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_RMul(LAB_Vec2F a, float b) { return (LAB_Vec2F) { ((a.x) * (b)), ((a.y) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_Div(LAB_Vec2F a, float b) { return (LAB_Vec2F) { ((a.x) / (b)), ((a.y) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_HdMul(LAB_Vec2F a, LAB_Vec2F b) { return (LAB_Vec2F) { ((a.x) * (b.x)), ((a.y) * (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_HdDiv(LAB_Vec2F a, LAB_Vec2F b) { return (LAB_Vec2F) { ((a.x) / (b.x)), ((a.y) / (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_HdMin(LAB_Vec2F a, LAB_Vec2F b) { return (LAB_Vec2F) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_HdMax(LAB_Vec2F a, LAB_Vec2F b) { return (LAB_Vec2F) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec2F_GetMin(LAB_Vec2F a) { return LAB_MIN(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec2F_GetMax(LAB_Vec2F a) { return LAB_MAX(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec2F_MinIndex(LAB_Vec2F a) { return LAB_V_MIN_INDEX2(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec2F_MaxIndex(LAB_Vec2F a) { return LAB_V_MAX_INDEX2(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2F_Abs(LAB_Vec2F a) { return (LAB_Vec2F) { fabsf(a.x), fabsf(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec2F LAB_Vec2F_FromArray(const float* array) { return (LAB_Vec2F) { array[0], array[1] }; }
LAB_INLINE LAB_ALWAYS_INLINE float* LAB_Vec2F_AsArray(LAB_Vec2F* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const float* LAB_Vec2F_AsCArray(const LAB_Vec2F* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE float* LAB_Vec2F_ToArray(float* array, LAB_Vec2F a) { array[0] = a.x; array[1] = a.y; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec2F_Unpack(float* px, float* py, LAB_Vec2F a) { *px = a.x; *py = a.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec2F_Get(LAB_Vec2F a, size_t i) { return LAB_Vec2F_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float* LAB_Vec2F_Ref(LAB_Vec2F* a, size_t i) { return &LAB_Vec2F_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const float* LAB_Vec2F_CRef(const LAB_Vec2F* a, size_t i) { return &LAB_Vec2F_AsCArray(a)[i]; }

typedef struct LAB_Vec2D { double x, y; } LAB_Vec2D;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_Add(LAB_Vec2D a, LAB_Vec2D b) { return (LAB_Vec2D) { ((a.x) + (b.x)), ((a.y) + (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_Sub(LAB_Vec2D a, LAB_Vec2D b) { return (LAB_Vec2D) { ((a.x) - (b.x)), ((a.y) - (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_Neg(LAB_Vec2D a) { return (LAB_Vec2D) { (-(a.x)), (-(a.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec2D_Dot(LAB_Vec2D a, LAB_Vec2D b) { return ((((a.x) * (b.x))) + (((a.y) * (b.y)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec2D_LengthSq(LAB_Vec2D a) { return ((((a.x) * (a.x))) + (((a.y) * (a.y)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec2D_DistanceSq(LAB_Vec2D a, LAB_Vec2D b) { return LAB_Vec2D_LengthSq(LAB_Vec2D_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_Mul(double a, LAB_Vec2D b) { return (LAB_Vec2D) { ((a) * (b.x)), ((a) * (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_RMul(LAB_Vec2D a, double b) { return (LAB_Vec2D) { ((a.x) * (b)), ((a.y) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_Div(LAB_Vec2D a, double b) { return (LAB_Vec2D) { ((a.x) / (b)), ((a.y) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_HdMul(LAB_Vec2D a, LAB_Vec2D b) { return (LAB_Vec2D) { ((a.x) * (b.x)), ((a.y) * (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_HdDiv(LAB_Vec2D a, LAB_Vec2D b) { return (LAB_Vec2D) { ((a.x) / (b.x)), ((a.y) / (b.y)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_HdMin(LAB_Vec2D a, LAB_Vec2D b) { return (LAB_Vec2D) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_HdMax(LAB_Vec2D a, LAB_Vec2D b) { return (LAB_Vec2D) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec2D_GetMin(LAB_Vec2D a) { return LAB_MIN(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec2D_GetMax(LAB_Vec2D a) { return LAB_MAX(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec2D_MinIndex(LAB_Vec2D a) { return LAB_V_MIN_INDEX2(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec2D_MaxIndex(LAB_Vec2D a) { return LAB_V_MAX_INDEX2(a.x, a.y); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2D_Abs(LAB_Vec2D a) { return (LAB_Vec2D) { fabs(a.x), fabs(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec2D LAB_Vec2D_FromArray(const double* array) { return (LAB_Vec2D) { array[0], array[1] }; }
LAB_INLINE LAB_ALWAYS_INLINE double* LAB_Vec2D_AsArray(LAB_Vec2D* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const double* LAB_Vec2D_AsCArray(const LAB_Vec2D* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE double* LAB_Vec2D_ToArray(double* array, LAB_Vec2D a) { array[0] = a.x; array[1] = a.y; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec2D_Unpack(double* px, double* py, LAB_Vec2D a) { *px = a.x; *py = a.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec2D_Get(LAB_Vec2D a, size_t i) { return LAB_Vec2D_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double* LAB_Vec2D_Ref(LAB_Vec2D* a, size_t i) { return &LAB_Vec2D_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const double* LAB_Vec2D_CRef(const LAB_Vec2D* a, size_t i) { return &LAB_Vec2D_AsCArray(a)[i]; }

LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2I2F(LAB_Vec2I a) { return (LAB_Vec2F) { (float)(a.x), (float)(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2I2D(LAB_Vec2I a) { return (LAB_Vec2D) { (double)(a.x), (double)(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Vec2F2D(LAB_Vec2F a) { return (LAB_Vec2D) { (double)(a.x), (double)(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Vec2D2F_Cast(LAB_Vec2D a) { return (LAB_Vec2F) { (float)(a.x), (float)(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2F2I_FastFloor(LAB_Vec2F a) { return (LAB_Vec2I) { LAB_FastFloorF2I(a.x), LAB_FastFloorF2I(a.y) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Vec2D2I_FastFloor(LAB_Vec2D a) { return (LAB_Vec2I) { LAB_FastFloorD2I(a.x), LAB_FastFloorD2I(a.y) }; }

typedef struct LAB_Box2I { LAB_Vec2I a, b; } LAB_Box2I;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2I LAB_Box2I_Add(LAB_Box2I a, LAB_Vec2I b) { return (LAB_Box2I) { LAB_Vec2I_Add(a.a, b), LAB_Vec2I_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2I_Contains_Inc(LAB_Box2I box, LAB_Vec2I v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2I_IsEmpty(LAB_Box2I box) { return box.a.x >= box.b.x || box.a.y >= box.b.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2I LAB_Box2I_Intersection(LAB_Box2I a, LAB_Box2I b) { return (LAB_Box2I) { LAB_Vec2I_HdMax(a.a, b.a), LAB_Vec2I_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2I_Intersects(LAB_Box2I a, LAB_Box2I b) { return !LAB_Box2I_IsEmpty(LAB_Box2I_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2I LAB_Box2I_Size(LAB_Box2I box) { return LAB_Vec2I_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2I LAB_Box2I_FromOriginAndSize(LAB_Vec2I origin, LAB_Vec2I size) { return (LAB_Box2I) { origin, LAB_Vec2I_Add(origin, size) }; }

typedef struct LAB_Box2F { LAB_Vec2F a, b; } LAB_Box2F;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2F LAB_Box2F_Add(LAB_Box2F a, LAB_Vec2F b) { return (LAB_Box2F) { LAB_Vec2F_Add(a.a, b), LAB_Vec2F_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2F_Contains_Inc(LAB_Box2F box, LAB_Vec2F v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2F_IsEmpty(LAB_Box2F box) { return box.a.x >= box.b.x || box.a.y >= box.b.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2F LAB_Box2F_Intersection(LAB_Box2F a, LAB_Box2F b) { return (LAB_Box2F) { LAB_Vec2F_HdMax(a.a, b.a), LAB_Vec2F_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2F_Intersects(LAB_Box2F a, LAB_Box2F b) { return !LAB_Box2F_IsEmpty(LAB_Box2F_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2F LAB_Box2F_Size(LAB_Box2F box) { return LAB_Vec2F_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2F LAB_Box2F_FromOriginAndSize(LAB_Vec2F origin, LAB_Vec2F size) { return (LAB_Box2F) { origin, LAB_Vec2F_Add(origin, size) }; }

typedef struct LAB_Box2D { LAB_Vec2D a, b; } LAB_Box2D;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2D LAB_Box2D_Add(LAB_Box2D a, LAB_Vec2D b) { return (LAB_Box2D) { LAB_Vec2D_Add(a.a, b), LAB_Vec2D_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2D_Contains_Inc(LAB_Box2D box, LAB_Vec2D v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2D_IsEmpty(LAB_Box2D box) { return box.a.x >= box.b.x || box.a.y >= box.b.y; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2D LAB_Box2D_Intersection(LAB_Box2D a, LAB_Box2D b) { return (LAB_Box2D) { LAB_Vec2D_HdMax(a.a, b.a), LAB_Vec2D_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box2D_Intersects(LAB_Box2D a, LAB_Box2D b) { return !LAB_Box2D_IsEmpty(LAB_Box2D_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec2D LAB_Box2D_Size(LAB_Box2D box) { return LAB_Vec2D_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box2D LAB_Box2D_FromOriginAndSize(LAB_Vec2D origin, LAB_Vec2D size) { return (LAB_Box2D) { origin, LAB_Vec2D_Add(origin, size) }; }

