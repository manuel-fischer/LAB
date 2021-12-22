#pragma once

#include "LAB_chunk_pos.h"
#include "LAB_view_chunk.h"
#include "LAB_arith.h"

typedef LAB_ChunkPos LAB_ViewArray_ChunkOffset;

typedef struct LAB_ViewArray
{
    LAB_ChunkPos origin;
    size_t radius;
    size_t cube_length; // always a pow2
    LAB_ViewChunkEntry** entries; // cube_length**3
    LAB_ViewArray_ChunkOffset* sorted_offsets;

    LAB_ViewChunkEntry* del_list;
    
    size_t entries_sorted_count;
    LAB_ViewChunkEntry** entries_sorted;

    size_t entries_sorted_nonempty_count;
    LAB_ViewChunkEntry** entries_sorted_nonempty;
} LAB_ViewArray;

bool LAB_ViewArray_Create(LAB_ViewArray* arr, size_t r);
void LAB_ViewArray_Destroy(LAB_ViewArray* arr);

bool LAB_ViewArray_Resize(LAB_ViewArray* arr, size_t r);

// Transfer all chunks into the delete list
void LAB_ViewArray_Clear(LAB_ViewArray* arr);

void LAB_ViewArray_Collect(LAB_ViewArray* arr);


LAB_INLINE
void LAB_ViewArray_SetOrigin(LAB_ViewArray* arr, LAB_ChunkPos chunk)
{
    arr->origin = chunk;
}



LAB_INLINE
size_t LAB_ViewArray_Volume(LAB_ViewArray* arr)
{
    size_t l = arr->cube_length;
    return l*l*l;
}

// undefined if array is null sized
LAB_INLINE
size_t LAB_ViewArray_Index(LAB_ViewArray* arr, LAB_ChunkPos p)
{
    size_t l = arr->cube_length;
    size_t mask = l - 1;

    size_t index;
    index =          p.z & mask;
    index = LAB_XADD(p.y & mask, index * l);
    index = LAB_XADD(p.x & mask, index * l);

    return index;
}

LAB_INLINE
LAB_ViewChunkEntry* LAB_ViewArray_Get(LAB_ViewArray* arr, LAB_ChunkPos p)
{
    size_t index = LAB_ViewArray_Index(arr, p);
    LAB_ViewChunkEntry* e = arr->entries[index];
    if(e && (e->x != p.x || e->y != p.y || e->z != p.z))
        return NULL;
    return e;
}

LAB_INLINE
void LAB_ViewArray_Set(LAB_ViewArray* arr, LAB_ViewChunkEntry* e)
{
    LAB_ASSERT(e);
    LAB_ChunkPos p = { e->x, e->y, e->z };
    size_t index = LAB_ViewArray_Index(arr, p);
    LAB_ViewChunkEntry* e2 = arr->entries[index];
    LAB_ASSERT(e != e2);
    //if(e == e2) return;
    if(e2 && (e->x != e2->x || e->y != e2->y || e->z != e2->z))
    {
        LAB_ASSERT(!e2->del_list_next);
        e2->del_list_next = arr->del_list;
        arr->del_list = e2;
    }
    arr->entries[index] = e;
}

LAB_INLINE
bool LAB_ViewArray_SetClipped(LAB_ViewArray* arr, LAB_ViewChunkEntry* e)
{
    LAB_ASSERT(e);
    if(abs(arr->origin.x - e->x) >= arr->radius ||
       abs(arr->origin.y - e->y) >= arr->radius ||
       abs(arr->origin.z - e->z) >= arr->radius)
    {
        LAB_ASSERT(!e->del_list_next);
        e->del_list_next = arr->del_list;
        arr->del_list = e;
        return false;
    }
    
    LAB_ViewArray_Set(arr, e);
    return true;
}





