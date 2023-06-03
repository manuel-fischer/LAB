#pragma once

#include "LAB_block.h"
#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_select.h"


typedef struct LAB_Gen_IBiome
{
    //double (* ground_height)(const void* user, int x, int z);

    const void* user;

    double ground_factor;

    LAB_BlockID (* ground_block)(const void* user, int x, int y, int z);

} LAB_Gen_IBiome;



typedef struct LAB_Gen_BiomeMix
{
    float u, v; // fractional mixing values
    const LAB_Gen_IBiome* biomes[4];

} LAB_Gen_BiomeMix;



typedef struct LAB_Gen_BiomePalette
{
    size_t w, h; // number of cells
    float du, dv; // fraction of gradient size over cell size

    const LAB_Gen_IBiome** palette;
} LAB_Gen_BiomePalette;






LAB_PURE LAB_INLINE
LAB_Gen_BiomeMix LAB_Gen_BiomePalette_Sample(const LAB_Gen_BiomePalette* p, double u, float v)
{
    LAB_Gen_BiomeMix mix;

    LAB_ASSERT_FMT(0.f <= u && u < p->w, "u=%f, v=%f", u, v);
    LAB_ASSERT_FMT(0.f <= v && v < p->h, "u=%f, v=%f", u, v);

    float u0 = u - p->du*0.5f;
    float v0 = v - p->dv*0.5f;

    float u1 = u0 + p->du;
    float v1 = v0 + p->dv;

    //float mix_u


    int iu0 = (int)u0, iv0 = (int)v0;
    iu0 = LAB_CLAMP_BL(iu0, 0, p->w-1);
    iv0 = LAB_CLAMP_BL(iv0, 0, p->h-1);

    int iu1 = (int)u1, iv1 = (int)v1;
    iu1 = LAB_CLAMP_BL(iu1, 0, p->w-1);
    iv1 = LAB_CLAMP_BL(iv1, 0, p->h-1);

    mix.biomes[0|0] = p->palette[iu0 + iv0 * p->w];
    mix.biomes[1|0] = p->palette[iu1 + iv0 * p->w];
    mix.biomes[0|2] = p->palette[iu0 + iv1 * p->w];
    mix.biomes[1|2] = p->palette[iu1 + iv1 * p->w];

    // TODO
    mix.u = 0;
    mix.v = 0;

    return mix;
}
