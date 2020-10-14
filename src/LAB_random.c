#include "LAB_random.h"

#include "LAB_attr.h"


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
    #if 0
    s ^= (uint64_t)x * 23u;
    s ^= (uint64_t)y * 17u;
    s ^= (uint64_t)z * 26u;
    #else
    s += (uint64_t)x * 123456811u;
    s += (uint64_t)y * 234567899u;
    s += (uint64_t)z * 214365871u;
    s += ((uint64_t)x * (uint64_t)y * (uint64_t)z)*127u;
    #endif
    return s;
}

void LAB_ChunkRandom(LAB_OUT LAB_Random* random, uint64_t world_seed, int x, int y, int z)
{
    LAB_SetRandom(random, LAB_ChunkSeed(world_seed, x, y, z));
}

#if 0
void LAB_ChunkNoise2D(LAB_OUT uint64_t noise[17*17], uint64_t world_seed, int cx, int cz)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, world_seed, cx, 0, cz);

    for(int x = 0; x < 16; ++x)
        noise[x] = LAB_NextRandom(&random);

    for(int z = 17; z < 16*17; z+=17)
        noise[z] = LAB_NextRandom(&random);

    for(int z = 17; z < 16*17; z+=17)
        for(int x = 1; x < 16; ++x)
            noise[x+z] = LAB_NextRandom(&random);


    LAB_ChunkRandom(&random, world_seed, cx+1, 0, cz);
    noise[16] = LAB_NextRandom(&random);
    for(int x = 1; x < 16; ++x)
        LAB_NextRandom(&random);

    for(int z = 17; z < 16*17; z+=17)
        noise[16+z] = LAB_NextRandom(&random);


    LAB_ChunkRandom(&random, world_seed, cx, 0, cz+1);
    for(int x = 0; x < 16; ++x)
        noise[16*17+x] = LAB_NextRandom(&random);


    LAB_ChunkRandom(&random, world_seed, cx+1, 0, cz+1);
    noise[16*17+16] = LAB_NextRandom(&random);
}


LAB_HOT LAB_ALWAYS_INLINE
LAB_STATIC void LAB_ChunkNoise3D_Sub(LAB_OUT uint64_t* noise, uint64_t world_seed,
                                 int cx, int cy, int cz,
                                 int rx, int ry, int rz)
{
    LAB_Random random;
    LAB_ChunkRandom(&random, world_seed, cx, cy, cz);

    noise[0] = LAB_NextRandom(&random);


    if(!rx)
    {
        for(int x = 1; x < 16; ++x)
            noise[x] = LAB_NextRandom(&random);
    }
    else //if(!ry || !rz)
    {
        for(int x = 1; x < 16; ++x)
            LAB_NextRandom(&random);
    }



    if(!ry)
    {
        for(int y = 17; y < 16*17; y+=17)
            noise[y] = LAB_NextRandom(&random);
    }
    else //if(!rx || !rz)
    {
        for(int y = 17; y < 16*17; y+=17)
            LAB_NextRandom(&random);
    }



    if(!rz)
    {
        for(int z = 17*17; z < 16*17*17; z+=17*17)
            noise[z] = LAB_NextRandom(&random);
    }
    else //if(!rx && !ry)
    {
        for(int z = 17*17; z < 16*17*17; z+=17*17)
            LAB_NextRandom(&random);
    }





    if(!rx && !ry)
    {
        for(int y = 17; y < 16*17; y+=17)
            for(int x = 1; x < 16; ++x)
                noise[x+y] = LAB_NextRandom(&random);
    }
    else //if((!ry && !rz) || (!ry && !rz))
    {
        for(int y = 17; y < 16*17; y+=17)
            for(int x = 1; x < 16; ++x)
                LAB_NextRandom(&random);
    }



    if(!ry && !rz)
    {
        for(int z = 17*17; z < 16*17*17; z+=17*17)
            for(int x = 1; x < 16; ++x)
                noise[x+z] = LAB_NextRandom(&random);
    }
    else //if(!ry && !rz)
    {
        for(int z = 17*17; z < 16*17*17; z+=17*17)
            for(int x = 1; x < 16; ++x)
                LAB_NextRandom(&random);
    }



    if(!ry && !rz)
    {
        for(int z = 17*17; z < 16*17*17; z+=17*17)
            for(int y = 17; y < 16*17; y+=17)
                noise[y+z] = LAB_NextRandom(&random);
    }
    //else
    //    for(int z = 17*17; z < 16*17*17; z+=17*17)
    //        for(int y = 17; y < 16*17; y+=17)
    //            LAB_NextRandom(&random);






    if(!rx && !ry && !rz)
        for(int z = 17*17; z < 16*17*17; z+=17*17)
            for(int y = 17; y < 16*17; y+=17)
                for(int x = 1; x < 16; ++x)
                    noise[x+y+z] = LAB_NextRandom(&random);
}


void LAB_ChunkNoise3D(LAB_OUT uint64_t noise[17*17*17], uint64_t world_seed, int cx, int cy, int cz)
{
    //for(int i = 0; i < 8; ++i)
    for(int i = 7; i >= 0; --i)
    {
        int off = 16*(i&1) + 17*16*(!!(i&2)) + 17*17*16*(!!(i&4));
        LAB_ChunkNoise3D_Sub(noise+off, world_seed,
                             cx+!!(i&1), cy+!!(i&2), cz+!!(i&4),
                             !!(i&1), !!(i&2), !!(i&4));
    }
}
#endif
