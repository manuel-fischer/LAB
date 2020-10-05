#pragma once

#define LAB_IN
#define LAB_OUT
#define LAB_INOUT

#define LAB_PP_(a, b) a##b
#define LAB_PP(a, b) LAB_PP_(a, b)
#define LAB_UNQ(name) LAB_PP(name, __LINE__)

#define LAB_PAD(type) type : sizeof(type)*8

#define LAB_DEPRECATED(msg) __attribute__((deprecated))

#ifdef __GNUC__

#define LAB_UNREACHABLE() __builtin_unreachable()

// function attributes
#define LAB_INLINE   __attribute__((always_inline)) inline
#define LAB_NOINLINE __attribute__((noinline))
#define LAB_HOT      __attribute__((hot))

#define LAB_PURE     __attribute__((pure))
#define LAB_CONST    __attribute__((const))

//#define LAB_UNROLL(n) _Pragma("GCC unroll " #n)
#define LAB_PRAGMA(str) _Pragma(#str)
#define LAB_UNROLL(n) LAB_PRAGMA(GCC unroll n)


/*#define LAB_PAD(type) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wuninitialized\"") \
    type LAB_UNQ(pad); \
    _Pragma("GCC diagnostic pop")*/

#define LAB_ALIGNED

#define LAB_UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#define LAB_LIKELY(cond) __builtin_expect(!!(cond), 1)

#else

#ifndef __attribute__
#define __attribute__(tuple)
#endif

#define LAB_UNREACHABLE() ((void)0)

#define LAB_INLINE
#define LAB_NOINLINE
#define LAB_HOT

#define LAB_PURE
#define LAB_CONST

#define LAB_UNROLL(n)


//#define LAB_PAD(type) type LAB_UNQ(pad)


#define LAB_ALIGNED

#define LAB_UNLIKELY(cond) !!(cond)
#define LAB_LIKELY(cond)   !!(cond)

#endif





#if 0

//#define NO_ASM_OPTS

#if !defined NO_ASM_OPTS && defined __GNUC__ && defined __x86_64__
/*
#define LAB_CMOV(cond, dst, src)            \
    asm (                                   \
         "test %3, %3\n\t"                  \
         "cmovne %2, %0"                    \
         : "=r"(dst) : "0"(dst), "r"(src), "r"(cond)  \
    )
#define LAB_CMOV_NOT(cond, dst, src)            \
    asm (                                   \
         "test %3, %3\n\t"                  \
         "cmove %2, %0"                     \
         : "=r"(dst) : "0"(dst), "r"(src), "r"(cond)  \
    )
*/


#define LAB_CMOV(cond, dst, src)            \
    asm (                                   \
         "test %2, %2\n\t"                  \
         "cmovne %1, %0"                    \
         : "+r"(dst) : "r"(src), "g"(cond) : "cc"  \
    )
#define LAB_CMOV_NOT(cond, dst, src)            \
    asm (                                   \
         "test %2, %2\n\t"                  \
         "cmove %1, %0"                     \
         : "+r"(dst) : "r"(src), "g"(cond) : "cc"  \
    )


#define LAB_CMOV_FROM(cond, dst, src)            \
    asm (                                   \
         "test %2, %2\n\t"                  \
         "cmovne (%1), %0"                    \
         : "+r"(dst) : "r"(src), "g"(cond) : "cc"  \
    )
#define LAB_CMOV_FROM_NOT(cond, dst, src)            \
    asm (                                   \
         "test %2, %2\n\t"                  \
         "cmove (%1), %0"                     \
         : "+r"(dst) : "r"(src), "g"(cond) : "cc"  \
    )
#else
#define LAB_CMOV(cond, dst, src)            \
    do { if(cond) (dst) = (src); } while(0)
#define LAB_CMOV_NOT(cond, dst, src)            \
    do { if(!cond) (dst) = (src); } while(0)

#define LAB_CMOV_FROM(cond, dst, src)            \
    do { if(cond) (dst) = *(src); } while(0)
#define LAB_CMOV_FROM_NOT(cond, dst, src)            \
    do { if(!cond) (dst) = *(src); } while(0)
#endif

#endif
