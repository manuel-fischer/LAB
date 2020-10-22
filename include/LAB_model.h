#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_render_pass.h"
#include "LAB_color.h"

// TODO setting to switch between triangle rendering and quad rendering

/* Triangle occlusion (culling) -- TODO
 *
 * 32[     24[     16[      8[      0[
 * 0b00000000DDDDDDCCCCCCBBBBBBAAAAAA
 *
 * Groups
 * - A: selectors of     solid   faces, those are combined with OR (default true)
 * - B: selectors of not solid   faces, those are combined with OR (default true)
 * - C: selectors of     similar faces, those are combined with OR (default true)
 * - D: selectors of not similar faces, those are combined with OR (default true)
 * each group combined with AND
 *
 * If a bitset group has no bits set in the vertex data, the resulting truth value is true
 * NOTE: if there are no solid faces, the model is not rendered (see LAB_view.c)
 *
 *
 * combining a selection of a bitset with a binary op can be done easily in C
 *
 * Selecting bits for an operation is done as follows
 *  _____
 *  \    |
 *   \
 *    OP  bits   =  (bits & sel)
 *   /        i
 *  /____|
 *  i ∊ sel
 *
 *
 *  - AND (default true)
 *      /\
 *     /  \
 *    /    \   bits   =  (bits & sel) == sel
 *   /      \      i
 *  /        \
 *    i ∊ sel
 *
 *  - OR (default false)
 *  \        /
 *   \      /
 *    \    /   bits   =  (bits & sel) != 0
 *     \  /        i
 *      \/
 *    i ∊ sel
 *
 *  - OR (default true) -> LAB_BITOR_TRUE
 *  \        /
 *   \      /
 *    \    /   bits   =  sel == 0 || (bits & sel) != 0
 *     \  /        i
 *      \/
 *    i ∊ sel
 *
 */
#define LAB_CULL_SELECTOR(sel_solid, sel_not_solid, sel_similar, sel_not_similar) \
            ((sel_solid) | (sel_not_solid) << 6 | (sel_similar) << 12 | (sel_not_similar) << 18)
#define LAB_CULL_BITS(solid, similar) \
            ((solid) | (similar) << 6)
#define LAB_BITOR_TRUE(bits, selector) \
            ((selector) == 0 | ((bits) & (selector)) != 0)
#define LAB_CULL_CHECK(bits, selector) \
            (LAB_BITOR_TRUE((bits)    & 077, (selector)     & 077) & \
             LAB_BITOR_TRUE((bits)    & 077, (selector)>> 6 & 077) & \
             LAB_BITOR_TRUE((bits)>>6 & 077, (selector)>>12 & 077) & \
             LAB_BITOR_TRUE((bits)>>6 & 077, (selector)>>18 & 077))

/**
 *  the flags field of the first vertex of a triangle
 *  contains the faces to cull
 *  otherwise flags works as padding,
 */
typedef struct LAB_Vertex   //     Size       Align
{                           //-----------------------
    float x, y, z;          //   3*4 Bytes   4 Bytes  \_ could be used as 4 floats
    LAB_Color color;        //     4 Bytes   4 Bytes  /  -> 16 Byte == 128 bit (vector intrinsics)
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
// the midpoint between [0] and [1] is used
// to compute the distance to the camera

#define LAB_TRIANGLE_CULL(triangle)       ((triangle).v[0].flags)
#define LAB_TRIANGLE_LIGHT(triangle)      ((triangle).v[1].flags)
#define LAB_TRIANGLE_VISIBILITY(triangle) ((triangle).v[2].flags)

typedef struct LAB_Model
{
    size_t size;
    LAB_Triangle* data;
    LAB_RenderPass render_pass;
} LAB_Model;



/**
 *  The dst-array should have space for the whole model,
 *  - if the number of triangles is known, the space could be
 *    smaller but sufficient to the number of triangles
 *  RETURN: the number of triangles written
 */
int LAB_PutModelAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                   float x, float y, float z, unsigned faces, unsigned visibility);

int LAB_PutModelShadedAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                         float x, float y, float z, unsigned faces, unsigned visibility,
                         const LAB_Color light_sides[6]);

int LAB_PutModelSmoothShadedAt(LAB_OUT LAB_Triangle* dst,
                               LAB_Model const* model,
                               float x, float y, float z, unsigned faces, unsigned visibility,
                               const LAB_Color light_sides[6][4]);
