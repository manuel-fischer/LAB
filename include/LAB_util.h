#pragma once
/** \file LAB_util.h
 *
 *  Common utility funcions
 */

#include <stdint.h> // uint64_t
#include <stddef.h> // size_t
#include "LAB_opt.h"


/**
 *  Extract the filename of a path.
 *
 *  \return Pointer to the first char of the filename
 */
LAB_PURE
const char* LAB_Filename(const char* path);


#define LAB_MAX(a, b) ((a)>(b) ? (a) : (b))
#define LAB_MIN(a, b) ((a)<(b) ? (a) : (b))

#define LAB_MIN3(a, b, c) (  (a)<(b) ? ( (a)<(c) ? (a) : (c) ) : ( (b)<(c) ? (b) : (c) )  )
#define LAB_MAX3(a, b, c) (  (a)>(b) ? ( (a)>(c) ? (a) : (c) ) : ( (b)>(c) ? (b) : (c) )  )

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


uint64_t LAB_NanoSeconds();


LAB_PURE
size_t LAB_StrHash(const char* str);
