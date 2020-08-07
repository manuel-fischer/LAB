#pragma once

#define LAB_IN
#define LAB_OUT
#define LAB_INOUT

#ifdef __GNUC__

#define LAB_INLINE   __attribute__((always_inline)) inline
#define LAB_NOINLINE __attribute__((noinline))
#define LAB_HOT      __attribute__((hot))

#define LAB_PURE     __attribute__((pure))
#define LAB_CONST    __attribute__((const))



#define LAB_ALIGNED

#define LAB_ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)
#define LAB_UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#define LAB_LIKELY(cond) __builtin_expect(!!(cond), 1)

#else

#ifndef __attribute__
#define __attribute__(tuple)
#endif

#define LAB_INLINE
#define LAB_NOINLINE
#define LAB_HOT

#define LAB_PURE
#define LAB_CONST

#define LAB_ALIGNED

#define LAB_ASSUME(cond) (void)(0)
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
