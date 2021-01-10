#pragma once
/** \file LAB_util.h
 *
 *  Common utility funcions
 */

#include <stdint.h> // uint64_t
#include "LAB_opt.h"


/**
 *  Extract the filename of a path.
 *
 *  \return Pointer to the first char of the filename
 */
LAB_PURE
const char* LAB_Filename(const char* path);


#define LAB_MAX(a, b) ((a)>(b) ? (a) : (b))
#define LAB_MIN(a, b) ((a)<(b) ? (a) : (b))

#define LAB_MAX3(a, b, c) (  (a)>(b) ? ( (a)>(c) ? (a) : (c) ) : ( (b)>(c) ? (b) : (c) )  )

#define LAB_PTR_OFFSET(ptr, index, size) ((void*)((char*)(ptr) + (index)*(size)))

uint64_t LAB_NanoSeconds();


LAB_PURE
size_t LAB_StrHash(const char* str);
