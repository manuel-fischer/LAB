#pragma once
#include "LAB_stdinc.h"
#include "LAB_opt.h"
#include "LAB_render_pass.h"
#include "LAB_color.h"
#include "LAB_color_hdr.h"
#include "LAB_vec.h"
#include "LAB_array.h"
#include "LAB_util.h"

// TODO setting to switch between triangle rendering and quad rendering

/* Triangle occlusion (culling) -- TODO
 *
 * 32[     24[     16[      8[      0[
 * 0bJJJJJJJJDDDDDDCCCCCCBBBBBBAAAAAA
 *
 * Groups
 * - A: selectors of     solid   faces, those are combined with AND (default true)
 * - B: selectors of not solid   faces, those are combined with AND (default true)
 * - C: selectors of     similar faces, those are combined with AND (default true)
 * - D: selectors of not similar faces, those are combined with AND (default true)
 * each group combined with AND
 * - J: jump/skip amount, allows to skip multiple triangles, the triangles are only skipped
 *      if the triangle was not culled
 *
 * Previously the bits in a group were combined with OR, it makes more sence to use
 * AND here, it allows to enable a triangle only for a specific case
 *
 * The jump amount allows to simplify the models and allows to only keep one triangle if
 * its visible from two faces
 *
 * TODO: make dependent on all 27 neighboring blocks instead of just 6
 *       -> allows for connected models and overlays
 * TODO: add layer property to triangle, triangles in a chunk are sorted by this property
 *       by iteratively rendering models with the triangles on the first layer, then the
 *       second etc..
 *       - multiple render passes with the same blending behavior
 *       -> allows for overlays
 *
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
/* NOP triangle
 * - set visible faces to 0, the face is never copied but jumping is possible
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

typedef struct LAB_ModelQuadVertex
{
    LAB_Vec3F pos;
    LAB_Vec2F tex;
} LAB_ModelQuadVertex;
typedef struct LAB_ModelQuad
{
    LAB_ModelQuadVertex v[4];
    LAB_Color color;

    uint64_t cull;
    uint64_t visibility;
    uint64_t light;
    //LAB_Vec3F light_normal;
} LAB_ModelQuad;


/**
 *  the flags field of the first vertex of a triangle
 *  contains the faces to cull
 *  otherwise flags works as padding,
 */
typedef struct LAB_Vertex   //     Size       Align
{                           //-----------------------
    LAB_Vec3F pos;          //   3*4 Bytes   4 Bytes  \_ could be used as 4 floats
    uint32_t  padding;      //     4 Bytes   4 Bytes  /  -> 16 Byte == 128 bit (vector intrinsics)
                            //
    LAB_Vec2F tex;          //   2*4 Bytes   4 Bytes
    LAB_Color color;        //     4 Bytes   4 Bytes
    LAB_ColorHDR light;     //     4 Bytes   4 Bytes
}                           //-----------------------
LAB_Vertex;                 //    32 Bytes   4 Bytes

typedef struct LAB_Triangle //     Size
{                           //-------------
    LAB_Vertex v[3];        //  3*32 Bytes
}                           //-------------
LAB_Triangle;               //    96 Bytes
// the midpoint between [0] and [1] is used
// to compute the distance to the camera

#define LAB_ModelQuad_MakeVertex(pmq, i, shift_pos) \
    ((LAB_Vertex) { .pos=LAB_Vec3F_Add((pmq)->v[i].pos, shift_pos), .padding=0, .tex=(pmq)->v[i].tex, .color=(pmq)->color, .light=0xffffffff })

#define LAB_ModelQuad_MakeVertexShadedHDR(pmq, i, shift_pos, shade) \
    ((LAB_Vertex) { .pos=LAB_Vec3F_Add((pmq)->v[i].pos, shift_pos), .padding=0, (pmq)->v[i].tex, .color=(pmq)->color, .light=(shade) })


typedef struct LAB_Model
{
    size_t quad_count;
    LAB_ModelQuad* quads;
    #define LAB_Model_QuadsArray(pmodel) (LAB_ModelQuad, (pmodel)->quads, (pmodel)->quad_count)
    #define LAB_Model_TriangleCount(pmodel) ((pmodel)->quad_count*2)

    LAB_RenderPass render_pass;
} LAB_Model;






#define LAB_Model_Create(model) (LAB_ObjClear(model), true)
void LAB_Model_Destroy(LAB_Model* model);
LAB_ModelQuad* LAB_Model_Extend(LAB_Model* model, size_t num_tris);






/**
 *  The dst-array should have space for the whole model,
 *  - if the number of triangles is known, the space could be
 *    smaller but sufficient to the number of triangles
 *  RETURN: the number of triangles written
 */
int LAB_PutModelAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                   LAB_Vec3F pos, unsigned faces, unsigned visibility);

int LAB_PutModelShadedAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                         LAB_Vec3F pos, unsigned faces, unsigned visibility,
                         const LAB_Color light_sides[6]);

int LAB_PutModelSmoothShadedAt(LAB_OUT LAB_Triangle* dst,
                               LAB_Model const* model,
                               LAB_Vec3F pos, unsigned faces, unsigned visibility,
                               const LAB_Color light_sides[6][8]);
