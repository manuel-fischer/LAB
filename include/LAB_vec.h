#pragma once

#include "LAB_util.h"

// These macros work with arrays, no pointers!

#define LAB_Vec3_Copy(dst, src) do { \
    (dst)[0] = (src)[0]; \
    (dst)[1] = (src)[1]; \
    (dst)[2] = (src)[2]; \
} while(0)

#define LAB_Vec3_Assign(dst, x, y, z) do { \
    (dst)[0] = x; \
    (dst)[1] = y; \
    (dst)[2] = z; \
} while(0)


#define LAB_Vec3_Add(dst, src1, src2) do { \
    (dst)[0] = (src1)[0]+(src2)[0]; \
    (dst)[1] = (src1)[1]+(src2)[1]; \
    (dst)[2] = (src1)[2]+(src2)[2]; \
} while(0)

#define LAB_Vec3_Sub(dst, src1, src2) do { \
    (dst)[0] = (src1)[0]-(src2)[0]; \
    (dst)[1] = (src1)[1]-(src2)[1]; \
    (dst)[2] = (src1)[2]-(src2)[2]; \
} while(0)

#define LAB_Vec3_SclMul(dst, src_vec, src_scalar) do { \
    (dst)[0] = (src_vec)[0]*(src_scalar); \
    (dst)[1] = (src_vec)[1]*(src_scalar); \
    (dst)[2] = (src_vec)[2]*(src_scalar); \
} while(0)

#define LAB_Vec3_SclDiv(dst, src_vec, src_scalar) do { \
    (dst)[0] = (src_vec)[0]/(src_scalar); \
    (dst)[1] = (src_vec)[1]/(src_scalar); \
    (dst)[2] = (src_vec)[2]/(src_scalar); \
} while(0)

#define LAB_Vec3_HadmdMul(dst, src1, src2) do { \
    (dst)[0] = (src1)[0]*(src2)[0]; \
    (dst)[1] = (src1)[1]*(src2)[1]; \
    (dst)[2] = (src1)[2]*(src2)[2]; \
} while(0)

#define LAB_Vec3_HadmdDiv(dst, src1, src2) do { \
    (dst)[0] = (src1)[0]/(src2)[0]; \
    (dst)[1] = (src1)[1]/(src2)[1]; \
    (dst)[2] = (src1)[2]/(src2)[2]; \
} while(0)

#define LAB_Vec3_HadmdMin(dst, src1, src2) do { \
    (dst)[0] = LAB_MIN((src1)[0], (src2)[0]); \
    (dst)[1] = LAB_MIN((src1)[1], (src2)[1]); \
    (dst)[2] = LAB_MIN((src1)[2], (src2)[2]); \
} while(0)

#define LAB_Vec3_HadmdMax(dst, src1, src2) do { \
    (dst)[0] = LAB_MAX((src1)[0], (src2)[0]); \
    (dst)[1] = LAB_MAX((src1)[1], (src2)[1]); \
    (dst)[2] = LAB_MAX((src1)[2], (src2)[2]); \
} while(0)

#define LAB_Vec3_GetMin(src) LAB_MIN3((src)[0], (src)[1], (src)[2])
#define LAB_Vec3_GetMax(src) LAB_MAX3((src)[0], (src)[1], (src)[2])

#define LAB_Vec3_GetMinIndex(src) LAB_SELECT_MIN3((src)[0],0, (src)[1],1, (src)[2],2)
#define LAB_Vec3_GetMaxIndex(src) LAB_SELECT_MAX3((src)[0],0, (src)[1],1, (src)[2],2)
