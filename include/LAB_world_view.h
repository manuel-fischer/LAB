#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"

// DEPEND
typedef struct LAB_World LAB_World;
typedef struct LAB_Chunk LAB_Chunk;
typedef struct LAB_Window LAB_Window;
// END DEPEND

typedef struct LAB_ViewVertex
{
    float x, y, z;
    uint8_t r, g, b, a;

    float u, v;
    char padding[8];
} LAB_ViewVertex;

typedef struct LAB_ViewTriangle
{
    LAB_ViewVertex v[3];
} LAB_ViewTriangle;

typedef struct LAB_ViewChunkEntry
{
    int x, y, z;


    size_t mesh_count, mesh_capacity;
    LAB_ViewTriangle* mesh;

    unsigned dirty:1, used:1;

    unsigned vbo;

} LAB_ViewChunkEntry;







/*#define LAB_VIEW_CHUNK_MAP_NAME             LAB_ViewChunkMap
#define LAB_VIEW_CHUNK_MAP_KEY_TYPE         LAB_ChunkPos
#define LAB_VIEW_CHUNK_MAP_VALUE_TYPE       LAB_ViewChunkEntry
#define LAB_VIEW_CHUNK_MAP_HASH_FUNC        LAB_ChunkPosHash
#define LAB_VIEW_CHUNK_MAP_COMP_FUNC        LAB_ChunkPosComp
#define LAB_VIEW_CHUNK_MAP_CALLOC           LAB_Calloc
#define LAB_VIEW_CHUNK_MAP_FREE             LAB_Free
#define LAB_VIEW_CHUNK_MAP_LOAD_NUM         3
#define LAB_VIEW_CHUNK_MAP_LOAD_DEN         4
#define LAB_VIEW_CHUNK_MAP_INITIAL_CAPACITY 16
#define LAB_VIEW_CHUNK_MAP_NULL_REPR        entry->value->used != 0

#define HTL_PARAM LAB_VIEW_CHUNK_MAP
#include "HTL_hashmap.t.h"
#undef HTL_PARAM*/


#define LAB_VIEW_USE_VBO 1u

typedef struct LAB_View
{
    // View position
    double x, y, z;
    // View angle
    double ax, ay, az;

    // Cache
    size_t chunk_count, chunk_capacity;
    LAB_ViewChunkEntry* chunks;

    //LAB_ViewChunkMap chunks;

    LAB_World* world;

    uint32_t flags;
} LAB_View;


LAB_View* LAB_CreateView(LAB_World* world);
void LAB_DestroyView(LAB_View* world_view);


void LAB_ViewChunkProc(void* user, LAB_World* world, int x, int y, int z);
void LAB_ViewRenderProc(void* user, LAB_Window* window);

LAB_ViewChunkEntry* LAB_ViewGetChunkEntry(LAB_View* view, int x, int y, int z);
void LAB_ViewInvalidateEverything(LAB_View* view);

void LAB_ViewGetDirection(LAB_View* view, LAB_OUT float dir[3]);
