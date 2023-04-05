#include "LAB_game_surface_dimension_gen.h"

#define LAB_GEN_DIRT_SALT        0x12345
#define LAB_GEN_UNDERGROUND_SALT 0x54321

#define LAB_CAVE_ALTITUDE_C (2)
#define LAB_CAVE_ALTITUDE (16*LAB_CAVE_ALTITUDE_C)

/**
 * fills blocks corresponding to surface height
 */
LAB_STATIC
void LAB_SurfaceDimension_ShapeSurface(const LAB_SurfaceDimension* dim, uint64_t world_seed, LAB_Chunk_Blocks* chunk_blocks, int x, int y, int z);

LAB_STATIC
void LAB_SurfaceDimension_ShapeCave(const LAB_SurfaceDimension* dim, uint64_t world_seed, LAB_Chunk_Blocks* chunk_blocks, int x, int y, int z);

LAB_STATIC
void LAB_SurfaceDimension_PopulateStructures(const LAB_SurfaceDimension* dim, uint64_t world_seed, LAB_Chunk_Blocks* chunk_blocks, int x, int y, int z);

LAB_STATIC
void LAB_SurfaceDimension_PopulateBiomeStructures(const LAB_SurfaceDimension* dim, uint64_t world_seed,
    LAB_Placer* p,
    int sx, int sy, int sz, // position of span origin, in chunks
    size_t span_id, LAB_Game_StructureSpan span,
    LAB_SurfaceBiomeID biome_id, const LAB_Game_StructureArray* structures
);



bool LAB_SurfaceDimension_GenerateChunk_Func(const void* ctx, uint64_t world_seed, LAB_Chunk* chunk, int x, int y, int z)
{
    const LAB_SurfaceDimension* dim = ctx;

    LAB_Chunk_FillGenerate(chunk, LAB_BID_AIR);
    LAB_Chunk_Blocks* chunk_blocks = chunk->buf_blocks;

    LAB_SurfaceDimension_ShapeSurface(dim, world_seed, chunk_blocks, x, y, z);
    LAB_SurfaceDimension_ShapeCave(dim, world_seed, chunk_blocks, x, y, z);
    LAB_SurfaceDimension_PopulateStructures(dim, world_seed, chunk_blocks, x, y, z);

    return true;
}




LAB_STATIC
void LAB_SurfaceDimension_ShapeSurface(const LAB_SurfaceDimension* dim, uint64_t world_seed, LAB_Chunk_Blocks* chunk_blocks, int x, int y, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, world_seed^LAB_GEN_DIRT_SALT, x, y, z);

    for(int zz = 0; zz < 16; ++zz)
    for(int xx = 0; xx < 16; ++xx)
    {
        int xi = 16*x|xx;
        int zi = 16*z|zz;

        int river = 0;//LAB_Gen_River_Func(gen, xi, zi);
        int sheight = LAB_SurfaceDimension_SurfaceHeight(dim, world_seed, xi, zi);
        sheight -= river;
        for(int yy = 15; yy >= 0; --yy)
        {
            int yi = 16*y|yy;

            LAB_SurfaceBiomeID biome_id = LAB_SurfaceDimension_SurfaceBiome(dim, world_seed, xi, yi, zi, &random);
            const LAB_SurfaceBiome_Entry* biome = &dim->biomes[biome_id];
            const LAB_SurfaceBiome_Blocks* blocks = &biome->blocks;

            LAB_BlockID b = LAB_BID_AIR;
            if(yi < 0) b = blocks->ocean_block;

            //if(sheight < (int)(LAB_NextRandom(&random)&15) + 20)
            //if(sheight < (int)(LAB_NextRandom(&random)&31) + LAB_ROCKY_ALTITUDE)
            {
                if(yi == sheight && yi >= -1)
                    b = blocks->surface_block;
                else if(yi <= sheight)
                {
                    //uint64_t fact = 0x100000000ll/(32+16);
                    //if((~LAB_NextRandom(&random)>>32) >= (2u*(-yi)-(-sheight))*fact)
                    if(yi >= sheight-(int)(LAB_NextRandom(&random)&7)-1)
                    //if(yi >= sheight-2)
                        b = blocks->ground_block;
                    else
                        b = blocks->rock_block; // keep stone
                }
            }
            /*else if(yi <= sheight)
                //b = &LAB_BLOCK_CLAY.layered;
                b = blocks->rock_block; // keep stone*/

            chunk_blocks->blocks[LAB_CHUNK_OFFSET(xx, yy, zz)] = b;
        }
    }
}




LAB_STATIC
void LAB_SurfaceDimension_ShapeCave(const LAB_SurfaceDimension* dim, uint64_t world_seed, LAB_Chunk_Blocks* chunk_blocks, int x, int y, int z)
{
    
    if(y >= LAB_CAVE_ALTITUDE_C) return;

    const int floor_depth = 3;

    // Carve out caves
    for(int zz = 0; zz < 16; ++zz)
    for(int yy = 0; yy < 16; ++yy)
    for(int xx = 0; xx < 16; ++xx)
    {
        double xi = x*16|xx;
        double yi = y*16|yy;
        double zi = z*16|zz;
        int yt = LAB_SurfaceDimension_SurfaceHeight(dim, world_seed, xi, zi);
        if(yt < 0 && yi > yt-floor_depth) continue; // Carve out caves but with solid floor under water

        if(LAB_SurfaceDimension_IsCave(dim, world_seed, xi, yi, zi))
            chunk_blocks->blocks[xx|yy<<4|zz<<8] = LAB_BID_AIR;
    }
}




