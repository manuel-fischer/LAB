#pragma once

#include "LAB_stdinc.h"
#include "LAB_array.h"
#include "LAB_color.h"
#include <SDL2/SDL_surface.h>

// With the help of a quadtree, the most optimal arrangement for
// aligned! squared textures is produced
#define LAB_TexAlloc_Node_NULL SIZE_MAX
#define LAB_TexAlloc_EMPTY  0
#define LAB_TexAlloc_MIXED  1
#define LAB_TexAlloc_FULL   2
typedef union LAB_TexAlloc_Node
{
    struct
    {
        size_t childs[4]; // only initialized if the given state is LAB_TexAlloc_MIXED
        uint8_t states[4];
        size_t parent;
        size_t empty_cells; // amount of empty cells, never 0, because those nodes are removed
        size_t iter_state;
    };
    size_t next_unoccupied;
} LAB_TexAlloc_Node; // pow2 size

// the root node always has

typedef struct LAB_TexAlloc
{
    LAB_TexAlloc_Node* nodes;
    size_t nodes_size;
    #define LAB_TexAlloc_Nodes(texalloc) (LAB_TexAlloc_Node, (texalloc)->nodes, (texalloc)->nodes_size)

    size_t root_node;
    size_t root_size; // unit: cells

    size_t next_unoccupied;

} LAB_TexAlloc;

bool LAB_TexAlloc_Create(LAB_TexAlloc* alloc);
void LAB_TexAlloc_Destroy(LAB_TexAlloc* alloc);

// size, topleft_cell -- unit: cells
bool LAB_TexAlloc_Add(LAB_TexAlloc* alloc, size_t size, LAB_OUT size_t topleft_cell[2]);


typedef struct LAB_TexAtlas
{
    LAB_Color* data;
    size_t w, h;
    size_t cell_size;
    size_t capacity;
    unsigned gl_id;
} LAB_TexAtlas;


// cell_size: the size in pixels of a region that gets collapsed to a single
// pixel in the lowest mip-map level, use 1 to disable mipmaps
bool LAB_TexAtlas_Create(LAB_TexAtlas* atlas, size_t cell_size);
void LAB_TexAtlas_Destroy(LAB_TexAtlas* atlas);
void LAB_TexAtlas_Clear(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color clear);
void LAB_TexAtlas_Draw(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data);
void LAB_TexAtlas_DrawBlit(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data, LAB_Color black_tint, LAB_Color white_tint);
bool LAB_TexAtlas_ClearAlloc(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color clear);
bool LAB_TexAtlas_DrawAlloc(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data);
void LAB_TexAtlas_MakeMipmap(LAB_TexAtlas* atlas);
bool LAB_TexAtlas_Upload2GL(LAB_TexAtlas* atlas);
void LAB_TexAtlas_LoadTexMatrix(LAB_TexAtlas* atlas);


void LAB_TestTextureAtlas();
