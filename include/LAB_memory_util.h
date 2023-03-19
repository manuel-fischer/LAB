#pragma once

#include "LAB_memory.h"


LAB_INLINE
void* LAB_MemDup(void* mem, size_t size, size_t align)
{
    (void)align;

    void* new_mem = LAB_Malloc(size);
    if(new_mem != NULL) memcpy(new_mem, mem, size);
    return new_mem;
}