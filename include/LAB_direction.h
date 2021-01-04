#pragma once

#define LAB_DIR_WEST   1
#define LAB_DIR_EAST   2
#define LAB_DIR_DOWN   4
#define LAB_DIR_UP     8
#define LAB_DIR_NORTH 16
#define LAB_DIR_SOUTH 32

#define LAB_DIR_W  1
#define LAB_DIR_E  2
#define LAB_DIR_D  4
#define LAB_DIR_U  8
#define LAB_DIR_N 16
#define LAB_DIR_S 32

#define LAB_I_W  0
#define LAB_I_E  1
#define LAB_I_D  2
#define LAB_I_U  3
#define LAB_I_N  4
#define LAB_I_S  5

#define LAB_DIR_ALL 63



/**
 *  {-1, 0, 0},
 *  { 1, 0, 0},
 *  { 0,-1, 0},
 *  { 0, 1, 0},
 *  { 0, 0,-1},
 *  { 0, 0, 1},
 */
#define LAB_OFFSET_X(i) (int)(-!(i)+((i)==1))
#define LAB_OFFSET_Y(i) LAB_OFFSET_X((i)-2)
#define LAB_OFFSET_Z(i) LAB_OFFSET_X((i)-4)

#define LAB_OX(i) LAB_OFFSET_X(i)
#define LAB_OY(i) LAB_OFFSET_Y(i)
#define LAB_OZ(i) LAB_OFFSET_Z(i)

/**
 *  { 0, 1, 0 },
 *  { 0, 0, 1 },
 *  { 1, 0, 0 },
 */
#define LAB_AX(i) (int)((i)==2)
#define LAB_AY(i) (int)((i)==0)
#define LAB_AZ(i) (int)((i)==1)

/**
 *  { 0, 0, 1 },
 *  { 1, 0, 0 },
 *  { 0, 1, 0 },
 */
#define LAB_BX(i) (int)((i)==1)
#define LAB_BY(i) (int)((i)==2)
#define LAB_BZ(i) (int)((i)==0)


#define LAB_OUTWARDS_FACES(x0, y0, z0, x1, y1, z1) ( \
        ((x0) <= (x1)) << 0 \
      | ((x0) >= (x1)) << 1 \
      | ((y0) <= (y1)) << 2 \
      | ((y0) >= (y1)) << 3 \
      | ((z0) <= (z1)) << 4 \
      | ((z0) >= (z1)) << 5 \
)



#if 0
#include "LAB_bits.h"
// first version, not so good unrollable
#define LAB_DIR_EACH(faces, lval_face, ...) do \
{ \
    for(int LAB_DIR_EACH_faces = (faces); LAB_DIR_EACH_faces; LAB_DIR_EACH_faces&=LAB_DIR_EACH_faces-1) \
    { \
        (lval_face) = LAB_Ctz(LAB_DIR_EACH_faces); \
        {__VA_ARGS__} \
    } \
} while(0)
#else
/*#define LAB_DIR_EACH(faces, lval_face, ...) do \
{ \
    for((lval_face) = 0; (lval_face) < 6; ++(lval_face)) \
    { \
        if(1 << (lval_face) & (faces)) \
            {__VA_ARGS__} \
    } \
} while(0)*/
#include "LAB_opt.h"
#define LAB_DIR_EACH(faces, lval_face, ...) do \
{ \
    int LAB_DIR_EACH_faces = (faces); \
    LAB_UNROLL(6) \
    for(int LAB_DIR_EACH_face = 0; LAB_DIR_EACH_face < 6; ++LAB_DIR_EACH_face) \
    { \
        if(1 << LAB_DIR_EACH_face & LAB_DIR_EACH_faces) \
        { \
            (lval_face) = LAB_DIR_EACH_face; \
            {__VA_ARGS__} \
        } \
    } \
} while(0)

#endif
