#include "LAB_gen_overworld.h"
#include "LAB_random.h"
#include "LAB_opt.h"
//#include "LAB_noise.h"
#include "LAB_bits.h"

#include "LAB_simplex_noise.h"

#include "LAB_gen_overworld_shape.h"
#include "LAB_gen_overworld_structures.h"




#define LAB_MAX_STRUCTURE_SIZE 1 /*in chunks*/


#define SMTST(smth, prob, orig) ((smth) > (orig)-(prob)/2 && (smth) < (orig)+(prob)/2)

#define LAB_GEN_CRYSTAL_SALT     0x98765

LAB_STATIC void LAB_Gen_PopulateLayers(LAB_GenOverworld* gen, LAB_Chunk* chunk,
                                       const LAB_StructureLayer* layers, size_t layer_count,
                                       int cx, int cy, int cz);

LAB_STATIC void LAB_Gen_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p,
                                           const LAB_StructureLayer* layer,
                                           int cx, int cy, int cz);


#if 0
//LAB_STATIC void LAB_Gen_Surface(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Surface_PopulateLayer(LAB_GenOverworld* gen, LAB_Chunk* chunk, const LAB_StructureLayer* lyr, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Surface_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p, const LAB_StructureLayer* lyr, int cx, int cy, int cz);
#endif
#if 0
LAB_STATIC void LAB_Gen_Surface_Populate(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Surface_Populate_Func(LAB_GenOverworld* gen, LAB_Placer* p, int cx, int cy, int cz);
#endif
LAB_STATIC void LAB_Gen_Cave(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);

LAB_DEPRECATED("old")
LAB_STATIC void LAB_Gen_Cave_Crystals(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Cave_RockVariety(LAB_GenOverworld* gen, LAB_Chunk* chunk, int cx, int cy, int cz);

LAB_STATIC void LAB_Gen_Cave_PopulateLayer(LAB_GenOverworld* gen, LAB_Chunk* chunk, const LAB_StructureLayer* lyr, int cx, int cy, int cz);
LAB_STATIC void LAB_Gen_Cave_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p, const LAB_StructureLayer* lyr, int cx, int cy, int cz);





LAB_HOT
LAB_Chunk* LAB_GenOverworldProc(void* user, /*unused*/LAB_World* world_, int x, int y, int z)
{
    LAB_GenOverworld* gen = user;

    LAB_Block* block = y < LAB_SURFACE_MAX_CY ? &LAB_BLOCK_STONE : &LAB_BLOCK_AIR;
    LAB_Chunk* chunk = LAB_CreateChunk(block);
    if(!chunk) return NULL;

    //LAB_Gen_Surface(gen, chunk, x, y, z);

    LAB_Gen_Surface_Shape(gen, chunk, x, y, z);
    //return chunk; // DBG
    LAB_Gen_Cave(gen, chunk, x, y, z);
    //LAB_Gen_Surface_Populate(gen, chunk, x, y, z);
    /*for(size_t i = 0; i < overworld_layers_count; ++i)
        LAB_Gen_Surface_PopulateLayer(gen, chunk, &overworld_layers[i], x, y, z);*/
    LAB_Gen_PopulateLayers(gen, chunk, overworld_layers, overworld_layers_count, x, y, z);

    return chunk;
}


/*LAB_STATIC void LAB_Gen_Surface(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Gen_Surface_Shape(gen, chunk, x, y, z);
    LAB_Gen_Surface_Populate(gen, chunk, x, y, z);
}*/




LAB_STATIC void LAB_Gen_PopulateLayers(LAB_GenOverworld* gen, LAB_Chunk* chunk,
                                              const LAB_StructureLayer* layers, size_t layer_count,
                                              int cx, int cy, int cz)
{
    // TODO: check lyr->min/max_height +- 16*LAB_MAX_STRUCTURE_SIZE

    for(size_t i = 0; i < layer_count; ++i)
    {
        int structure_radius = layers[i].max_chunk_radius;
        for(int z = -structure_radius; z <= structure_radius; ++z)
        for(int y = -structure_radius; y <= structure_radius; ++y)
        for(int x = -structure_radius; x <= structure_radius; ++x)
        {
            LAB_Placer p;
            p.chunk = chunk;
            p.ox = -16*x;
            p.oy = -16*y;
            p.oz = -16*z;
            LAB_Gen_PopulateLayer_Func(gen, &p, &layers[i], cx+x, cy+y, cz+z);
        }
    }
}



LAB_STATIC void LAB_Gen_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p,
                                           const LAB_StructureLayer* lyr,
                                           int cx, int cy, int cz)
{
    LAB_Random rnd;
    LAB_ChunkRandom(&rnd, gen->seed^lyr->salt, cx, cy, cz);

    int count;

    // place plant groups
    uint64_t r = LAB_NextRandom(&rnd);
    if((int)(r&0xff) < lyr->probability)
    {
        count = lyr->min_count+((r>>8)%(lyr->max_count-lyr->min_count+1));
        for(int i = 0; i < count; ++i)
        {
            int xz = LAB_NextRandom(&rnd);
            // absolute
            int ax = (xz    & 15) | cx<<4;
            int ay = (xz>>4 & 15) | cy<<4;
            int az = (xz>>8 & 15) | cz<<4;

            if(lyr->near_place_func(gen, &ax, &ay, &az)
                && ax>>4 == cx
                && ay>>4 == cy
                && az>>4 == cz
                && lyr->min_height <= ay && ay <= lyr->max_height)
            {
                LAB_Placer p2;
                p2.chunk = p->chunk;
                p2.ox = p->ox - (ax&15);
                p2.oy = p->oy - (ay&15);
                p2.oz = p->oz - (az&15);

                lyr->structure_func(&p2, &rnd);
            }
        }
    }
}



