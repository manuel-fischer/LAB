#pragma once

#define LAB_INLINE   __attribute__((always_inline)) inline
#define LAB_NOINLINE __attribute__((noinline))
#define LAB_HOT      __attribute__((hot))

#define LAB_ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)

#define LAB_ALIGNED

#define LAB_UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#define LAB_LIKELY(cond) __builtin_expect(!!(cond), 1)


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

