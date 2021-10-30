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


/**
 *  Extract the filename of a path.
 *
 *  \return Pointer to the first char of the filename
 */
LAB_PURE
const char* LAB_Filename(const char* path);


#define LAB_MAX(a, b) ((a)>(b) ? (a) : (b))
#define LAB_MIN(a, b) ((a)<(b) ? (a) : (b))

#define LAB_MAX_EQ(a, b) ((a)>(b) ? (a) : ((a)=(b)))
#define LAB_MIN_EQ(a, b) ((a)<(b) ? (a) : ((a)=(b)))

#define LAB_MIN3(a, b, c) (  (a)<(b) ? ( (a)<(c) ? (a) : (c) ) : ( (b)<(c) ? (b) : (c) )  )
#define LAB_MAX3(a, b, c) (  (a)>(b) ? ( (a)>(c) ? (a) : (c) ) : ( (b)>(c) ? (b) : (c) )  )

#define LAB_CLAMP(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (a))


LAB_PURE LAB_INLINE
float LAB_fMix(float a, float b, float mult)
{
    return a + (b-a)*mult;
}

LAB_PURE LAB_INLINE
float LAB_fSmoothMin(float a, float b, float k)
{
    float mult = 0.5f + 0.5f*(a-b)/k;
    mult = LAB_CLAMP(mult, 0.f, 1.f);
    return LAB_fMix(a, b, mult) - k*mult*(1.f-mult);
}

#define LAB_fSmoothMax(a, b, c) (-LAB_fSmoothMin(-(a), -(b), c))


#define LAB_SELECT_MAX(a,va, b,vb) ((a)>(b) ? (va) : (vb))
#define LAB_SELECT_MIN(a,va, b,vb) ((a)<(b) ? (va) : (vb))

#define LAB_SELECT_MIN3(a,va, b,vb, c,vc) (  (a)<(b) ? ( (a)<(c) ? (va) : (vc) ) : ( (b)<(c) ? (vb) : (vc) )  )
#define LAB_SELECT_MAX3(a,va, b,vb, c,vc) (  (a)>(b) ? ( (a)>(c) ? (va) : (vc) ) : ( (b)>(c) ? (vb) : (vc) )  )

#define LAB_PTR_OFFSET(ptr, index, size) ((void*)((char*)(ptr) + (index)*(size)))


#define LAB_BITS_EACH(bits, lval_bit_index, ...) do \
{ \
    for(int LAB_BITS_EACH_bits = (bits); LAB_BITS_EACH_bits; LAB_BITS_EACH_bits&=LAB_BITS_EACH_bits-1) \
    { \
        (lval_bit_index) = LAB_Ctz(LAB_BITS_EACH_bits); \
        {__VA_ARGS__} \
    } \
} while(0)


LAB_PURE LAB_ALWAYS_INLINE LAB_INLINE
bool LAB_MultOverflow(size_t a, size_t b)
{
#ifdef __GNUC__
    size_t unused_result;
    return __builtin_mul_overflow(a, b, &unused_result);
#else
    return (a*b)/b < a;
#endif
}


typedef uint64_t LAB_Nanos;

LAB_Nanos LAB_NanoSeconds();


LAB_PURE
size_t LAB_StrHash(const char* str);



#define LAB_ObjCopy(dstp, srcp) LAB_CORRECT_IF( \
    sizeof(*(dstp)) == sizeof(*(srcp)), \
    memcpy(dstp, srcp, sizeof(*(dstp))))



/*LAB_PURE*/ LAB_ALWAYS_INLINE LAB_UNUSED LAB_INLINE 
bool LAB_MakeTrue(void) { return true; }


#define LAB_ObjClear(dstp) (memset(dstp, 0, sizeof(*(dstp))), LAB_MakeTrue())


#include <math.h>

#define LAB_EPSILON (1e-5)
#define LAB_APPROX_EQ(a, b) (fabs((a) - (b)) < LAB_EPSILON)