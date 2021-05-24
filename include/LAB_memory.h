#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_util.h" // -> LAB_MultOverflow

#ifndef NDEBUG
#define LAB_DEBUG_ALLOC
#endif

#ifndef LAB_CUSTOM_ALLOC
#ifndef LAB_DEBUG_ALLOC
#define LAB_Malloc(size)          malloc(size)
#define LAB_Realloc(memory, size) realloc(memory, size)
#define LAB_Calloc(count, size)   calloc(count, size)
#define LAB_Free(memory)          free(memory)
// overflow safe version of LAB_Realloc
#define LAB_MallocN(count, size) ( \
    (LAB_UNLIKELY(LAB_MultOverflow(count, size))) \
        ? NULL \
        : LAB_Malloc((size_t)(count)*(size_t)(size)) \
)

#define LAB_ReallocN(memory, count, size) ( \
    (LAB_UNLIKELY(LAB_MultOverflow(count, size))) \
        ? NULL \
        : LAB_Realloc(memory, (size_t)(count)*(size_t)(size)) \
)

#define LAB_StrDup(str)           LAB_RealStrDup(str)

#else
#define LAB_Malloc(size)          LAB_DbgMalloc(size, __FILE__, __LINE__)
#define LAB_Realloc(memory, size) LAB_DbgRealloc(memory, size, __FILE__, __LINE__)
#define LAB_Calloc(count, size)   LAB_DbgCalloc(count, size, __FILE__, __LINE__)
#define LAB_Free(memory)          LAB_DbgFree(memory, __FILE__, __LINE__)
#define LAB_MallocN(count, size)  LAB_DbgMallocN(count, size, __FILE__, __LINE__)
#define LAB_ReallocN(memory, count, size) LAB_DbgReallocN(memory, count, size, __FILE__, __LINE__)

#define LAB_StrDup(str)           LAB_DbgStrDup(str, __FILE__, __LINE__)

#define LAB_RealMalloc malloc
#define LAB_RealRealloc realloc


void* LAB_RealReallocN(void* memory, size_t count, size_t size);


void* LAB_DbgMalloc(size_t size, const char* file, int line);
void* LAB_DbgRealloc(void* memory, size_t size, const char* file, int line);
void* LAB_DbgCalloc(size_t count, size_t size, const char* file, int line);
void  LAB_DbgFree(void* memory, const char* file, int line);
void* LAB_DbgMallocN(size_t count, size_t size, const char* file, int line);
void* LAB_DbgReallocN(void* memory, size_t count, size_t size, const char* file, int line);

char* LAB_DbgStrDup(const char* str, const char* file, int line);

void  LAB_DbgMemShow(void);

#endif

#if 0
#define LAB_RealStrDup(str) strdup(str)
#else
// Do not use this function directly
static inline char* LAB_RealStrDup(const char* str)
{
    size_t len = strlen(str);
    char* n_str = malloc(len+1);
    if(n_str) memcpy(n_str, str, len+1);
    return n_str;
}
#endif

#endif

