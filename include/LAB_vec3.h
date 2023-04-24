#pragma once

#include "LAB_math.h"
#include "LAB_util.h"
#include "LAB_direction.h"
#include "LAB_bits.h"

#define LAB_V_MIN_INDEX3(x, y, z) LAB_SELECT_MIN3(x,0, y,1, z,2)
#define LAB_V_MAX_INDEX3(x, y, z) LAB_SELECT_MAX3(x,0, y,1, z,2)

#define LAB_VEC3_FROM(vtype, ptr) ((vtype) { (ptr)->x, (ptr)->y, (ptr)->z })


typedef struct LAB_Vec3I { int x, y, z; } LAB_Vec3I;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Add(LAB_Vec3I a, LAB_Vec3I b) { return (LAB_Vec3I) { ((a.x) + (b.x)), ((a.y) + (b.y)), ((a.z) + (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Sub(LAB_Vec3I a, LAB_Vec3I b) { return (LAB_Vec3I) { ((a.x) - (b.x)), ((a.y) - (b.y)), ((a.z) - (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Neg(LAB_Vec3I a) { return (LAB_Vec3I) { (-(a.x)), (-(a.y)), (-(a.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec3I_Dot(LAB_Vec3I a, LAB_Vec3I b) { return ((((((a.x) * (b.x))) + (((a.y) * (b.y))))) + (((a.z) * (b.z)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec3I_LengthSq(LAB_Vec3I a) { return ((((((a.x) * (a.x))) + (((a.y) * (a.y))))) + (((a.z) * (a.z)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec3I_DistanceSq(LAB_Vec3I a, LAB_Vec3I b) { return LAB_Vec3I_LengthSq(LAB_Vec3I_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Mul(int a, LAB_Vec3I b) { return (LAB_Vec3I) { ((a) * (b.x)), ((a) * (b.y)), ((a) * (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_RMul(LAB_Vec3I a, int b) { return (LAB_Vec3I) { ((a.x) * (b)), ((a.y) * (b)), ((a.z) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Div(LAB_Vec3I a, int b) { return (LAB_Vec3I) { ((a.x) / (b)), ((a.y) / (b)), ((a.z) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_HdMul(LAB_Vec3I a, LAB_Vec3I b) { return (LAB_Vec3I) { ((a.x) * (b.x)), ((a.y) * (b.y)), ((a.z) * (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_HdDiv(LAB_Vec3I a, LAB_Vec3I b) { return (LAB_Vec3I) { ((a.x) / (b.x)), ((a.y) / (b.y)), ((a.z) / (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_HdMin(LAB_Vec3I a, LAB_Vec3I b) { return (LAB_Vec3I) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y), LAB_MIN(a.z, b.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_HdMax(LAB_Vec3I a, LAB_Vec3I b) { return (LAB_Vec3I) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y), LAB_MAX(a.z, b.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec3I_GetMin(LAB_Vec3I a) { return LAB_MIN3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec3I_GetMax(LAB_Vec3I a) { return LAB_MAX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec3I_MinIndex(LAB_Vec3I a) { return LAB_V_MIN_INDEX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec3I_MaxIndex(LAB_Vec3I a) { return LAB_V_MAX_INDEX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Abs(LAB_Vec3I a) { return (LAB_Vec3I) { abs(a.x), abs(a.y), abs(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_FromDirIndex(LAB_DirIndex i) { return (LAB_Vec3I) { LAB_OX(i), LAB_OY(i), LAB_OZ(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Dir(LAB_DirIndex i) { return (LAB_Vec3I) { LAB_OX(i), LAB_OY(i), LAB_OZ(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Dir_OrthA(LAB_DirIndex i) { return (LAB_Vec3I) { LAB_AXF(i), LAB_AYF(i), LAB_AZF(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Dir_OrthB(LAB_DirIndex i) { return (LAB_Vec3I) { LAB_BXF(i), LAB_BYF(i), LAB_BZF(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_AddDir(LAB_Vec3I a, LAB_DirIndex i) { return (LAB_Vec3I) { ((a.x) + (LAB_OX(i))), ((a.y) + (LAB_OY(i))), ((a.z) + (LAB_OZ(i))) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_SubDir(LAB_Vec3I a, LAB_DirIndex i) { return (LAB_Vec3I) { ((a.x) - (LAB_OX(i))), ((a.y) - (LAB_OY(i))), ((a.z) - (LAB_OZ(i))) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec3I LAB_Vec3I_FromArray(const int* array) { return (LAB_Vec3I) { array[0], array[1], array[2] }; }
LAB_INLINE LAB_ALWAYS_INLINE int* LAB_Vec3I_AsArray(LAB_Vec3I* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const int* LAB_Vec3I_AsCArray(const LAB_Vec3I* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE int* LAB_Vec3I_ToArray(int* array, LAB_Vec3I a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec3I_Unpack(int* px, int* py, int* pz, LAB_Vec3I a) { *px = a.x; *py = a.y; *pz = a.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int LAB_Vec3I_Get(LAB_Vec3I a, size_t i) { return LAB_Vec3I_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST int* LAB_Vec3I_Ref(LAB_Vec3I* a, size_t i) { return &LAB_Vec3I_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const int* LAB_Vec3I_CRef(const LAB_Vec3I* a, size_t i) { return &LAB_Vec3I_AsCArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Sar(LAB_Vec3I a, int b) { return (LAB_Vec3I) { LAB_Sar(a.x, b), LAB_Sar(a.y, b), LAB_Sar(a.z, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Shr(LAB_Vec3I a, int b) { return (LAB_Vec3I) { ((a.x) >> (b)), ((a.y) >> (b)), ((a.z) >> (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3I_Shl(LAB_Vec3I a, int b) { return (LAB_Vec3I) { ((a.x) << (b)), ((a.y) << (b)), ((a.z) << (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Vec3I_Equals(LAB_Vec3I a, LAB_Vec3I b) { return ((((((a.x) == (b.x))) && (((a.y) == (b.y))))) && (((a.z) == (b.z)))); }

typedef struct LAB_Vec3F { float x, y, z; } LAB_Vec3F;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Add(LAB_Vec3F a, LAB_Vec3F b) { return (LAB_Vec3F) { ((a.x) + (b.x)), ((a.y) + (b.y)), ((a.z) + (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Sub(LAB_Vec3F a, LAB_Vec3F b) { return (LAB_Vec3F) { ((a.x) - (b.x)), ((a.y) - (b.y)), ((a.z) - (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Neg(LAB_Vec3F a) { return (LAB_Vec3F) { (-(a.x)), (-(a.y)), (-(a.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec3F_Dot(LAB_Vec3F a, LAB_Vec3F b) { return ((((((a.x) * (b.x))) + (((a.y) * (b.y))))) + (((a.z) * (b.z)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec3F_LengthSq(LAB_Vec3F a) { return ((((((a.x) * (a.x))) + (((a.y) * (a.y))))) + (((a.z) * (a.z)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec3F_DistanceSq(LAB_Vec3F a, LAB_Vec3F b) { return LAB_Vec3F_LengthSq(LAB_Vec3F_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Mul(float a, LAB_Vec3F b) { return (LAB_Vec3F) { ((a) * (b.x)), ((a) * (b.y)), ((a) * (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_RMul(LAB_Vec3F a, float b) { return (LAB_Vec3F) { ((a.x) * (b)), ((a.y) * (b)), ((a.z) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Div(LAB_Vec3F a, float b) { return (LAB_Vec3F) { ((a.x) / (b)), ((a.y) / (b)), ((a.z) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_HdMul(LAB_Vec3F a, LAB_Vec3F b) { return (LAB_Vec3F) { ((a.x) * (b.x)), ((a.y) * (b.y)), ((a.z) * (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_HdDiv(LAB_Vec3F a, LAB_Vec3F b) { return (LAB_Vec3F) { ((a.x) / (b.x)), ((a.y) / (b.y)), ((a.z) / (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_HdMin(LAB_Vec3F a, LAB_Vec3F b) { return (LAB_Vec3F) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y), LAB_MIN(a.z, b.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_HdMax(LAB_Vec3F a, LAB_Vec3F b) { return (LAB_Vec3F) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y), LAB_MAX(a.z, b.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec3F_GetMin(LAB_Vec3F a) { return LAB_MIN3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec3F_GetMax(LAB_Vec3F a) { return LAB_MAX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec3F_MinIndex(LAB_Vec3F a) { return LAB_V_MIN_INDEX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec3F_MaxIndex(LAB_Vec3F a) { return LAB_V_MAX_INDEX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Abs(LAB_Vec3F a) { return (LAB_Vec3F) { fabsf(a.x), fabsf(a.y), fabsf(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_FromDirIndex(LAB_DirIndex i) { return (LAB_Vec3F) { LAB_OX(i), LAB_OY(i), LAB_OZ(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Dir(LAB_DirIndex i) { return (LAB_Vec3F) { LAB_OX(i), LAB_OY(i), LAB_OZ(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Dir_OrthA(LAB_DirIndex i) { return (LAB_Vec3F) { LAB_AXF(i), LAB_AYF(i), LAB_AZF(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_Dir_OrthB(LAB_DirIndex i) { return (LAB_Vec3F) { LAB_BXF(i), LAB_BYF(i), LAB_BZF(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_AddDir(LAB_Vec3F a, LAB_DirIndex i) { return (LAB_Vec3F) { ((a.x) + (LAB_OX(i))), ((a.y) + (LAB_OY(i))), ((a.z) + (LAB_OZ(i))) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3F_SubDir(LAB_Vec3F a, LAB_DirIndex i) { return (LAB_Vec3F) { ((a.x) - (LAB_OX(i))), ((a.y) - (LAB_OY(i))), ((a.z) - (LAB_OZ(i))) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec3F LAB_Vec3F_FromArray(const float* array) { return (LAB_Vec3F) { array[0], array[1], array[2] }; }
LAB_INLINE LAB_ALWAYS_INLINE float* LAB_Vec3F_AsArray(LAB_Vec3F* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const float* LAB_Vec3F_AsCArray(const LAB_Vec3F* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE float* LAB_Vec3F_ToArray(float* array, LAB_Vec3F a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec3F_Unpack(float* px, float* py, float* pz, LAB_Vec3F a) { *px = a.x; *py = a.y; *pz = a.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float LAB_Vec3F_Get(LAB_Vec3F a, size_t i) { return LAB_Vec3F_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST float* LAB_Vec3F_Ref(LAB_Vec3F* a, size_t i) { return &LAB_Vec3F_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const float* LAB_Vec3F_CRef(const LAB_Vec3F* a, size_t i) { return &LAB_Vec3F_AsCArray(a)[i]; }

typedef struct LAB_Vec3D { double x, y, z; } LAB_Vec3D;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Add(LAB_Vec3D a, LAB_Vec3D b) { return (LAB_Vec3D) { ((a.x) + (b.x)), ((a.y) + (b.y)), ((a.z) + (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Sub(LAB_Vec3D a, LAB_Vec3D b) { return (LAB_Vec3D) { ((a.x) - (b.x)), ((a.y) - (b.y)), ((a.z) - (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Neg(LAB_Vec3D a) { return (LAB_Vec3D) { (-(a.x)), (-(a.y)), (-(a.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec3D_Dot(LAB_Vec3D a, LAB_Vec3D b) { return ((((((a.x) * (b.x))) + (((a.y) * (b.y))))) + (((a.z) * (b.z)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec3D_LengthSq(LAB_Vec3D a) { return ((((((a.x) * (a.x))) + (((a.y) * (a.y))))) + (((a.z) * (a.z)))); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec3D_DistanceSq(LAB_Vec3D a, LAB_Vec3D b) { return LAB_Vec3D_LengthSq(LAB_Vec3D_Sub(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Mul(double a, LAB_Vec3D b) { return (LAB_Vec3D) { ((a) * (b.x)), ((a) * (b.y)), ((a) * (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_RMul(LAB_Vec3D a, double b) { return (LAB_Vec3D) { ((a.x) * (b)), ((a.y) * (b)), ((a.z) * (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Div(LAB_Vec3D a, double b) { return (LAB_Vec3D) { ((a.x) / (b)), ((a.y) / (b)), ((a.z) / (b)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_HdMul(LAB_Vec3D a, LAB_Vec3D b) { return (LAB_Vec3D) { ((a.x) * (b.x)), ((a.y) * (b.y)), ((a.z) * (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_HdDiv(LAB_Vec3D a, LAB_Vec3D b) { return (LAB_Vec3D) { ((a.x) / (b.x)), ((a.y) / (b.y)), ((a.z) / (b.z)) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_HdMin(LAB_Vec3D a, LAB_Vec3D b) { return (LAB_Vec3D) { LAB_MIN(a.x, b.x), LAB_MIN(a.y, b.y), LAB_MIN(a.z, b.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_HdMax(LAB_Vec3D a, LAB_Vec3D b) { return (LAB_Vec3D) { LAB_MAX(a.x, b.x), LAB_MAX(a.y, b.y), LAB_MAX(a.z, b.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec3D_GetMin(LAB_Vec3D a) { return LAB_MIN3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec3D_GetMax(LAB_Vec3D a) { return LAB_MAX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec3D_MinIndex(LAB_Vec3D a) { return LAB_V_MIN_INDEX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST size_t LAB_Vec3D_MaxIndex(LAB_Vec3D a) { return LAB_V_MAX_INDEX3(a.x, a.y, a.z); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Abs(LAB_Vec3D a) { return (LAB_Vec3D) { fabs(a.x), fabs(a.y), fabs(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_FromDirIndex(LAB_DirIndex i) { return (LAB_Vec3D) { LAB_OX(i), LAB_OY(i), LAB_OZ(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Dir(LAB_DirIndex i) { return (LAB_Vec3D) { LAB_OX(i), LAB_OY(i), LAB_OZ(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Dir_OrthA(LAB_DirIndex i) { return (LAB_Vec3D) { LAB_AXF(i), LAB_AYF(i), LAB_AZF(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_Dir_OrthB(LAB_DirIndex i) { return (LAB_Vec3D) { LAB_BXF(i), LAB_BYF(i), LAB_BZF(i) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_AddDir(LAB_Vec3D a, LAB_DirIndex i) { return (LAB_Vec3D) { ((a.x) + (LAB_OX(i))), ((a.y) + (LAB_OY(i))), ((a.z) + (LAB_OZ(i))) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3D_SubDir(LAB_Vec3D a, LAB_DirIndex i) { return (LAB_Vec3D) { ((a.x) - (LAB_OX(i))), ((a.y) - (LAB_OY(i))), ((a.z) - (LAB_OZ(i))) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_PURE LAB_Vec3D LAB_Vec3D_FromArray(const double* array) { return (LAB_Vec3D) { array[0], array[1], array[2] }; }
LAB_INLINE LAB_ALWAYS_INLINE double* LAB_Vec3D_AsArray(LAB_Vec3D* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE const double* LAB_Vec3D_AsCArray(const LAB_Vec3D* a) { return &a->x; }
LAB_INLINE LAB_ALWAYS_INLINE double* LAB_Vec3D_ToArray(double* array, LAB_Vec3D a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; return array; }
LAB_INLINE LAB_ALWAYS_INLINE void LAB_Vec3D_Unpack(double* px, double* py, double* pz, LAB_Vec3D a) { *px = a.x; *py = a.y; *pz = a.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double LAB_Vec3D_Get(LAB_Vec3D a, size_t i) { return LAB_Vec3D_AsArray(&a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST double* LAB_Vec3D_Ref(LAB_Vec3D* a, size_t i) { return &LAB_Vec3D_AsArray(a)[i]; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST const double* LAB_Vec3D_CRef(const LAB_Vec3D* a, size_t i) { return &LAB_Vec3D_AsCArray(a)[i]; }

LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3I2F(LAB_Vec3I a) { return (LAB_Vec3F) { (float)(a.x), (float)(a.y), (float)(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3I2D(LAB_Vec3I a) { return (LAB_Vec3D) { (double)(a.x), (double)(a.y), (double)(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Vec3F2D(LAB_Vec3F a) { return (LAB_Vec3D) { (double)(a.x), (double)(a.y), (double)(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Vec3D2F_Cast(LAB_Vec3D a) { return (LAB_Vec3F) { (float)(a.x), (float)(a.y), (float)(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3F2I_FastFloor(LAB_Vec3F a) { return (LAB_Vec3I) { LAB_FastFloorF2I(a.x), LAB_FastFloorF2I(a.y), LAB_FastFloorF2I(a.z) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Vec3D2I_FastFloor(LAB_Vec3D a) { return (LAB_Vec3I) { LAB_FastFloorD2I(a.x), LAB_FastFloorD2I(a.y), LAB_FastFloorD2I(a.z) }; }

typedef struct LAB_Box3I { LAB_Vec3I a, b; } LAB_Box3I;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3I LAB_Box3I_Add(LAB_Box3I a, LAB_Vec3I b) { return (LAB_Box3I) { LAB_Vec3I_Add(a.a, b), LAB_Vec3I_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3I_Contains_Inc(LAB_Box3I box, LAB_Vec3I v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y && box.a.z <= v.z && v.z <= box.b.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3I_IsEmpty(LAB_Box3I box) { return box.a.x >= box.b.x || box.a.y >= box.b.y || box.a.z >= box.b.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3I LAB_Box3I_Intersection(LAB_Box3I a, LAB_Box3I b) { return (LAB_Box3I) { LAB_Vec3I_HdMax(a.a, b.a), LAB_Vec3I_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3I_Intersects(LAB_Box3I a, LAB_Box3I b) { return !LAB_Box3I_IsEmpty(LAB_Box3I_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3I LAB_Box3I_Size(LAB_Box3I box) { return LAB_Vec3I_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3I LAB_Box3I_FromOriginAndSize(LAB_Vec3I origin, LAB_Vec3I size) { return (LAB_Box3I) { origin, LAB_Vec3I_Add(origin, size) }; }

typedef struct LAB_Box3F { LAB_Vec3F a, b; } LAB_Box3F;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3F LAB_Box3F_Add(LAB_Box3F a, LAB_Vec3F b) { return (LAB_Box3F) { LAB_Vec3F_Add(a.a, b), LAB_Vec3F_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3F_Contains_Inc(LAB_Box3F box, LAB_Vec3F v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y && box.a.z <= v.z && v.z <= box.b.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3F_IsEmpty(LAB_Box3F box) { return box.a.x >= box.b.x || box.a.y >= box.b.y || box.a.z >= box.b.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3F LAB_Box3F_Intersection(LAB_Box3F a, LAB_Box3F b) { return (LAB_Box3F) { LAB_Vec3F_HdMax(a.a, b.a), LAB_Vec3F_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3F_Intersects(LAB_Box3F a, LAB_Box3F b) { return !LAB_Box3F_IsEmpty(LAB_Box3F_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3F LAB_Box3F_Size(LAB_Box3F box) { return LAB_Vec3F_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3F LAB_Box3F_FromOriginAndSize(LAB_Vec3F origin, LAB_Vec3F size) { return (LAB_Box3F) { origin, LAB_Vec3F_Add(origin, size) }; }

typedef struct LAB_Box3D { LAB_Vec3D a, b; } LAB_Box3D;
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3D LAB_Box3D_Add(LAB_Box3D a, LAB_Vec3D b) { return (LAB_Box3D) { LAB_Vec3D_Add(a.a, b), LAB_Vec3D_Add(a.b, b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3D_Contains_Inc(LAB_Box3D box, LAB_Vec3D v) { return box.a.x <= v.x && v.x <= box.b.x && box.a.y <= v.y && v.y <= box.b.y && box.a.z <= v.z && v.z <= box.b.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3D_IsEmpty(LAB_Box3D box) { return box.a.x >= box.b.x || box.a.y >= box.b.y || box.a.z >= box.b.z; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3D LAB_Box3D_Intersection(LAB_Box3D a, LAB_Box3D b) { return (LAB_Box3D) { LAB_Vec3D_HdMax(a.a, b.a), LAB_Vec3D_HdMin(a.b, b.b) }; }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST bool LAB_Box3D_Intersects(LAB_Box3D a, LAB_Box3D b) { return !LAB_Box3D_IsEmpty(LAB_Box3D_Intersection(a, b)); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Vec3D LAB_Box3D_Size(LAB_Box3D box) { return LAB_Vec3D_Sub(box.b, box.a); }
LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST LAB_Box3D LAB_Box3D_FromOriginAndSize(LAB_Vec3D origin, LAB_Vec3D size) { return (LAB_Box3D) { origin, LAB_Vec3D_Add(origin, size) }; }

