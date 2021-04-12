#include "LAB_memory.h"

#include "LAB_attr.h"
#include "LAB_opt.h"
#include "LAB_util.h"
#include <stdio.h>
#include <stdarg.h>

#ifndef NDEBUG
static size_t mem_allocs = 0;
static size_t mem_relocs = 0;
static size_t mem_frees = 0;
static size_t mem_fails = 0;
static ptrdiff_t mem_checksum = 0; // all pointers xored together

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

LAB_STATIC void LAB_DbgMemPrint(const char* format, const char* file, int line, ...)
{
    #if 0
    fprintf(stderr, "%-16s|%3i:\t", LAB_Filename(file), line);
    va_list args;
    va_start(args, line);
    vfprintf(stderr, format, args);
    va_end(args);
    #endif
}

void* LAB_DbgMalloc(size_t size, const char* file, int line)
{
    void* new_memory = malloc(size);
    LAB_DbgMemPrint("malloc(%u) -> %p\n", file, line, (u)size, new_memory);

    if(new_memory != NULL)
        mem_allocs++;
    else if(size != 0)
        mem_fails++;

    mem_checksum ^= (ptrdiff_t)new_memory;
    return new_memory;
}

void* LAB_DbgRealloc(void* memory, size_t size, const char* file, int line)
{
    void* new_memory = realloc(memory, size);
    LAB_DbgMemPrint("realloc(%p, %u) -> %p\n", file, line, memory, (u)size, new_memory);

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
    void* new_memory = calloc(count, size);
    LAB_DbgMemPrint("calloc(%u, %u) -> %p\n", file, line, (u)count, (u)size, new_memory);

    if(new_memory != NULL)
        mem_allocs++;
    else if(size != 0 && count != 0)
        mem_fails++;

    mem_checksum ^= (ptrdiff_t)new_memory;

    return new_memory;
}

void  LAB_DbgFree(void* memory, const char* file, int line)
{
    free(memory);
    LAB_DbgMemPrint("free(%p)\n", file, line, memory);

    if(memory != NULL)
        mem_frees++;

    mem_checksum ^= (ptrdiff_t)memory;
}

void* LAB_DbgMallocN(size_t count, size_t size, const char* file, int line)
{
    void* new_memory = LAB_RealMallocN(count, size);
    LAB_DbgMemPrint("mallocN(%u, %u) -> %p\n", file, line, (u)count, (u)size, new_memory);

    if(new_memory != NULL)
        mem_allocs++;
    else if(size != 0 && count != 0)
        mem_fails++;

    mem_checksum ^= (ptrdiff_t)new_memory;

    return new_memory;
}

void* LAB_DbgReallocN(void* memory, size_t count, size_t size, const char* file, int line)
{
    void* new_memory = LAB_RealReallocN(memory, count, size);
    LAB_DbgMemPrint("reallocN(%p, %u, %u) -> %p\n", file, line, memory, (u)count, (u)size, new_memory);

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
    char* new_string = LAB_RealStrDup(str);
    LAB_DbgMemPrint("strdup(%s) -> %p\n", file, line, str, new_string);

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
                    "check: %p\n",
            (u)mem_allocs,
            (u)mem_relocs,
            (u)mem_frees,
            (u)mem_fails,
            (u)(mem_allocs-mem_frees),
            (void*)mem_checksum);
}
#else
LAB_EMPTY_TU;
#endif
