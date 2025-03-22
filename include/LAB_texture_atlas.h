#pragma once

#include "LAB_stdinc.h"
#include "LAB_array.h"
#include "LAB_color.h"
#include "LAB_vec2.h"
#include "LAB_bits.h"
#include "LAB_gl_types.h"
#include "LAB_image.h"
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

typedef struct LAB_TexAlloc_Result
{
    LAB_Vec2Z topleft_cell;
    bool success;
} LAB_TexAlloc_Result;

LAB_TexAlloc_Result LAB_TexAlloc_Add(LAB_TexAlloc* alloc, size_t size);


typedef struct LAB_TexAtlas
{
    LAB_Color* data;
    size_t w, h;
    size_t cell_size;
    size_t capacity;
    LAB_GL_Texture tex;
} LAB_TexAtlas;


// cell_size: the size in pixels of a region that gets collapsed to a single
// pixel in the lowest mip-map level, use 1 to disable mipmaps
bool LAB_TexAtlas_Create(LAB_TexAtlas* atlas, size_t cell_size);
void LAB_TexAtlas_Destroy(LAB_TexAtlas* atlas);
LAB_ImageView LAB_TexAtlas_AsImageView(LAB_TexAtlas* atlas);
LAB_ImageView LAB_TexAtlas_CellImageView(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size);
LAB_ImageView LAB_TexAtlas_ClipImageView(LAB_TexAtlas* atlas, LAB_Box2Z rect_cells);
bool LAB_TexAtlas_Alloc(LAB_TexAtlas* atlas, LAB_Box2Z rect_cells);
void LAB_TexAtlas_MakeMipmap(LAB_TexAtlas* atlas);
bool LAB_TexAtlas_Upload2GL(LAB_TexAtlas* atlas);
#define LAB_TexAtlas_Levels(atlas) (1 + LAB_Log2OfPow2((atlas)->cell_size))
#ifndef NDEBUG
bool LAB_TexAtlas_DbgDumpToFile(LAB_TexAtlas* atlas, const char* prefix);
#endif
LAB_Vec2F LAB_TexAtlas_ScaleFactor(LAB_TexAtlas* atlas);

