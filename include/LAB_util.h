#pragma once
/** \file LAB_util.h
 *
 *  Common utility funcions
 */

#include <stdint.h> // uint64_t
#include <stddef.h> // size_t
#include <stdbool.h> // bool
#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_debug.h"
#include "LAB_bits.h"


/**
 *  Extract the filename of a path.
 *
 *  \return Pointer to the first char of the filename
 */
LAB_POINTER_CONST
const char* LAB_Filename(const char* path);


#define LAB_MAX(a, b) ((a)>(b) ? (a) : (b))
#define LAB_MIN(a, b) ((a)<(b) ? (a) : (b))

#define LAB_MAX_EQ(a, b) ((a)>(b) ? (a) : ((a)=(b)))
#define LAB_MIN_EQ(a, b) ((a)<(b) ? (a) : ((a)=(b)))

#define LAB_MIN3(a, b, c) (  (a)<(b) ? ( (a)<(c) ? (a) : (c) ) : ( (b)<(c) ? (b) : (c) )  )
#define LAB_MAX3(a, b, c) (  (a)>(b) ? ( (a)>(c) ? (a) : (c) ) : ( (b)>(c) ? (b) : (c) )  )

#define LAB_CLAMP(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define LAB_CLAMP_EQ(x, a, b) ((x) < (a) ? ((x)=(a)) : (x) > (b) ? ((x)=(b)) : (x))


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


#define LAB_SELECT_MAX(a,va, b,vb) ((a)>(b) ? (va) : (vb))
#define LAB_SELECT_MIN(a,va, b,vb) ((a)<(b) ? (va) : (vb))

#define LAB_SELECT_MIN3(a,va, b,vb, c,vc) (  (a)<(b) ? ( (a)<(c) ? (va) : (vc) ) : ( (b)<(c) ? (vb) : (vc) )  )
#define LAB_SELECT_MAX3(a,va, b,vb, c,vc) (  (a)>(b) ? ( (a)>(c) ? (va) : (vc) ) : ( (b)>(c) ? (vb) : (vc) )  )

#define LAB_PTR_OFFSET(ptr, index, size) ((void*)((char*)(ptr) + (index)*(size)))
#define LAB_CPTR_OFFSET(ptr, index, size) ((const void*)((const char*)(ptr) + (index)*(size)))


#define LAB_BITS_EACH(bits, lval_bit_index, ...) do \
{ \
    for(int LAB_BITS_EACH_bits = (bits); LAB_BITS_EACH_bits; LAB_BITS_EACH_bits&=LAB_BITS_EACH_bits-1) \
    { \
        (lval_bit_index) = LAB_Ctz(LAB_BITS_EACH_bits); \
        {__VA_ARGS__} \
    } \
} while(0)


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

#define LAB_NANOS_MAX 0xffffffffffffffff
typedef uint64_t LAB_Nanos;

LAB_Nanos LAB_NanoSeconds();


LAB_POINTER_CONST
size_t LAB_StrHash(const char* str);


typedef struct LAB_FileContents
{
    char* data;
    size_t size;
    bool success;
    #define LAB_FileContents_array(file_contents) (char, (file_contents)->data, (file_contents)->size)
} LAB_FileContents;

LAB_FileContents LAB_ReadFile(const char* filename, const char* mode);



#define LAB_ObjCopy(dstp, srcp) LAB_CORRECT_IF( \
    sizeof(*(dstp)) == sizeof(*(srcp)), \
    memcpy(dstp, srcp, sizeof(*(dstp))))



/*LAB_PURE*/ LAB_ALWAYS_INLINE LAB_UNUSED LAB_INLINE 
bool LAB_MakeTrue(void) { return true; }


#define LAB_ObjClear(dstp) (memset(dstp, 0, sizeof(*(dstp))), LAB_MakeTrue())
#define LAB_ArrayClear(dstp) (memset(dstp, 0, sizeof(dstp)), LAB_MakeTrue())


#include <math.h>

#define LAB_EPSILON (1e-5)
#define LAB_APPROX_EQ(a, b) (fabs((a) - (b)) < LAB_EPSILON)

// Branchless conditionals, only use with integral types

