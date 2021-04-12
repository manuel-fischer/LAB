#pragma once

#include "LAB_stdinc.h"
#include "LAB_array.h"
#include "LAB_color.h"
#include <SDL2/SDL_surface.h>

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