LAB_STATIC
void LAB_SurfaceDimension_PopulateStructures(const LAB_SurfaceDimension* dim, uint64_t world_seed, LAB_Chunk_Blocks* chunk_blocks, int x, int y, int z)
{
    //
    // - go through all span shapes
    //   - go through 3x3x3 neighborhood of spans
    //     - enumerate biomes in span
    //     - try to place any structure in span
    
    LAB_PlacementPriorityBuffer zbuf;
    LAB_PlacementPriorityBuffer_Create(&zbuf);

    LAB_LandscapeMask mask;
    {
        size_t i;
        LAB_LandscapeMask_Create_Expr(&mask, i, !(LAB_BlockP(chunk_blocks->blocks[i])->tags & LAB_BLOCK_TAG_REPLACEABLE));
    }

    LAB_SurfaceBiomeID biomes[64];
    for(size_t span_index = 0; span_index < dim->spans.count; ++span_index)
    {
        LAB_Game_StructureSpan span = dim->spans.data[span_index];

        int sx0 = LAB_FloorMultiplePow2(x, span.cx);
        int sy0 = LAB_FloorMultiplePow2(y, span.cy);
        int sz0 = LAB_FloorMultiplePow2(z, span.cz);

        for(int dz = -1, sz = sz0-span.cz; dz <= 1; dz++, sz += span.cz)
        for(int dy = -1, sy = sy0-span.cy; dy <= 1; dy++, sy += span.cy)
        for(int dx = -1, sx = sx0-span.cx; dx <= 1; dx++, sx += span.cx)
        {
            LAB_Random random;
            LAB_ChunkRandom(&random, world_seed, sx, sy, sz);

            LAB_Placer p;
            p.chunk_blocks = chunk_blocks;
            p.priority = &zbuf;
            p.landscape = &mask;
            p.ox = 16*(x-sx);
            p.oy = 16*(y-sy);
            p.oz = 16*(z-sz);

            size_t count = LAB_SurfaceDimension_EnumerateBiomes(
                dim, world_seed, biomes, LAB_LEN(biomes),
                sx*16, sy*16, sz*16,
                span.cx*16, span.cy*16, span.cz*16,
                &random
            );

            LAB_ASSERT(count >= 1);

            LAB_SurfaceDimension_PopulateBiomeStructures(dim, world_seed,
                &p,
                sx, sy, sz,
                span_index, span,
                LAB_SurfaceBiomeID_NULL, &dim->common_structures
            );

            for(size_t j = 0; j < count; ++j)
            {
                LAB_SurfaceBiomeID biome_id = biomes[j];
                LAB_SurfaceBiome_Entry* b = &dim->biomes[biome_id];
                LAB_SurfaceDimension_PopulateBiomeStructures(dim, world_seed,
                    &p,
                    sx, sy, sz,
                    span_index, span,
                    biome_id, &b->structures
                );
            }
        }
    }
}


LAB_STATIC
void LAB_SurfaceDimension_PopulateBiomeStructures(const LAB_SurfaceDimension* dim, uint64_t world_seed,
    LAB_Placer* p,
    int sx, int sy, int sz, // position of span origin
    size_t span_id, LAB_Game_StructureSpan span,
    LAB_SurfaceBiomeID biome_id, const LAB_Game_StructureArray* structures
)
{
    LAB_Random rnd;

    size_t start = structures->span_offsets[span_id];
    size_t stop = span_id + 1 == dim->spans.count
                ? structures->structures_count
                : structures->span_offsets[span_id+1];

    LAB_Game_Structure_Opt* sts = structures->structures;

    for(LAB_Game_Structure_Opt* s = sts+start; s < sts+stop; ++s)
    {
        LAB_ChunkRandom(&rnd, world_seed^s->salt, sx, sy, sz);

        LAB_Game_StructureDensity density = { .count = 0 };
        density = LAB_CALL_FUNC(&dim->structure_args, density, s->density, /**/ span, world_seed, &rnd, sx, sy, sz);

        for(int i = 0; i < density.count; ++i)
        {
            uint64_t xyz = LAB_NextRandom(&rnd);
            // absolute
            int ax = (xyz     & (16*span.cx-1)) | sx<<4;
            int ay = (xyz>>16 & (16*span.cy-1)) | sy<<4;
            int az = (xyz>>32 & (16*span.cz-1)) | sz<<4;

            if(biome_id != LAB_SurfaceBiomeID_NULL)
            {
                LAB_Gen_BiomeID biome_id2 = LAB_SurfaceDimension_SurfaceBiome(dim, world_seed, ax, ay, az, &rnd);
                if(biome_id2 != biome_id) continue;
            }

            if(!LAB_CALL_FUNC(&dim->structure_args, false, s->placement, /**/ world_seed, &ax, &ay, &az)) continue;

            if(LAB_FloorMultiplePow2(ax, 16*span.cx) != (sx<<4)) continue;
            if(LAB_FloorMultiplePow2(ay, 16*span.cy) != (sy<<4)) continue;
            if(LAB_FloorMultiplePow2(az, 16*span.cz) != (sz<<4)) continue;
    
            LAB_Placer p2 = LAB_Placer_Offset(p, ax-(sx<<4), ay-(sy<<4), az-(sz<<4));
            LAB_CALL_FUNC(&dim->structure_args, (void)0, s->structure, /**/ &p2, &rnd);
        }
    }
}