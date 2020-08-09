#include "LAB_random.h"


void LAB_SetRandom(LAB_OUT LAB_Random* random, uint64_t seed)
{
    random->a = (seed & 0x00000000ffffffffll) | 0x0123456700000000ll;
    random->b = (seed & 0xffffffff00000000ll) | 0x0000000089abcdefll;
}


uint64_t LAB_NextRandom(LAB_INOUT LAB_Random* random)
{
    uint64_t s, t;
	t = random->a;
	s = random->b;

	random->a = s;
	t ^= t << 23;		// a
	t ^= t >> 17;		// b
	t ^= s ^ (s >> 26);	// c
	random->b = t;
	return t + s;
}

uint64_t LAB_GetRandom(LAB_IN LAB_Random* random)
{
    return random->a + random->b;
}




uint64_t LAB_ChunkSeed(uint64_t world_seed, int x, int y, int z)
{
    uint64_t s;
    s  = world_seed;
    s ^= x * 23;
    s ^= y * 17;
    s ^= z * 26;
    return s;
}

void LAB_ChunkRandom(LAB_OUT LAB_Random* random, uint64_t world_seed, int x, int y, int z)
{
    LAB_SetRandom(random, LAB_ChunkSeed(world_seed, x, y, z));
}

void LAB_ChunkNoise2D(LAB_OUT uint64_t noise[17*17], uint64_t world_seed, int x, int z)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, world_seed, x, 0, z);

    for(int x = 0; x < 16; ++x)
        noise[x] = LAB_NextRandom(&random);

    for(int z = 17; z < 16*17; z+=17)
        noise[z] = LAB_NextRandom(&random);

    for(int z = 17; z < 16*17; z+=17)
        for(int x = 1; x < 16; ++x)
            noise[x+z] = LAB_NextRandom(&random);


    LAB_ChunkRandom(&random, world_seed, x+1, 0, z);
    noise[16] = LAB_NextRandom(&random);
    for(int x = 1; x < 16; ++x)
        LAB_NextRandom(&random);

    for(int z = 17; z < 16*17; z+=17)
        noise[16+z] = LAB_NextRandom(&random);


    LAB_ChunkRandom(&random, world_seed, x, 0, z+1);
    for(int x = 0; x < 16; ++x)
        noise[16*17+x] = LAB_NextRandom(&random);


    LAB_ChunkRandom(&random, world_seed, x+1, 0, z+1);
    noise[16*17+16] = LAB_NextRandom(&random);
}
