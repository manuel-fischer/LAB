#pragma once

#include "LAB_stdinc.h"
#include "LAB_attr.h"
#include "LAB_memory.h"
#include "LAB_util.h"

typedef struct LAB_PolyBuffer
{
    void* data;
    size_t size;
    size_t capacity;
} LAB_PolyBuffer;

typedef size_t LAB_PolyBufferPtr;
#define LAB_POLY_BUFFER_NULLPTR ((LAB_PolyBufferPtr)-1)

LAB_INLINE
bool LAB_PolyBuffer_Create(LAB_PolyBuffer* b)
{
    memset(b, 0, sizeof* b);
    return true;
}

LAB_INLINE
void LAB_PolyBuffer_Destroy(LAB_PolyBuffer* b)
{
    LAB_Free(b->data);
}

LAB_INLINE
const void* LAB_PolyBuffer_Get(LAB_PolyBuffer* b, LAB_PolyBufferPtr index)
{
    LAB_ASSERT(index != LAB_POLY_BUFFER_NULLPTR);
    return LAB_PTR_OFFSET(b->data, index, 1);
}

LAB_INLINE
const void* LAB_PolyBuffer_GetOrNull(const LAB_PolyBuffer* b, LAB_PolyBufferPtr index)
{
    if(index == LAB_POLY_BUFFER_NULLPTR) return NULL;
    return LAB_PTR_OFFSET(b->data, index, 1);
}

LAB_INLINE
bool LAB_PolyBuffer_Find(const LAB_PolyBuffer* b, const void* data, size_t size, size_t alignment, LAB_OUT LAB_PolyBufferPtr* out_index)
{
    for(size_t o = 0; o+size <= b->size; o += alignment)
    {
        if(memcmp(b->data, data, size) == 0)
        {
            if(out_index != NULL)
                *out_index = o;
            return true;
        }
    }
    return false;
}

LAB_INLINE
bool LAB_PolyBuffer_Append(LAB_PolyBuffer* b, size_t size, size_t alignment, LAB_OUT LAB_PolyBufferPtr* out_index)
{
    size_t index = LAB_RoundUpAlign(b->size, alignment); // todo align

    if(index + size > b->capacity)
    {
        size_t new_capacity = b->capacity ? b->capacity*2 : 64;
        while(index + size > new_capacity)
            new_capacity *= 2;
        void* data = LAB_Realloc(b->data, new_capacity);
        if(data == NULL) return false;

        b->data = data;
        b->capacity = new_capacity;
    }

    if(index - b->size)
        // fill padded bytes with 0
        memset(LAB_PTR_OFFSET(b->data, b->size, 1), 0, index - b->size);

    if(out_index != NULL)
        *out_index = index;

    b->size = index+size;
    return true;
}

LAB_INLINE
bool LAB_PolyBuffer_AppendCopy(LAB_PolyBuffer* b, const void* data, size_t size, size_t alignment, LAB_OUT LAB_PolyBufferPtr* out_index)
{
    if(LAB_PolyBuffer_Find(b, data, size, alignment, out_index)) return true;

    size_t index;
    if(!LAB_PolyBuffer_Append(b, size, alignment, &index)) return false;

    if(out_index != NULL)
        *out_index = index;
    
    memcpy(LAB_PTR_OFFSET(b->data, index, 1), data, size);
    return true;
}

#define LAB_PolyBuffer_AppendCopyOf(b, type, data, out_index) \
    LAB_PolyBuffer_AppendCopy(b, data, sizeof(type), alignof(type), out_index)

