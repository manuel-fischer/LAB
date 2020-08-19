#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"


/**
 *  the flags field of the first vertex of a triangle
 *  contains the faces to cull
 *  otherwise flags works as padding,
 */
typedef struct LAB_Vertex   //     Size       Align
{                           //-----------------------
    float x, y, z;          //   3*4 Bytes   4 Bytes  \_ could be used as 4 floats
    uint8_t r, g, b, a;     //     4 Bytes   1 Byte   /  -> 16 Byte == 128 bit (vector intrinsics)
                            //
    float u, v;             //   2*4 Bytes   4 Bytes
    uint64_t flags;         //     8 Bytes   8 Bytes
}                           //-----------------------
LAB_Vertex;                 //    32 Bytes   8 Bytes

typedef struct LAB_Triangle //     Size
{                           //-------------
    LAB_Vertex v[3];        //  3*32 Bytes
}                           //-------------
LAB_Triangle;               //    96 Bytes

#define LAB_TRIANGLE_CULL(triangle) ((triangle).v[0].flags)
#define LAB_TRIANGLE_LIGHT(triangle) ((triangle).v[1].flags)

typedef struct LAB_Model
{
    size_t size;
    LAB_Triangle data[];
} LAB_Model;



/**
 *  The dst-array should have space for the whole model,
 *  - if the number of triangles is known, the space could be
 *    smaller but sufficient to the number of triangles
 */
void LAB_PutModelAt(LAB_OUT LAB_Triangle* dst, LAB_Model* model,
                    float x, float y, float z, unsigned faces);