#if 0
LAB_STATIC void LAB_Gen_Surface_PopulateLayer(LAB_GenOverworld* gen, LAB_Chunk* chunk, const LAB_StructureLayer* lyr, int cx, int cy, int cz)
{
    for(int z = -LAB_MAX_STRUCTURE_SIZE; z <= LAB_MAX_STRUCTURE_SIZE; ++z)
    for(int y = -LAB_MAX_STRUCTURE_SIZE; y <= LAB_MAX_STRUCTURE_SIZE; ++y)
    for(int x = -LAB_MAX_STRUCTURE_SIZE; x <= LAB_MAX_STRUCTURE_SIZE; ++x)
    {
        LAB_Placer p;
        p.chunk = chunk;
        p.ox = -16*x;
        p.oy = -16*y;
        p.oz = -16*z;
        LAB_Gen_Surface_PopulateLayer_Func(gen, &p, lyr, cx+x, cy+y, cz+z);
    }
}

LAB_STATIC void LAB_Gen_Surface_PopulateLayer_Func(LAB_GenOverworld* gen, LAB_Placer* p, const LAB_StructureLayer* lyr, int cx, int cy, int cz)
{
    LAB_Random rnd;
    LAB_ChunkRandom(&rnd, gen->seed^lyr->salt, cx, cy, cz);

    int count;

    // place plant groups
    uint64_t r = LAB_NextRandom(&rnd);
    if((int)(r&0xff) < lyr->probability)
    {
        count = lyr->min_count+((r>>8)%(lyr->max_count-lyr->min_count+1));
        for(int i = 0; i < count; ++i)
        {
            int xz = LAB_NextRandom(&rnd);
            int x = xz    & 15;
            int z = xz>>4 & 15;

            // absolute
            int ay = 1+LAB_Gen_Surface_Shape_Func(gen, x|cx<<4, z|cz<<4);
            if(ay >> 4 == cy && !LAB_Gen_Cave_Carve_Func(gen, x|cx<<4, ay-1, z|cz<<4)
                             && lyr->min_height <= ay && ay <= lyr->max_height)
            {
                LAB_Placer p2;
                p2.chunk = p->chunk;
                p2.ox = p->ox - x;
                p2.oy = p->oy - (ay&15);
                p2.oz = p->oz - z;

                lyr->structure_func(&p2, &rnd);
            }
        }
    }
}
#endif













