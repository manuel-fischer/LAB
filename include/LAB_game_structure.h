#pragma once

#include "LAB_poly_buffer.h"
#include "LAB_gen.h" // -> placer
#include "LAB_random.h" // -> placer
#include "LAB_func.h"

// cx == 0 indicates an error
typedef struct LAB_Game_StructureSpan
{
    uint16_t cx, cy, cz; // cell size, only powers of two allowed!
} LAB_Game_StructureSpan;

typedef size_t LAB_Game_StructureSpanIndex;

typedef struct LAB_Game_StructureDensity
{
    int count;
} LAB_Game_StructureDensity;

typedef LAB_Game_StructureDensity (*LAB_Game_StructureDensity_Func)(const void* ctx, LAB_Game_StructureSpan span, uint64_t world_seed, LAB_Random* rnd, int cx, int cy, int cz);
LAB_DEF_FUNC_TYPES(LAB_Game_IStructureDensity, LAB_Game_StructureDensity_Func);

typedef bool (*LAB_Game_StructurePlacement_Func)(const void* ctx, uint64_t world_seed, int* x, int* y, int* z);
LAB_DEF_FUNC_TYPES(LAB_Game_IStructurePlacement, LAB_Game_StructurePlacement_Func);

typedef void (*LAB_Game_Structure_Func)(const void* ctx, LAB_Placer* p, LAB_Random* rnd);
LAB_DEF_FUNC_TYPES(LAB_Game_IStructure, LAB_Game_Structure_Func);




typedef struct LAB_Game_Structure
{
    LAB_Game_StructureSpan span;

    LAB_Game_IStructureDensity density;
    LAB_Game_IStructurePlacement placement;
    LAB_Game_IStructure structure;

} LAB_Game_Structure;


typedef struct LAB_Game_Structure_Opt
{
    LAB_Game_StructureSpanIndex span;
    uint64_t salt;

    LAB_Game_IStructureDensity_Opt density;
    LAB_Game_IStructurePlacement_Opt placement;
    LAB_Game_IStructure_Opt structure;

} LAB_Game_Structure_Opt;

#define LAB_Structure_Destroy(s) do { \
    LAB_FREE_FUNC((s)->structure); \
    LAB_FREE_FUNC((s)->placement); \
    LAB_FREE_FUNC((s)->density); \
} while(0)

LAB_INLINE
LAB_Game_Structure LAB_Structure_Copy(LAB_Game_Structure s)
{
    return (LAB_Game_Structure)
    {
        .span = s.span,
        .density = LAB_DUP_FUNC(LAB_Game_IStructureDensity, s.density),
        .placement = LAB_DUP_FUNC(LAB_Game_IStructurePlacement, s.placement),
        .structure = LAB_DUP_FUNC(LAB_Game_IStructure, s.structure),
    };
}



typedef struct LAB_Game_StructureSpanArray
{
    LAB_DEF_ARRAY_UNNAMED(LAB_Game_StructureSpan);
    #define LAB_Game_StructureSpanArray_spans(o) \
        LAB_REF_ARRAY_UNNAMED(LAB_Game_StructureSpan, o)
} LAB_Game_StructureSpanArray;

#define LAB_Game_StructureSpanArray_Create(a) \
    LAB_ARRAY_CREATE_EMPTY(LAB_Game_StructureSpanArray_spans(a))

#define LAB_Game_StructureSpanArray_Destroy(a) \
    LAB_ARRAY_DESTROY(LAB_Game_StructureSpanArray_spans(a))

size_t LAB_Game_StructureSpanID(LAB_Game_StructureSpanArray* a, LAB_Game_StructureSpan span);


typedef struct LAB_Game_StructureArray
{
    LAB_DEF_ARRAY(LAB_Game_Structure_Opt, structures);
    #define LAB_Game_StructureArray_structures(o) \
        LAB_REF_ARRAY(LAB_Game_Structure_Opt, o, structures)

    LAB_DEF_ARRAY(size_t, span_offsets);
    #define LAB_Game_StructureArray_span_offsets(o) \
        LAB_REF_ARRAY(size_t, o, span_offsets)
} LAB_Game_StructureArray;

bool LAB_Game_StructureArray_Create(LAB_Game_StructureArray* a);
void LAB_Game_StructureArray_Destroy(LAB_Game_StructureArray* a);

bool LAB_Game_StructureArray_Sort(const LAB_Game_StructureSpanArray* spans, LAB_Game_StructureArray* a);