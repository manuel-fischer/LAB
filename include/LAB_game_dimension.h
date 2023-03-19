#pragma once

#include "LAB_stdinc.h"
#include "LAB_poly_buffer.h"
#include "LAB_blocks.h"
#include "LAB_chunk.h"

/**
 * Specifies that the game content is changed by the function
 */
#define LAB_MUT_Game_Dimension



typedef size_t LAB_Gen_BiomeID;

typedef struct LAB_Gen_Biome
{
    uint64_t tags;
    const LAB_BlockID surface_block;
    const LAB_BlockID ground_block;


} LAB_Gen_Biome;

typedef struct LAB_Gen_Dimension
{
    LAB_PolyBuffer structure_args;
} LAB_Gen_Dimension;

//void LAB_Gen_Dimension_AddStructure();

typedef struct LAB_IDimension
{
    void* ctx;
    /**
     * Might be NULL
     */
    void (*destroy)(void* ctx);
    /**
     * Function to be called, when initialization is complete, this
     * function might validate internal state and might make it immutable.
     * 
     * Might be NULL
     */
    bool (*finish_init)(void* ctx);
    /**
     * Should not be NULL
     */
    bool (*generate_chunk)(const void* ctx, uint64_t world_seed, LAB_Chunk* chunk, int x, int y, int z);
    /**
     * Should not be NULL
     */
    void (*spawn_point)(const void* ctx, uint64_t world_seed, double* x, double* y, double* z);
    // sky renderering, etc...
} LAB_IDimension;



bool LAB_Dimension_Init(void);
void LAB_Dimension_Quit(void);

LAB_IDimension* LAB_Dimension_GetDefault(void);

bool LAB_Dimension_Register(const LAB_IDimension* dim);
bool LAB_Dimension_FinishInit(void);