#pragma once

#include "LAB_opt.h"
#include "LAB_stdinc.h"

// xor-shift
typedef struct LAB_Random
{
    uint64_t a, b;
} LAB_Random;

/**
 *  Initialize random state with a given seed
 */
void LAB_SetRandom(LAB_OUT LAB_Random* random, uint64_t seed);

/**
 *  Advance Random and return value
 */
uint64_t LAB_NextRandom(LAB_INOUT LAB_Random* random);

/**
 *  Get current random value
 */
uint64_t LAB_GetRandom(LAB_INOUT LAB_Random* random);





/**
 *  Get chunk seed
 */
uint64_t LAB_ChunkSeed(uint64_t world_seed, int x, int y, int z);

/**
 *  Initialize chunk random
 */
void LAB_ChunkRandom(LAB_OUT LAB_Random* random, uint64_t world_seed, int x, int y, int z);

/**
 *  Initialize chunk noise
 *  The array is filled with random values, that can be used
 *  for continuous noise like perlin noise
 */
void LAB_ChunkNoise2D(LAB_OUT uint64_t randoms[17*17], uint64_t world_seed, int x, int z);

/**
 *  Initialize chunk noise
 *  The array is filled with random values, that can be used
 *  for continuous noise like perlin noise
 */
void LAB_ChunkNoise3D(LAB_OUT uint64_t randoms[17*17*17], uint64_t world_seed, int x, int y, int z);
