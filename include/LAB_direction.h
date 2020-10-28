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

#define LAB_DIR_ALL 63

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