LAB_STATIC void LAB_Gen_Cave(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    if(y <= -1)
    {
        // Underground Generation
        LAB_Gen_Cave_Carve(gen, chunk, x, y, z);
        //LAB_Gen_Cave_Crystals(gen, chunk, x, y, z);
        LAB_Gen_Cave_RockVariety(gen, chunk, x, y, z);
    }
}





LAB_DEPRECATED("old")
LAB_STATIC void LAB_Gen_Cave_Crystals(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, gen->seed^LAB_GEN_CRYSTAL_SALT, x, y, z);

    // Less probability that crystals generate at the top
    unsigned amount = LAB_MIN(2*LAB_MAX(0, -y), 100);

    if((LAB_NextRandom(&random)&255) < amount)
    {
        for(int i = LAB_NextRandom(&random)&15; i > 0; --i)
        {
            LAB_Block*const LIGHTS[4] = {
                &LAB_BLOCK_BLUE_CRYSTAL,
                &LAB_BLOCK_YELLOW_CRYSTAL,
                &LAB_BLOCK_GREEN_CRYSTAL,
                &LAB_BLOCK_RED_CRYSTAL,
            };

            LAB_Block* light = LIGHTS[LAB_NextRandom(&random)&3];
            int xx = LAB_NextRandom(&random) & 0xf;
            int zz = LAB_NextRandom(&random) & 0xf;
            int yy;
            for(yy = 0; yy < 16; ++yy)
                if(chunk->blocks[xx+16*yy+16*16*zz] == &LAB_BLOCK_STONE)
                    break;
            if(yy == 16) continue;

            int h = (LAB_NextRandom(&random)&3)+2;
            for(--yy; yy >= 0 && h > 0; --yy, --h)
            {
                chunk->blocks[xx+16*yy+16*16*zz] = light;
            }
        }
    }
}


LAB_STATIC void LAB_Gen_Cave_RockVariety(LAB_GenOverworld* gen, LAB_Chunk* chunk, int x, int y, int z)
{
    #if 0
    static uint64_t noise[17*17*17];
    static uint32_t smooth[16*16*16];

    LAB_ChunkNoise3D(noise, gen->seed^0x12345, x, y, z);
    LAB_SmoothNoise3D(smooth, noise);
    for(int zz = 0; zz < 16*16*16; zz+=16*16)
    for(int yy = 0; yy < 16*16;    yy+=16)
    for(int xx = 0; xx < 16;       xx++)
    {
        uint32_t n = smooth[xx|yy|zz];
        if(SMTST(n, 0x20000000, 0x80000000) && chunk->blocks[xx|yy|zz] == &LAB_BLOCK_STONE)
        {
            chunk->blocks[xx|yy|zz] = &LAB_BLOCK_MARBLE;
        }
    }
    #else
    for(int zz = 0; zz < 16; ++zz)
    for(int yy = 0; yy < 16; ++yy)
    for(int xx = 0; xx < 16; ++xx)
    {
        int xi = xx|x<<4;
        int yi = yy|y<<4;
        int zi = zz|z<<4;
        if(chunk->blocks[xx|yy<<4|zz<<8] == &LAB_BLOCK_STONE)
        {
            #define F (1.f/42.f)
            double n = LAB_SimplexNoise3D(xi*F, yi*F+gen->seed+0x12345, zi*F);
            //if(n >= 0.375 && n <= 0.625)
            //if(n <= 0.25)
            if(n <= 0.17)
                chunk->blocks[xx|yy<<4|zz<<8] = &LAB_BLOCK_MARBLE;
            //else if(n >= 0.25 && n <= 0.75)
            //else if(n >= 0.75)
            else if(n >= 0.83)
                chunk->blocks[xx|yy<<4|zz<<8] = &LAB_BLOCK_BASALT;
        }
    }
    #endif
}
