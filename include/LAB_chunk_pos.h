#pragma once

#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_check.h"

typedef struct LAB_ChunkPos
{
    int16_t x, y, z;
    LAB_PAD(int16_t);
    //int32_t x, y, z;
    //LAB_PAD(int32_t);
} LAB_ChunkPos;
LAB_CHECK_STRUCT_SIZE(LAB_ChunkPos);

LAB_ALWAYS_INLINE
LAB_INLINE unsigned LAB_ChunkPosHash(LAB_ChunkPos pos)
{
    //return 0;
    return ((unsigned)pos.x*257u + (unsigned)pos.y*8191u + (unsigned)pos.y*65537u)*3u;
    //return (unsigned)pos.x*257 + (unsigned)pos.y*8191 + (unsigned)pos.y*65537;
    //return (unsigned)pos.x*7 + (unsigned)pos.y*13 + (unsigned)pos.y*19;
    /*return (unsigned)pos.x
         ^ (unsigned)pos.y << (unsigned)6 ^ pos.y << 4
         ^ (unsigned)pos.z << (unsigned)2 ^ pos.z << 7;*/

    unsigned v = 0x12345678
               ^ (unsigned)pos.x
               ^ (unsigned)pos.y << 4
               ^ (unsigned)pos.z << 7;
    v ^= v >> 3;
    v ^= v >> 5;
    v ^= v >> 17;
    //v ^= v*v;
    return v;
}

LAB_ALWAYS_INLINE
LAB_INLINE int LAB_ChunkPosComp(LAB_ChunkPos a, LAB_ChunkPos b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
    //return (a.x != b.x) | (a.y != b.y) | (a.z != b.z);
    //return memcmp(&a, &b, sizeof a)!=0;
    /*LAB_ChunkPos tmp_a, tmp_b;
    tmp_a=a; tmp_b=b;
    return memcmp(&tmp_a, &tmp_b, sizeof a)!=0;*/
}