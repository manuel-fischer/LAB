
#include "LAB_game_structure.h"

#include "LAB_stdinc.h"



size_t LAB_Game_StructureSpanID(LAB_Game_StructureSpanArray* a, LAB_Game_StructureSpan span)
{
    for(size_t i = 0; i < a->count; ++i)
        if(memcmp(&a->data[i], &span, sizeof span) == 0) return i;
    
    LAB_Game_StructureSpan* entry;
    LAB_ARRAY_APPEND(LAB_Game_StructureSpanArray_spans(a), 1, &entry);
    if(!entry) return 0; // TODO: handle error
    *entry = span;
    return a->count-1;
}




bool LAB_Game_StructureArray_Create(LAB_Game_StructureArray* a)
{
    LAB_ARRAY_CREATE_EMPTY(LAB_Game_StructureArray_structures(a));
    LAB_ARRAY_CREATE_EMPTY(LAB_Game_StructureArray_span_offsets(a));
    return true;
}

void LAB_Game_StructureArray_Destroy(LAB_Game_StructureArray* a)
{
    LAB_ARRAY_DESTROY(LAB_Game_StructureArray_span_offsets(a));
    LAB_ARRAY_DESTROY(LAB_Game_StructureArray_structures(a));
}


int LAB_Game_StructureArray_Sort_CmpFunc(const void* lhs, const void* rhs)
{
    size_t lhs_span = ((const LAB_Game_Structure_Opt*)lhs)->span;
    size_t rhs_span = ((const LAB_Game_Structure_Opt*)rhs)->span;

    return lhs_span < rhs_span ? -1 : lhs_span > rhs_span ? 1 : 0;
}

bool LAB_Game_StructureArray_Sort(const LAB_Game_StructureSpanArray* spans, LAB_Game_StructureArray* a)
{
    if(spans->count == 0) return true;

    bool success;
    LAB_ARRAY_RESIZE(&success, LAB_Game_StructureArray_span_offsets(a), spans->count);
    if(!success) return false;

    qsort(a->structures, a->structures_count, sizeof(*a->structures), LAB_Game_StructureArray_Sort_CmpFunc);

    a->span_offsets[0] = 0;
    size_t prev_span = 0;
    for(size_t i = 0; i < a->structures_count; ++i)
    {
        LAB_ASSERT(a->structures[i].span >= prev_span);
        size_t new_span = a->structures[i].span;
        for(size_t j = prev_span+1; j <= new_span; ++j)
            a->span_offsets[j] = i;
        prev_span = new_span;
    }
    for(size_t j = prev_span+1; j < spans->count; ++j)
        a->span_offsets[j] = a->structures_count;

    return true;
}