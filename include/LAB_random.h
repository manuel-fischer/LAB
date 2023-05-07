#pragma once

#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_debug.h"
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
 *  Advance Random and return random value
 */
uint64_t LAB_NextRandom(LAB_INOUT LAB_Random* random);

/**
 *  Get current random value
 */
uint64_t LAB_GetRandom(LAB_INOUT LAB_Random* random);


/**
 *  Advance Random and return random value in range [min, max)
 */
#define LAB_NextRandomRange(random, min, max) (LAB_NextRandom(random) % ((max)-(min))  +  (min))



/**
 *  Get chunk seed
 */
uint64_t LAB_ChunkSeed(uint64_t world_seed, int x, int y, int z);

/**
 *  Initialize chunk random
 */
void LAB_ChunkRandom(LAB_OUT LAB_Random* random, uint64_t world_seed, int x, int y, int z);

#if 0
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
#endif



typedef struct LAB_RandomBits64
{
    uint64_t bits;
    #ifndef NDEBUG
    int remaining_bits;
    #endif
} LAB_RandomBits64;

LAB_INLINE
LAB_RandomBits64 LAB_RandomBits64_Make(LAB_Random* random)
{
    return (LAB_RandomBits64) {
        .bits = LAB_GetRandom(random),
        #ifndef NDEBUG
        .remaining_bits = 64,
        #endif
    };
}

LAB_INLINE
uint64_t LAB_RandomBits64_Next(LAB_RandomBits64* bits, int count)
{
    uint64_t mask = ((uint64_t)1u << count)-1u;
    uint64_t result = bits->bits & mask;
    bits->bits >>= count;
    #ifndef NDEBUG
    bits->remaining_bits -= count;
    LAB_ASSERT(bits->remaining_bits >= 0);
    #endif
    return result;
}
