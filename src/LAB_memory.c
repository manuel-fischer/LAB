#include "LAB_memory.h"
#include "LAB_opt.h"

void* LAB_ReallocN(void* memory, size_t count, size_t size)
{
    size_t size_bytes = count*size;
    if(LAB_UNLIKELY(size_bytes/size < count)) return NULL; // Overflow
    return LAB_Realloc(memory, size_bytes);
}
