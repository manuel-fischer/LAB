#pragma once

/**
 *  LAB_ASSUME(cond)
 *  - check that has no side effects -> might be used for hints to the compiler
 *
 *  LAB_ASSUME_0(cond)
 *  - check that calls extern functions -> removed completely in release
**/
#include "LAB_attr.h"
#include "LAB_opt.h"
#include <stdbool.h>
#include <stdarg.h>

#include <signal.h>

//#define LAB_DBG_SET_BREAKPOINT

#ifdef LAB_DBG_SET_BREAKPOINT
void LAB_DbgBreak(void);
#define LAB_DBG_BREAK LAB_DbgBreak
#else
#ifdef _POSIX
#define LAB_DBG_BREAK() ((void)raise(SIGTRAP), (void)0)
#else
#ifdef __GNUC__
#define LAB_DBG_BREAK() ((void)__builtin_trap(), (void)0)
#else
#define LAB_DBG_BREAK() ((void)raise(SIGBREAK), (void)0)
#endif
#endif
#endif


#ifdef NDEBUG
#  define LAB_DBG_PRINTF(...) ((void)0)
#  define LAB_FUNCTION() NULL
#  ifdef __GNUC__
     /* hint to compiler */
//#    define LAB_ASSUME2(type, cond, cond_str) do { if (!(cond)) LAB_UNREACHABLE(); } while(0)
#    define LAB_ASSUME2(type, cond, cond_str) ((!(cond)) ? LAB_UNREACHABLE() : (void)0)
#  else
#    define LAB_ASSUME2(type, cond, cond_str) (void)(0)
#  endif
#  define LAB_ASSUME2_FMT(type, cond, cond_str, ...) LAB_ASSUME2(type, cond, cond_str)
#  define LAB_ASSUME_0(cond) (void)(0)
#  define LAB_INIT_DBG(...)
#  define LAB_ASSERT_OR_ABORT(cond) \
    ((!(cond)) ? abort() : (void)0)
#  define LAB_ASSERT_OR_WARN(cond) (void)(0)

#define LAB_ASSERT_LONG(cond) (void)(0)
#else
#  include <stdio.h> // -> fprintf
#  define LAB_DBG_PRINTF(...) LAB_DbgPrintf(__VA_ARGS__)
#  ifdef __GNUC__
#    define LAB_FUNCTION() __builtin_FUNCTION()
#  else
#    define LAB_FUNCTION() "??"
#  endif
/*#  define LAB_ASSUME2(type, cond, cond_str) do { \
    if(!(cond)) \
        LAB_AssumptionFailed(type, cond_str, __FILE__, __LINE__, LAB_FUNCTION()); \
    } while(0) */
#  define LAB_ASSUME2(type, cond, cond_str) \
    ((!(cond)) ? (LAB_AssumptionFailed(type, cond_str, __FILE__, __LINE__, LAB_FUNCTION(), 0), LAB_DBG_BREAK()) : (void)0)
#  define LAB_ASSUME2_FMT(type, cond, cond_str, ...) \
    ((!(cond)) ? (LAB_AssumptionFailedFmt(type, cond_str, __FILE__, __LINE__, LAB_FUNCTION(), 0, __VA_ARGS__), LAB_DBG_BREAK()) : (void)0)
#  define LAB_ASSUME_0 LAB_ASSUME // no parameter aliasing -> keep text replacement behavior
#  define LAB_INIT_DBG(...) __VA_ARGS__

#  define LAB_ASSERT_OR_ABORT LAB_ASSERT
#  define LAB_ASSERT_OR_WARN(cond) \
    ((!(cond)) ? LAB_AssumptionFailed("warned assertion", #cond, __FILE__, __LINE__, LAB_FUNCTION(), 0) : (void)0)

#define LAB_ASSERT_LONG(cond) LAB_ASSUME2("assertion", cond, #cond)
#endif


// TODO remove ASSUME
#define LAB_ASSERT(cond) LAB_ASSUME2("assertion", cond, #cond)
#define LAB_ASSERT_EQ(a, b) LAB_ASSUME2("assertion", (a) == (b), #a " == " #b)
// LAB_ASSERT_MSG(x < y, "%i < %i", x, y)
#define LAB_ASSERT_FMT(cond, ...) LAB_ASSUME2_FMT("assertion", cond, #cond, __VA_ARGS__)
#define LAB_ASSUME(cond) LAB_ASSUME2("assumption", cond, #cond)
#define LAB_ASSERT_FALSE(msg) (LAB_ASSUME2("assertion", false, msg), LAB_UNREACHABLE())
#define LAB_PRECONDITION(cond) LAB_ASSUME2("precondition", cond, #cond)
#define LAB_POSTCONDITION(cond) LAB_ASSUME2("postcondition", cond, #cond)

#define LAB_READABLE(ptr) ((ptr) != NULL)
#define LAB_WRITABLE(ptr) ((ptr) != NULL)
// readable (and writable if not const)
#define LAB_PROPER(ptr) ((ptr) != NULL)


/**
 *  Assume a precondition for expr
 */
#ifndef __GNUC__
#  define LAB_CORRECT_IF(cond, expr) (expr)
#else
/* #  define LAB_CORRECT_IF(cond, expr) ({ \
    LAB_PRECONDITION(cond); \
    (expr); \
}) */
#  define LAB_CORRECT_IF(cond, expr) ( \
    LAB_PRECONDITION(cond), \
    (expr) \
)
#endif

void LAB_AssumptionFailed(const char* type,
                          const char* expr,
                          const char* file,
                          int line,
                          const char* function,
                          int trap);

void LAB_AssumptionFailedFmt(const char* type,
                             const char* expr,
                             const char* file,
                             int line,
                             const char* function,
                             int trap,
                             const char* format,
                             ...);

void LAB_DbgInitOrAbort(void);
void LAB_DbgExit(void);
void LAB_DbgVPrintf(const char* fmt, va_list args);
void LAB_DbgPrintf(const char* fmt, ...);
bool LAB_DbgAtHalt(void(*handler)(void* user), void* user);
void LAB_DbgRemoveHalt(void(*handler)(void* user), void* user);


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
