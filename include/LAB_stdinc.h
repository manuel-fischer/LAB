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
#include <stdint.h>  // uint32_t, size_t
#include <stdlib.h>  // malloc, calloc, realloc, free
#include <string.h>  // strlen
#include <stdbool.h> // bool, true, false
#include <time.h>    // clock_gettime
