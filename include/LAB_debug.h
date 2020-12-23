#pragma once

/**
 *  LAB_ASSUME(cond)
 *  - check that has no side effects -> might be used for hints to the compiler
 *
 *  LAB_ASSUME_0(cond)
 *  - check that calls extern functions -> removed completely in release
**/
#include "LAB_opt.h"
#ifdef NDEBUG
#  ifdef __GNUC__
     /* hint to compiler */
#    define LAB_ASSUME(cond) do { if (!(cond)) LAB_UNREACHABLE(); } while(0)
#  else
#    define LAB_ASSUME(cond) (void)(0)
#  endif
#  define LAB_ASSUME_0(cond) (void)(0)
#else
#  ifdef __GNUC__
#    define LAB_FUNCTION() __builtin_FUNCTION()
#  else
#    define LAB_FUNCTION() NULL
#  endif
#  define LAB_ASSUME(cond) do { \
    if(!(cond)) \
        LAB_AssumptionFailed(#cond, __FILE__, __LINE__, LAB_FUNCTION()); \
    } while(0)
#  define LAB_ASSUME_0 LAB_ASSUME // no parameter aliasing -> keep text replacement behavior
#endif

void LAB_AssumptionFailed(const char* expr,
                          const char* file,
                          int line,
                          const char* function);




/**
 *  Type should be int, float or double
 */
#define LAB_DEBUG_MINMAX(type, variable) do {               \
    static type dbg_min;                                    \
    static type dbg_max;                                    \
    static bool dbg_initialized = 0;                        \
    type dbg_tmp = (variable);                              \
    if(!dbg_initialized)                                    \
    {                                                       \
        dbg_min = dbg_max = dbg_tmp;                        \
        printf("first encountered value of " #variable ": " \
                LAB_DEBUG_MINMAX_FMT_##type "\n",           \
                dbg_tmp);                                   \
        dbg_initialized = 1;                                \
    }                                                       \
    else                                                    \
    {                                                       \
        if(dbg_tmp < dbg_min)                               \
        {                                                   \
            printf("New minimum for " #variable " found: "  \
                   LAB_DEBUG_MINMAX_FMT_##type "\n",        \
                   dbg_tmp);                                \
            dbg_min = dbg_tmp;                              \
        }                                                   \
        if(dbg_tmp > dbg_max)                               \
        {                                                   \
            printf("New maximum for " #variable " found: "  \
                   LAB_DEBUG_MINMAX_FMT_##type "\n",        \
                   dbg_tmp);                                \
            dbg_max = dbg_tmp;                              \
        }                                                   \
    }                                                       \
} while(0)

#define LAB_DEBUG_MINMAX_FMT_int "%i"
#define LAB_DEBUG_MINMAX_FMT_float "%f"
#define LAB_DEBUG_MINMAX_FMT_double "%f"
