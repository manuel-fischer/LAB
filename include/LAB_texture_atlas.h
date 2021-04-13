#pragma once

#include "LAB_stdinc.h"
#include "LAB_array.h"
#include "LAB_color.h"
#include <SDL2/SDL_surface.h>

#if 0
typedef int LAB_TexID;
#define LAB_TEX_ID_INVALID (-1)

///// LAB_TexClt /////

typedef struct LAB_TexClt // texture collector
{
    LAB_ARRAY(size_t) texture_sizes;
    size_t cell_size;
} LAB_TexClt;

bool      LAB_TexClt_Create(LAB_TexClt* texbld, size_t cell_size);
void      LAB_TexClt_Destroy(LAB_TexClt* texbld);
LAB_TexID LAB_TexClt_AddTexture(LAB_TexClt* texbld, size_t width, size_t height);


///// LAB_TexBld /////

typedef struct LAB_TexPos
{
    size_t x, y;
} LAB_TexPos;

typedef struct LAB_TexBld // Texture atlas layout builder
{
    LAB_TexClt* texclt;
    // all size_t-s
    size_t*     tex_order;     // owns tex_order_inv and tex_pos
    size_t*     tex_order_inv; // no own
    LAB_TexPos* tex_pos;       // no own, indexed by original index in texclt

    size_t at_width, at_height;
} LAB_TexBld;

bool LAB_TexBld_Create(LAB_TexBld* texbld, LAB_TexClt* texclt);
void LAB_TexBld_Destroy(LAB_TexBld* texbld);
void LAB_TexBld_Sort(LAB_TexBld*);
bool LAB_TexBld_Arrange(LAB_TexBld* texbld);



typedef struct LAB_TexAnim // texture animation
{
    size_t offset; // left top pos in data
    /*TODO frames*/
} LAB_TexAnim;

typedef struct LAB_TexAtlas
{
    size_t width, height, cell_size;
    LAB_Color* data;

    //LAB_ARRAY(LAB_TexAnim) animations;
} LAB_TexAtlas;
#endif


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
    LAB_ARRAY(LAB_TexAlloc_Node) nodes;
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
    size_t width, height;
} LAB_TexAtlas;


bool LAB_TexAtlas_Create(LAB_TexAtlas* atlas);
void LAB_TexAtlas_Destroy(LAB_TexAtlas* atlas);
void LAB_TexAtlas_Draw(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data);
bool LAB_TexAtlas_DrawAlloc(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data);


void LAB_TestTextureAtlas();
