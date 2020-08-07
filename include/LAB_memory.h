#pragma once
#include "LAB_stdinc.h"

#define LAB_DEBUG_ALLOC

#ifndef LAB_CUSTOM_ALLOC
#ifndef LAB_DEBUG_ALLOC
#define LAB_Malloc(size)          malloc(size)
#define LAB_Realloc(memory, size) realloc(memory, size)
#define LAB_Calloc(count, size)   calloc(count, size)
#define LAB_Free(memory)          free(memory)
#define LAB_ReallocN(memory, count, size) LAB_RealReallocN(memory, count, size)

#define LAB_RealRealloc LAB_Realloc
#else
#define LAB_Malloc(size)          LAB_DbgMalloc(size, __FILE__, __LINE__)
#define LAB_Realloc(memory, size) LAB_DbgRealloc(memory, size, __FILE__, __LINE__)
#define LAB_Calloc(count, size)   LAB_DbgCalloc(count, size, __FILE__, __LINE__)
#define LAB_Free(memory)          LAB_DbgFree(memory, __FILE__, __LINE__)
#define LAB_ReallocN(memory, count, size) LAB_DbgReallocN(memory, count, size, __FILE__, __LINE__)

#define LAB_RealRealloc realloc
#endif
#endif

void* LAB_RealReallocN(void* memory, size_t count, size_t size);


void* LAB_DbgMalloc(size_t size, const char* file, int line);
void* LAB_DbgRealloc(void* memory, size_t size, const char* file, int line);
void* LAB_DbgCalloc(size_t count, size_t size, const char* file, int line);
void  LAB_DbgFree(void* memory, const char* file, int line);
void* LAB_DbgReallocN(void* memory, size_t count, size_t size, const char* file, int line);

void  LAB_DbgMemShow(void);
