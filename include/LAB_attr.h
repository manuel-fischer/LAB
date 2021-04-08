#pragma once
/** \file LAB_attr.h
 *
 *  Function attributes
 *
 *  \see LAB_opt.h
 */

#ifndef DOXYGEN

#if defined PROFILE || !defined NDEBUG
#define LAB_AVOID_INLINING
#endif

#if !defined LAB_AVOID_INLINING && !defined __GNUC__
#  define LAB_STATIC static
#  define LAB_INLINE static inline
#elif !defined LAB_AVOID_INLINING && defined __GNUC__
#  define LAB_STATIC static
#  define LAB_INLINE static __attribute__((always_inline)) inline
#else
#  define LAB_STATIC
#  define LAB_INLINE __attribute__((always_inline)) inline
#endif

#else

/**
 *  Used only for functions.
 *  There might be a problem with the profiler when static functions
 *  are used. When some static function is called, the function might be
 *  messed up with another completely independent function, probably
 *  because the name of the static function is stripped from the executable.
 *  To fix this, the static keyword needs to be removed (conditionally)
 *  from all static functions, which exposes the name to the link time
 *  namespace, but this is not a problem here, because all functions have
 *  an unique name.
 */
#define LAB_STATIC static
/**
 *  Like \ref LAB_STATIC, but the function declaration can appear multiple times (in a header file)
 */
#define LAB_INLINE static inline

#endif
