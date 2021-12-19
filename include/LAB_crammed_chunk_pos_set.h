#pragma once

#include <stdint.h>
#include <stdbool.h>
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
LAB_CCPS LAB_CCPS_Pos(int x, int y, int z)
{
    LAB_ASSUME((x &~ 15)==0);
    LAB_ASSUME((y &~ 15)==0);
    LAB_ASSUME((z &~ 15)==0);

    LAB_CCPS bits = 0;

    // add x, y, z coords
    bits |= (1ull<< 0) << x;
    bits |= (1ull<<16) << y;
    bits |= (1ull<<32) << z;

    // add hashtable entry
    bits |= (1ull<<48) << LAB_CCPS_PosHash(x, y, z);

    return bits;
}

/*LAB_PURE LAB_INLINE
LAB_CCPS LAB_CCPS_AddPos(LAB_CCPS set, int x, int y, int z)
{
    set |= LAB_CCPS_Pos(x, y, z);
    return set;
}*/

LAB_PURE LAB_INLINE
bool LAB_CCPS_HasPos(LAB_CCPS set, int x, int y, int z)
{
    LAB_ASSUME((x &~ 15)==0);
    LAB_ASSUME((y &~ 15)==0);
    LAB_ASSUME((z &~ 15)==0);

    LAB_CCPS mask = LAB_CCPS_Pos(x, y, z);
    return (set & mask) == mask; // all four bits set -> true
}

LAB_PURE LAB_INLINE
int LAB_CCPS_Faces(LAB_CCPS set)
{
    // which outer faces are touched?
    // check outer column bits and generate face set

    return (set&          1ull     )
         | (set&(1ull<<15|1ull<<16)) >> (15-1)
         | (set&(1ull<<31|1ull<<32)) >> (31-3)
         | (set&(1ull<<47         )) >> (47-5);
}

// return 27 bit bitset
LAB_PURE LAB_INLINE
int LAB_CCPS_Neighborhood(LAB_CCPS set)
{
    int bits = 0;
    int i = 0;
    LAB_UNROLL(3)
    for(int z = 0; z < 3; ++z)
    LAB_UNROLL(3)
    for(int y = 0; y < 3; ++y)
    LAB_UNROLL(3)
    for(int x = 0; x < 3; ++x, ++i)
    {
        LAB_CCPS mask = ((LAB_CCPS)(x==0)<<0 | (LAB_CCPS)(x==2)<<15) <<  0
                      | ((LAB_CCPS)(y==0)<<0 | (LAB_CCPS)(y==2)<<15) << 16
                      | ((LAB_CCPS)(z==0)<<0 | (LAB_CCPS)(z==2)<<15) << 32;
        bits |= (int)((set&mask)==mask) << i;
    }
    LAB_ASSERT(i == 27);
    LAB_ASSERT(bits & 1 << (1+3+9));
    if(!set)
    {
        //bits &= ~(1 << (1+3+9));
        bits -= 1 << (1+3+9);
    }
    return bits;
}

#define LAB_CCPS_EACH_POS(set, x, y, z, ...) do \
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
                if(LAB_CCPS_HasPos(LAB_CCPS_set, LAB_CCPS_x, LAB_CCPS_y, LAB_CCPS_z)) \
                { \
                    (x)=LAB_CCPS_x; \
                    (y)=LAB_CCPS_y; \
                    (z)=LAB_CCPS_z; \
                    {__VA_ARGS__} \
                } \
            } \
        } \
    } \
} \
while(0)





#define LAB_CCPS_YIELD(i) do { LAB_CCPS_npos = i; goto LAB_CCPS_LABEL(execute); case i:; } while(0)

#define LAB_CCPS_LABEL__(id, line) id##_##line
#define LAB_CCPS_LABEL_(id, line) LAB_CCPS_LABEL__(id, line)
#define LAB_CCPS_LABEL(id) LAB_CCPS_LABEL_(LAB_CCPS_LABEL_##id, __LINE__)

// iterate over inserted elements and neighboring elements, (each position yielded at most once)
// (if one position is in the set then 7 positions are yielded)
// x, y, z lvalues, gets assigned a value in [-1, 16]
#define LAB_CCPS_EACH_NEAR_POS(set, x, y, z, ...) do \
{ \
    LAB_CCPS LAB_CCPS_EACH_NEAR_POS_set = (set); \
    int LAB_CCPS_pos = 0; \
    while(1) \
    { \
        int LAB_CCPS_npos; \
        switch(LAB_CCPS_pos) \
        { \
        case 0: \
            for((z)=0; (z)<16; ++(z)) \
            for((y)=0; (y)<16; ++(y)) \
            for((x)=0; (x)<16; ++(x)) \
            { \
                if( \
                                 LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set,   (x), (y), (z))  || \
                   ((x) !=  0 && LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x)-1, (y), (z))) || \
                   ((x) != 15 && LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x)+1, (y), (z))) || \
                   ((y) !=  0 && LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x), (y)-1, (z))) || \
                   ((y) != 15 && LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x), (y)+1, (z))) || \
                   ((z) !=  0 && LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x), (y), (z)-1)) || \
                   ((z) != 15 && LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x), (y), (z)+1))    \
                ) \
                    LAB_CCPS_YIELD(1); \
            } \
            for((z)=0; (z)<16; ++(z)) \
            for((y)=0; (y)<16; ++(y)) \
            { \
                if(LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set,  0, (y), (z))) { (x) = -1; LAB_CCPS_YIELD(2); } \
                if(LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, 15, (y), (z))) { (x) = 16; LAB_CCPS_YIELD(3); } \
            } \
            for((z)=0; (z)<16; ++(z)) \
            for((x)=0; (x)<16; ++(x)) \
            { \
                if(LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x),  0, (z))) { (y) = -1; LAB_CCPS_YIELD(4); } \
                if(LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x), 15, (z))) { (y) = 16; LAB_CCPS_YIELD(5); } \
            } \
            for((y)=0; (y)<16; ++(y)) \
            for((x)=0; (x)<16; ++(x)) \
            { \
                if(LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x), (y),  0)) { (z) = -1; LAB_CCPS_YIELD(6); } \
                if(LAB_CCPS_HasPos(LAB_CCPS_EACH_NEAR_POS_set, (x), (y), 15)) { (z) = 16; LAB_CCPS_YIELD(7); } \
            } \
            goto LAB_CCPS_LABEL(exit); \
        } \
        LAB_CCPS_LABEL(execute):; \
        {__VA_ARGS__} \
        LAB_CCPS_pos = LAB_CCPS_npos; \
    } \
    LAB_CCPS_LABEL(exit):; \
} \
while(0)

// TODO
// Iterate over all 27 neighboring blocks
//#define LAB_CCPS_EACH_NEAR_DIAG_POS
