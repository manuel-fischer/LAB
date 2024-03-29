#include "LAB_memory.h"

#include "LAB_attr.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdatomic.h>

#if !defined NDEBUG && defined LAB_SHOW_DBG_MEM
#include "LAB_filesystem.h"
#endif


#ifndef NDEBUG
static atomic_size_t mem_allocs = 0;
static atomic_size_t mem_relocs = 0;
static atomic_size_t mem_frees = 0;
static atomic_size_t mem_fails = 0;
static atomic_ptrdiff_t mem_checksum = 0; // all pointers xored together

LAB_STATIC void LAB_DbgMemPrint(const char* format, const char* file, int line, ...);

void* LAB_RealMallocN(size_t count, size_t size)
{
    size_t size_bytes = count*size;
    if(LAB_UNLIKELY(size_bytes/size < count)) return NULL; // Overflow
    return LAB_RealMalloc(size_bytes);
}

void* LAB_RealReallocN(void* memory, size_t count, size_t size)
{
    size_t size_bytes = count*size;
    if(LAB_UNLIKELY(size_bytes/size < count)) return NULL; // Overflow
    return LAB_RealRealloc(memory, size_bytes);
}

typedef unsigned u;

//#define LAB_SHOW_DBG_MEM
#if !defined NDEBUG && defined LAB_SHOW_DBG_MEM
LAB_STATIC FILE* LAB_getMemLogFile()
{
#if 1
    return stderr;
#else
    static FILE* memlog = NULL;
    if(memlog == NULL) {
        memlog = fopen("mem.log", "w");
    }
    return memlog;
#endif
}
#define LAB_memlog (LAB_getMemLogFile())
#endif

LAB_STATIC void LAB_DbgMemPrint(const char* format, const char* file, int line, ...)
{
    #if !defined NDEBUG && defined LAB_SHOW_DBG_MEM
    fprintf(LAB_memlog, "[%s:%i]  ", LAB_Filename(file), line);
    va_list args;
    va_start(args, line);
    vfprintf(LAB_memlog, format, args);
    va_end(args);
    fflush(LAB_memlog);
    #endif
}

LAB_STATIC void LAB_DbgMemPrintf(const char* format, ...)
{
    #if !defined NDEBUG && defined LAB_SHOW_DBG_MEM
    va_list args;
    va_start(args, format);
    vfprintf(LAB_memlog, format, args);
    va_end(args);
    fflush(LAB_memlog);
    #endif
}

LAB_STATIC void LAB_DbgMemPrintResult(void* result)
{
    #if !defined NDEBUG && defined LAB_SHOW_DBG_MEM
    fprintf(LAB_memlog, " -> %p\n", result);
    #endif
}

void* LAB_DbgMalloc(size_t size, const char* file, int line)
{
    LAB_DbgMemPrint("malloc(%u)", file, line, (u)size);
    void* new_memory = malloc(size);
    LAB_DbgMemPrintResult(new_memory);

    if(new_memory != NULL)
        mem_allocs++;
    else if(size != 0)
        mem_fails++;

    mem_checksum ^= (ptrdiff_t)new_memory;
    return new_memory;
}

void* LAB_DbgRealloc(void* memory, size_t size, const char* file, int line)
{
    LAB_DbgMemPrint("realloc(%p, %u)", file, line, memory, (u)size);
    void* new_memory = realloc(memory, size);
    LAB_DbgMemPrintResult(new_memory);

    if(memory == NULL)
    {
        if(new_memory != NULL)
            mem_allocs++;
        else if(size != 0)
            mem_fails++;
    }
    else if(size != 0)
    {
        if(new_memory == NULL)
            mem_fails++;
        else if((ptrdiff_t)new_memory != (ptrdiff_t)memory)
            mem_relocs++;
    }
    else
    {
        if(new_memory == NULL)
            mem_frees++;
    }

    mem_checksum ^= (ptrdiff_t)memory;
    mem_checksum ^= (ptrdiff_t)new_memory;

    return new_memory;
}

void* LAB_DbgCalloc(size_t count, size_t size, const char* file, int line)
{
    LAB_DbgMemPrint("calloc(%u, %u)", file, line, (u)count, (u)size);
    void* new_memory = calloc(count, size);
    LAB_DbgMemPrintResult(new_memory);

    if(new_memory != NULL)
        mem_allocs++;
    else if(size != 0 && count != 0)
        mem_fails++;

    mem_checksum ^= (ptrdiff_t)new_memory;

    return new_memory;
}

void  LAB_DbgFree(void* memory, const char* file, int line)
{
    LAB_DbgMemPrint("free(%p)\n", file, line, memory);
    free(memory);

    if(memory != NULL)
        mem_frees++;

    mem_checksum ^= (ptrdiff_t)memory;
}

void* LAB_DbgMallocN(size_t count, size_t size, const char* file, int line)
{
    LAB_DbgMemPrint("mallocN(%u, %u)", file, line, (u)count, (u)size);
    void* new_memory = LAB_RealMallocN(count, size);
    LAB_DbgMemPrintResult(new_memory);

    if(new_memory != NULL)
        mem_allocs++;
    else if(size != 0 && count != 0)
        mem_fails++;

    mem_checksum ^= (ptrdiff_t)new_memory;

    return new_memory;
}

void* LAB_DbgReallocN(void* memory, size_t count, size_t size, const char* file, int line)
{
    LAB_DbgMemPrint("reallocN(%p, %u, %u)", file, line, memory, (u)count, (u)size);
    void* new_memory = LAB_RealReallocN(memory, count, size);
    LAB_DbgMemPrintResult(new_memory);

    if(memory == NULL)
    {
        if(new_memory != NULL)
            mem_allocs++;
        else if(size != 0)
            mem_fails++;
    }
    else if(size != 0 && count != 0)
    {
        if(new_memory == NULL)
            mem_fails++;
        else if((ptrdiff_t)new_memory != (ptrdiff_t)memory)
            mem_relocs++;
    }
    else
    {
        if(new_memory == NULL)
            mem_frees++;
    }


    mem_checksum ^= (ptrdiff_t)new_memory;
    mem_checksum ^= (ptrdiff_t)memory;

    return new_memory;
}


char* LAB_DbgStrDup(const char* str, const char* file, int line)
{
    LAB_DbgMemPrint("strdup(%p", file, line, (void*) str);
    LAB_DbgMemPrintf(" = \"%s\")", str);
    char* new_string = LAB_RealStrDup(str);
    LAB_DbgMemPrintResult(new_string);

    if(new_string != NULL)
        mem_allocs++;
    else
        mem_fails++;

    mem_checksum ^= (ptrdiff_t)new_string;
    return new_string;
}



void LAB_DbgMemShow(void)
{
    fprintf(stderr, "allocs:%5u\n"
                    "relocs:%5u\n"
                    "frees:%6u\n"
                    "fails:%6u\n"
                    "alives:%5u\n"
                    "check:  %p\n",
            (u)mem_allocs,
            (u)mem_relocs,
            (u)mem_frees,
            (u)mem_fails,
            (u)(mem_allocs-mem_frees),
            (void*)mem_checksum);

/*#if !defined NDEBUG && defined LAB_SHOW_DBG_MEM
    FILE** memlog = LAB_getMemLogFile();
    fclose(*memlog);
    *memlog = NULL;
#endif*/
}
#else
LAB_EMPTY_TU;
#endif
