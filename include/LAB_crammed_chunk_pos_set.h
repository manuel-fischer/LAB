#pragma once

#include <stdint.h>
#include "LAB_attr.h"
#include "LAB_bits.h"
#include "LAB_debug.h"

// the set is divided into 4 parts, each 16 bit (hi2lo)
// [hashtable][z-coords][y-coords][x-coords]
//
// the set might contain additional positions in favor of less consumed memory
typedef uint64_t LAB_CrammedChunkPosSet, LAB_CCPS;

// hash function that maps x, y, z coordinates each in the range
// [0, 16) to an hash index in range [0, 16)
// so that when only a component of the coordinate vector is changed
// the resulting hash value is never the same
// near values could be the same, because there is a high probability
// that those coordinates are also added
LAB_PURE LAB_INLINE
LAB_CCPS LAB_CCPS_PosHash(int x, int y, int z)
{
    LAB_ASSUME((x &~ 15)==0);
    LAB_ASSUME((y &~ 15)==0);
    LAB_ASSUME((z &~ 15)==0);
    return (x+y+z) & 15;
}

LAB_PURE LAB_INLINE
LAB_CCPS LAB_CCPS_AddPos(LAB_CCPS set, int x, int y, int z)
{
    LAB_ASSUME((x &~ 15)==0);
    LAB_ASSUME((y &~ 15)==0);
    LAB_ASSUME((z &~ 15)==0);

    // add x, y, z coords
    set |= (1ull<< 0) << x;
    set |= (1ull<<16) << y;
    set |= (1ull<<32) << z;

    // add hashtable entry
    set |= (1ull<<48) << LAB_CCPS_PosHash(x, y, z);
}

#define LAB_CCPS_EACH_POS(set, x, y, z, execute) do \
{ \
    int LAB_CCPS_x_set, LAB_CCPS_y_set, LAB_CCPS_z_set; \
    int LAB_CCPS_x, LAB_CCPS_y, LAB_CCPS_z; \
    uint64_t LAB_CCPS_set = (set); \
    int LAB_CCPS_hashtbl = LAB_CCPS_set>>48; \
    for(int LAB_CCPS_z_set = LAB_CCPS_set>>32 & 0xffff; LAB_CCPS_z_set; LAB_CCPS_z_set &= LAB_CCPS_z_set-1) \
    { \
        LAB_CCPS_z = LAB_Ctz(LAB_CCPS_z_set); \
        for(int LAB_CCPS_y_set = LAB_CCPS_set>>16 & 0xffff; LAB_CCPS_y_set; LAB_CCPS_y_set &= LAB_CCPS_y_set-1) \
        { \
            LAB_CCPS_y = LAB_Ctz(LAB_CCPS_y_set); \
            for(int LAB_CCPS_x_set = LAB_CCPS_set     & 0xffff; LAB_CCPS_x_set; LAB_CCPS_x_set &= LAB_CCPS_x_set-1) \
            { \
                LAB_CCPS_x = LAB_Ctz(LAB_CCPS_x_set); \
                if(LAB_CCPS_set & 1<<LAB_CCPS_PosHash(LAB_CCPS_x, LAB_CCPS_y, LAB_CCPS_z)) \
                { \
                    (x)=LAB_CCPS_x; \
                    (y)=LAB_CCPS_y; \
                    (z)=LAB_CCPS_z; \
                    {execute} \
                } \
            } \
        } \
    } \
} \
while(0)