// e: lvalue LAB_ViewChunkEntry*
#define LAB_ViewArray_EACH(arr, e, ...) do \
{ \
    size_t LAB_ViewArray_v = LAB_ViewArray_Volume(arr); \
    for(size_t LAB_ViewArray_i = 0; LAB_ViewArray_i < LAB_ViewArray_v; ++LAB_ViewArray_i) \
    { \
        (e) = (arr)->entries[LAB_ViewArray_i]; \
        if((e)) { __VA_ARGS__ } \
    } \
} \
while(0)

// loop_dir from LAB_loop.h
#define LAB_ViewArray_EACH_SORTED_a(arr, loop_dir, e, ...) do \
{ \
    size_t LAB_ViewArray_v = LAB_ViewArray_Volume(arr); \
    loop_dir(size_t, LAB_ViewArray_i, 0, LAB_ViewArray_v, \
    { \
        LAB_ViewArray_ChunkOffset* LAB_ViewArray_offset = &(arr)->sorted_offsets[LAB_ViewArray_i]; \
        LAB_ChunkPos p = { \
            (arr)->origin.x+LAB_ViewArray_offset->x, \
            (arr)->origin.y+LAB_ViewArray_offset->y, \
            (arr)->origin.z+LAB_ViewArray_offset->z, \
        }; \
        (e) = LAB_ViewArray_Get(arr, p); \
        if((e)) { __VA_ARGS__ } \
    }); \
} \
while(0)

#define LAB_ViewArray_EACH_SORTED__BREAK_a(arr, e, ...) do \
{ \
    size_t LAB_ViewArray_r2 = (arr)->radius*(arr)->radius; \
    size_t LAB_ViewArray_v = LAB_ViewArray_Volume(arr); \
    for(size_t LAB_ViewArray_i = 0; LAB_ViewArray_i < LAB_ViewArray_v; ++LAB_ViewArray_i) \
    { \
        LAB_ViewArray_ChunkOffset* LAB_ViewArray_offset = &(arr)->sorted_offsets[LAB_ViewArray_i]; \
        size_t LAB_ViewArray_dist = LAB_ViewArray_offset->x*LAB_ViewArray_offset->x \
                                  + LAB_ViewArray_offset->y*LAB_ViewArray_offset->y \
                                  + LAB_ViewArray_offset->z*LAB_ViewArray_offset->z; \
        if(LAB_ViewArray_dist >= LAB_ViewArray_r2) break; \
        LAB_ChunkPos p = { \
            (arr)->origin.x+LAB_ViewArray_offset->x, \
            (arr)->origin.y+LAB_ViewArray_offset->y, \
            (arr)->origin.z+LAB_ViewArray_offset->z, \
        }; \
        (e) = LAB_ViewArray_Get(arr, p); \
        { __VA_ARGS__ } \
    } \
} \
while(0)

// loop_dir from LAB_loop.h
#define LAB_ViewArray_EACH_SORTED(arr, loop_dir, e, ...) do \
{ \
    loop_dir(size_t, LAB_ViewArray_i, 0, (arr)->entries_sorted_count, \
    { \
        (e) = (arr)->entries_sorted[LAB_ViewArray_i]; \
        { __VA_ARGS__ } \
    }); \
} \
while(0)

// loop_dir from LAB_loop.h
#define LAB_ViewArray_EACH_NONEMPTY_SORTED(arr, loop_dir, e, ...) do \
{ \
    loop_dir(size_t, LAB_ViewArray_i, 0, (arr)->entries_sorted_nonempty_count, \
    { \
        (e) = (arr)->entries_sorted_nonempty[LAB_ViewArray_i]; \
        { __VA_ARGS__ } \
    }); \
} \
while(0)

//#undef LAB_ViewArray_EACH_SORTED
//#define LAB_ViewArray_EACH_SORTED LAB_ViewArray_EACH_SORTED_a

// e: lvalue LAB_ViewChunkEntry*
#define LAB_ViewArray_DeleteList_EACH(arr, e, ...) do \
{ \
    while((arr)->del_list) \
    { \
        (e) = (arr)->del_list; \
        (arr)->del_list = (e)->del_list_next; \
        { __VA_ARGS__ } \
    } \
} \
while(0)
