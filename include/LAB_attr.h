#pragma once

#if !defined PROFILE && !defined __GNUC__
/** LAB_STATIC **
 *
 * Used only for functions,
 * there might be a problem with the profiler when static functions
 * are used. when some static function is called, the function might be
 * messed up with another completely independent function, probably
 * because the name of the static function is stripped from the executable
 * to fix this, the static keyword needs to be removed (conditionally)
 * from all static functions, which exposes the name to the link time
 * namespace, but this is not a problem here, because all functions have
 * an unique name
 */
#  define LAB_STATIC static
#  define LAB_INLINE static inline
#elif !defined PROFILE && defined __GNUC__
#  define LAB_STATIC static
#  define LAB_INLINE static __attribute__((always_inline)) inline
#else
#  define LAB_STATIC
#  define LAB_INLINE __attribute__((always_inline)) inline
#endif