// LAB_SELECT(cond, a, b) cond ? a : b, but branchless
// GCC: switches between cmov* and imul,
// imul is pretty optimized in current cpus, it is not as a problem, compared
// to a pipeline flush
//#define LAB_SELECT(cond, a, b) ((b) + (!!(cond))*((a)-(b)))
#define LAB_SELECT(cond, a, b) ((b) ^ (!!(cond))*((a)^(b)))
//#define LAB_SELECT(cond, a, b) ((!!(cond))*(a) | (!(cond))*(b))
//#define LAB_SELECT(cond, b, a) ((a) ^ (-!!(cond) & ((a) ^ (b))))
//#define LAB_SELECT(cond, b, a) ((-!(cond) & (a)) | (-!!(cond) & (b)))
//#define LAB_SELECT(cond, b, a) ((a) - (-!!(cond) & ((a) + (b))))

#define LAB_SELECT0(cond, a) ((!!(cond))*(a))

#define LAB_COND_MOVE(cond, dst, src) ((dst) = LAB_SELECT(cond, src, dst))

// Branchless min/max, only use with integers
#define LAB_MAX_BL(a, b) LAB_SELECT((a) > (b), a, b)
#define LAB_MIN_BL(a, b) LAB_SELECT((a) < (b), a, b)

#define LAB_MAX3_BL(a, b, c) LAB_MAX_BL(LAB_MAX_BL(a, b), c)
#define LAB_MIN3_BL(a, b, c) LAB_MIN_BL(LAB_MIN_BL(a, b), c)

#define LAB_CLAMP_BL(x, a, b) ((a) ^ LAB_SELECT0((x) >= (a), (x)^(a)) ^ LAB_SELECT0((b) <= (x), (b)^(x)))

#if 0
#undef LAB_MAX
#undef LAB_MIN
#undef LAB_MAX_EQ
#undef LAB_MIN_EQ
#define LAB_MAX LAB_MAX_BL
#define LAB_MIN LAB_MIN_BL
#define LAB_MAX_EQ(a, b) ((a) = LAB_MAX_BL(a, b))
#define LAB_MIN_EQ(a, b) ((a) = LAB_MIN_BL(a, b))
#endif

#define LAB_BOUND0(x) LAB_SELECT0((x) >= 0, x)



#define LAB_COND_SWAP_T(type, cond, a, b) do { \
    type LAB_COND_SWAP_delta = LAB_SELECT0(cond, (a)^(b)); \
    (a) ^= LAB_COND_SWAP_delta; \
    (b) ^= LAB_COND_SWAP_delta; \
} while(0)


#define LAB_LEN(array) (sizeof(array) / sizeof(array[0]))



#define LAB_REDUCE_3(func, a, b, c) ( \
    func(func(a, b), c) \
)

#define LAB_REDUCE_4(func, a, b, c, d) ( \
    func(func(a, b), func(c, d)) \
)

#define LAB_REDUCE_5(func, a, b, c, d, e) ( \
    func(func(func(a, b), func(c, d)), e) \
)



#define LAB_LREDUCE_3(func, a, b, c) ( \
    func(func(a, b), c) \
)

#define LAB_LREDUCE_4(func, a, b, c, d) ( \
    func(func(func(a, b), c), d) \
)

#define LAB_LREDUCE_5(func, a, b, c, d, e) ( \
    func(func(func(func(a, b), c), d), e) \
)



#define LAB_RREDUCE_3(func, a, b, c) ( \
    func(a, func(b, c)) \
)

#define LAB_RREDUCE_4(func, a, b, c, d) ( \
    func(a, func(b, func(c, d))) \
)

#define LAB_RREDUCE_5(func, a, b, c, d, e) ( \
    func(a, func(b, func(c, func(d, e)))) \
)




#define LAB_FOREACH(element_type, iterator, prefix) \
    for(element_type* LAB_FOREACH_BEGIN = (prefix), \
                    * LAB_FOREACH_END = LAB_FOREACH_BEGIN + (prefix ## _count), \
                    * iterator = LAB_FOREACH_BEGIN; \
        iterator != LAB_FOREACH_END; ++iterator)

#define LAB_FOREACH_INDEX(element_type, index_name, iterator, prefix) \
    LAB_FOREACH(element_type, iterator, prefix) \
    for(size_t LAB_FOREACH_TMP=1, index_name = iterator-LAB_FOREACH_BEGIN;LAB_FOREACH_TMP;LAB_FOREACH_TMP=0)

#define LAB_CFOREACH(element_type, iterator, prefix) \
    LAB_FOREACH(element_type const, iterator, prefix)

#define LAB_CFOREACH_INDEX(element_type, index_name, iterator, prefix) \
    LAB_FOREACH_INDEX(element_type const, index_name, iterator, prefix)

