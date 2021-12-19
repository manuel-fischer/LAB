#pragma once
/** \file LAB_stdinc.h
 *
 *  Include standard library headers
 *
 */
#if !defined _POSIX_C_SOURCE || _POSIX_C_SOURCE < 200101L
#  error _POSIX_C_SOURCE should be defined as at least 200101L
#endif
#include <stddef.h>  // NULL
#include <stdint.h>  // uint32_t, size_t, SIZE_MAX
#include <stdlib.h>  // malloc, calloc, realloc, free
#include <string.h>  // strlen
#include <stdbool.h> // bool, true, false
#include <time.h>    // clock_gettime


#ifdef __STDC_NO_ATOMICS__
#error Atomics should be available to compile LAB
#endif
#include <stdatomic.h>

#if !ATOMIC_POINTER_LOCK_FREE
//#warning Atomic pointers should be lock free
#endif