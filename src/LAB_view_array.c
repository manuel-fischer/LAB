#include "LAB_view_array.h"

#include "LAB_memory.h"
#include "LAB_loop.h"

#include "LAB_chunk.h"

LAB_STATIC int LAB_ViewArray_IndexDistV(const void* p0v, const void* p1v);

bool LAB_ViewArray_Create(LAB_ViewArray* arr, size_t r)
{
    memset(arr, 0, sizeof *arr);

    return LAB_ViewArray_Resize(arr, r);
}

void LAB_ViewArray_Destroy(LAB_ViewArray* arr)
{
    LAB_Free(arr->sorted_offsets);
    LAB_Free(arr->entries);
}

bool LAB_ViewArray_Resize(LAB_ViewArray* arr, size_t r)
{
    size_t diameter = r ? 2*r-1 : 0;
    size_t l = 2;
    while(l < diameter) l <<= 1;
    size_t v = l*l*l;

    // TODO overflow check
    if(l != arr->cube_length)
    {
        //LAB_DbgPrintf("Resize\n");

        LAB_ViewChunkEntry** n_entries = LAB_Calloc(3*v, sizeof*n_entries);
        if(!n_entries) return false;

        LAB_ViewArray_ChunkOffset* sorted_offsets = LAB_ReallocN(arr->sorted_offsets, v, sizeof*sorted_offsets);
        if(!sorted_offsets)
        {
            LAB_Free(n_entries);
            return false;
        }

        
        // Setup sorted offsets
        arr->sorted_offsets = sorted_offsets;

        int l2 = (int)l/2;
        size_t i = 0;
        for(int z = -l2; z < l2; ++z)
        for(int y = -l2; y < l2; ++y)
        for(int x = -l2; x < l2; ++x, ++i)
        {
            sorted_offsets[i].x = x;
            sorted_offsets[i].y = y;
            sorted_offsets[i].z = z;
        }
        LAB_ASSERT(i == v);

        qsort(sorted_offsets, v, sizeof*sorted_offsets, &LAB_ViewArray_IndexDistV);

        // copy chunks, add far chunks to delete list
        size_t o_volume = LAB_ViewArray_Volume(arr);
        LAB_ViewChunkEntry** o_entries = arr->entries;

        arr->radius = r;
        arr->cube_length = l;
        
        arr->entries_count = 0;
        arr->entries = n_entries;
        LAB_ASSERT(v == LAB_ViewArray_Volume(arr));

        if(o_entries)
            for(size_t j = 0; j < o_volume; ++j)
                if(o_entries[j])
                    (void)LAB_ViewArray_SetClipped(arr, o_entries[j]);

        LAB_Free(o_entries);

        arr->entries_sorted = n_entries + v;
        arr->entries_sorted_count = 0;

        arr->entries_sorted_nonempty = n_entries + 2*v;
        arr->entries_sorted_nonempty_count = 0;
    }
    else
    {
        arr->radius = r;
        //arr->cube_length = l;
    }

    return true;
}


LAB_STATIC int LAB_ViewArray_IndexDistV(const void* p0v, const void* p1v)
{
    const LAB_ViewArray_ChunkOffset* p0 = p0v;
    const LAB_ViewArray_ChunkOffset* p1 = p1v;

    size_t d0 = p0->x*p0->x + p0->y*p0->y + p0->z*p0->z;
    size_t d1 = p1->x*p1->x + p1->y*p1->y + p1->z*p1->z;

    if(d0 < d1) return -1;
    if(d0 > d1) return +1;
    return 0;
}





void LAB_ViewArray_Clear(LAB_ViewArray* arr)
{
    size_t v = LAB_ViewArray_Volume(arr);
    for(size_t i = 0; i < v; ++i)
    {
        LAB_ViewChunkEntry* e = arr->entries[i];
        if(e)
        {
            LAB_ViewArray_PushDel(arr, e);
            arr->entries[i] = NULL;
        }
    }
    arr->entries_count = 0;
    arr->entries_sorted_count = 0;
    arr->entries_sorted_nonempty_count = 0;
}


void LAB_ViewArray_Collect(LAB_ViewArray* arr)
{
    LAB_ViewChunkEntry* e;

    arr->entries_sorted_count = 0;
    LAB_ViewArray_EACH_SORTED__BREAK_a(arr, e,
    {
        arr->entries_sorted[arr->entries_sorted_count] = e;
        arr->entries_sorted_count += !!e;
    });

    arr->entries_sorted_nonempty_count = 0;
    LAB_ViewArray_EACH_SORTED(arr, LAB_LOOP_FORWARD, e,
    {
        arr->entries_sorted_nonempty[arr->entries_sorted_nonempty_count] = e;
        arr->entries_sorted_nonempty_count += e->world_chunk && !e->world_chunk->empty;
    });

    //arr->entries_sorted_nonempty = arr->entries_sorted;
    //arr->entries_sorted_nonempty_count = arr->entries_sorted_count;

    LAB_ASSERT(arr->entries_sorted_count <= arr->entries_count);
    LAB_ASSERT(arr->entries_sorted_nonempty_count <= arr->entries_sorted_count);
}