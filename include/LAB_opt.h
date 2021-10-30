#pragma once
/** \file LAB_opt.h
 *
 *  Various attributes and builtins to allow more optimizations
 *
 *  Some Attributes correspond to the
 *  [GCC Attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html)
 *  with the same name.
 *
 *  \see LAB_attr.h
 */

#ifndef DOXYGEN

#define LAB_IN
#define LAB_OUT
#define LAB_INOUT

#define LAB_NONULL
#define LAB_RESTRICT restrict

#define LAB_PP_(a, b) a##b
#define LAB_PP(a, b) LAB_PP_(a, b)
#define LAB_UNQ(name) LAB_PP(name, __LINE__)

#define LAB_PAD(type) type : sizeof(type)*8

#define LAB_DEPRECATED(msg) __attribute__((deprecated))

#ifdef __GNUC__

#define LAB_UNREACHABLE() __builtin_unreachable()

// function attributes
#define LAB_ALWAYS_INLINE __attribute__((always_inline))
#define LAB_NOINLINE      __attribute__((noinline))
#define LAB_HOT           __attribute__((hot))

#define LAB_PURE          __attribute__((pure))
#define LAB_CONST         __attribute__((const))

// C23: maybe_unused
#define LAB_UNUSED        __attribute__((unused))

//#define LAB_UNROLL(n) _Pragma("GCC unroll " #n)
#define LAB_PRAGMA(str) _Pragma(#str)
#define LAB_UNROLL(n) LAB_PRAGMA(GCC unroll n)


/*#define LAB_PAD(type) \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wuninitialized\"") \
    type LAB_UNQ(pad); \
    _Pragma("GCC diagnostic pop")*/

//#define LAB_ALIGNED

#define LAB_UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#define LAB_LIKELY(cond) __builtin_expect(!!(cond), 1)

#define LAB_IS_CONSTANT(expr)   __builtin_constant_p(expr)

#else

#ifndef __attribute__
#define __attribute__(tuple)
#endif

#define LAB_UNREACHABLE() ((void)0)

#define LAB_ALWAYS_INLINE
#define LAB_NOINLINE
#define LAB_HOT

#define LAB_PURE
#define LAB_CONST

#define LAB_UNUSED

#define LAB_UNROLL(n)


//#define LAB_PAD(type) type LAB_UNQ(pad)


#define LAB_ALIGNED

#define LAB_UNLIKELY(cond) !!(cond)
#define LAB_LIKELY(cond)   !!(cond)

#define LAB_IS_CONSTANT(expr)   0

#endif

#define LAB_EMPTY_TU LAB_UNUSED static char empty_tu = 0




// value evaluated multiple times
#define LAB_PULL_CONST(type, name, max, value) \
    LAB_UNROLL(max) \
    for(type name = 0; name < max; ++name) \
    if(name == (value))




#else /* DOXYGEN SECTION */

/**
 *  Specifies that a reference is read from
 *
 *  \see LAB_OUT LAB_INOUT
 */
#define LAB_IN

/**
 *  Specifies that a reference is written to
 *
 *  \see LAB_IN LAB_INOUT
 */
#define LAB_OUT

/**
 *  Specifies that a reference is read from and written to
 *
 *  \see LAB_IN LAB_OUT
 */
#define LAB_INOUT

/**
 *  Concatenate two expanded tokens
 */
#define LAB_PP(a, b)

/**
 *  Generate a unique id in the namespace name
 */
#define LAB_UNQ(name)

/**
 *  Create a padding field with the size type
 */
#define LAB_PAD(type)

/**
 *  Declare something to be deprecated
 */
#define LAB_DEPRECATED(msg)

/**
 *  Insert an unreachable statement. This hints the
 *  compiler that any code leading to this statement
 *  should never happen. It allows the compiler to
 *  optimize out related code.
 */
#define LAB_UNREACHABLE()

/**
 *  Declare a function to always be inlined
 *
 *  \see LAB_INLINE
 */
#define LAB_ALWAYS_INLINE

/**
 *  Declare a function to never be inlined
 *
 *  \see LAB_ALWAYS_INLINE
 */
#define LAB_NOINLINE

/**
 *  A Function that has a hot code path.
 *  The function might be moved to a hot section
 *  and optimized more aggressively.
 *
 *  \see LAB_ALWAYS_INLINE
 */
#define LAB_HOT

/**
 *  A Function that does not have any effect
 *  on the observable program state other than
 *  the return value.
 *
 *  Such a function might only be called once
 *  and its return value might be used multiple
 *  times, whenever multiple calls to a function
 *  with the same parameters are made. It does
 *  not happen if memory changes between calls.
 *
 *  \see LAB_CONST
 */
#define LAB_PURE

/**
 *  A Function that only depends on the values of
 *  the parameters and on constant memory.
 *  Constant memory is memory that never changes
 *  between multiple calls.
 *  The only observable effect of the function
 *  is the return value.
 *
 *  Such a function might only be called once
 *  and its return value might be used multiple
 *  times, whenever multiple calls to a function
 *  with the same parameters are made.
 *
 *  \see LAB_PURE
 */
#define LAB_CONST

/**
 *  Insert \c _Pragma directive. In difference
 *  to the \c _Pragma directive itself, no quotes
 *  are needed
 */
#define LAB_PRAGMA(str) _Pragma(#str)

/**
 *  Unroll a loop \c n times
 *
 *  It corresponds to the [GCC Pragma](https://gcc.gnu.org/onlinedocs/gcc/Loop-Specific-Pragmas.html)
 *  with the same name.
 */
#define LAB_UNROLL(n)

//#define LAB_ALIGNED

/**
 *  Hint the compiler, that \c cond is expected to be false.
 *
 *  Correspronds to \c __builtin_expect(!!(cond),0) ([GCC Builtin](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html))
 */
#define LAB_UNLIKELY(cond)

/**
 *  Hint the compiler, that \c cond is expected to be true.
 *
 *  Correspronds to \c __builtin_expect(!!(cond),1) ([GCC Builtin](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html))
 */
#define LAB_LIKELY(cond)

/**
 *  Check if \c expr is constant, that is if the compiler is able to constant-fold expressions
 *  involving \c expr.
 *
 *  It might not be 1 if \c expr is constant. It might be used for switching between
 *  more optimized code paths in macros and inline functions
 */
#define LAB_IS_CONSTANT(expr)

/**
 *  Use whenever a translation unit (TU) file does not contain anything, note this expands to a statement
 *  and a semicolon is expected:
 *
 *  ```
 *  // file.c
 *  #include "file.h"
 *  LAB_EMPTY_TU;
 *  ```
 */
#define LAB_EMPTY_TU
#endif
