#pragma once
#include "LAB_stdinc.h"

#ifndef LAB_CUSTOM_ALLOC
#define LAB_Malloc(size)          malloc(size)
#define LAB_Realloc(memory, size) realloc(memory, size)
#define LAB_Calloc(count, size)   calloc(count, size)
#define LAB_Free(memory)          free(memory)
#endif


void* LAB_ReallocN(void* memory, size_t count, size_t size);
