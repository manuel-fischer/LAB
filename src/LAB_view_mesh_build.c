#include "LAB_view_mesh_build.h"

#include "LAB_debug.h"
#include "LAB_blocks.h" // LAB_BlockP
#include "LAB_vec.h"
#include "LAB_light_shading.h"

LAB_INLINE
bool LAB_View_Mesh_BuildBlock(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    const LAB_Block* LAB_RESTRICT block,
    LAB_Vec3I block_pos
);


LAB_HOT
bool LAB_View_Mesh_BuildChunk(LAB_View_Mesh_BuildArgs ctx)
{
    for(int i = 0; i < LAB_RENDER_PASS_COUNT; ++i)
        LAB_ASSERT(ctx.render_passes[i].m_size == 0);

    for(size_t z = 0; z < LAB_CHUNK_SIZE; ++z)
    for(size_t y = 0; y < LAB_CHUNK_SIZE; ++y)
    for(size_t x = 0; x < LAB_CHUNK_SIZE; ++x)
    {
        LAB_BlockID bid = ctx.blocknbh->bufs[LAB_NB_CENTER]->blocks[LAB_CHUNK_OFFSET(x, y, z)];
        const LAB_Block* LAB_RESTRICT b = LAB_BlockP(bid);
        if(b->flags & LAB_BLOCK_VISUAL)
            if(!LAB_View_Mesh_BuildBlock(&ctx, b, (LAB_Vec3I) { x, y, z })) return false;
    }
    return true;
}




LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_DirSet LAB_View_Mesh_GetVisibleFaces(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    const LAB_Block* block,
    LAB_Vec3I block_pos
)
{
    LAB_DirSet faces = 0;
    LAB_UNROLL(6)
    for(LAB_DirIndex i = 0; i < 6; ++i)
    {
        LAB_Vec3I opos = LAB_Vec3I_Add(block_pos, LAB_Vec3I_FromDirIndex(i));
        LAB_Block* b = LAB_BlockNbHood_GetBlockVP(ctx->blocknbh, opos);
        bool opaque = !!(b->flags&LAB_BLOCK_OPAQUE);
        bool opaque_self = !!(b->flags&LAB_BLOCK_OPAQUE_SELF);
        bool hide_face = opaque | ((b==block) & opaque_self);
        faces |= (int)!hide_face << i;
    }
    return faces;
}





/*LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_View_Mesh_GetFlatLightFace(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I block_pos, LAB_DirIndex face
)
{
    return LAB_GetVisualNeighborhoodLight(ctx->lightnbh, block_pos, face, ctx->cfg.exposure, ctx->cfg.saturation);
}*/

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_ColorHDR LAB_View_Mesh_GetFlatLightFaceHDR(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I block_pos, LAB_DirIndex face
)
{
    return LAB_GetVisualNeighborhoodLightHDR(ctx->lightnbh, block_pos, face);
}



///// Shading Computation /////

typedef struct LAB_View_Mesh_FlatShadeCube
{
    // Indexing scheme: face; use face=6 for light inside block
    LAB_ColorHDR sides[7];
} LAB_View_Mesh_FlatShadeCube;


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_View_Mesh_FlatShadeCube LAB_View_Mesh_GetFlatShadeCube(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I block_pos
)
{
    LAB_View_Mesh_FlatShadeCube result;

    int face;
    LAB_DIR_EACH(LAB_DIR_SET_ALL, face,
    {
        result.sides[face] = LAB_View_Mesh_GetFlatLightFaceHDR(ctx, LAB_Vec3I_AddDir(block_pos, face), face);
    });

    result.sides[6] = LAB_View_Mesh_GetFlatLightFaceHDR(ctx, block_pos, LAB_I_UP);

    return result;
}


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
uint32_t LAB_View_Mesh_FaceReachability(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I p, LAB_Vec3I a, LAB_Vec3I b
)
{
    uint32_t blocks_reachable_bits =       1<<1        /// B
                                     |1<<3|1<<4|1<<5|  /// |
                                           1<<7;       /// +---A

    // ends of plus shape: 1, 3, 5, 7
    LAB_UNROLL(4)
    for(LAB_DirIndex i = 1; i <= 7; i+=2)
    {
        LAB_Vec3I d = i==3||i==5 ? a : b;
        if(i < 4) d = LAB_Vec3I_Neg(d);

        LAB_Block* block = LAB_BlockNbHood_GetBlockVP(ctx->blocknbh, LAB_Vec3I_Add(p, d));
        bool opaque = block->flags&LAB_BLOCK_OPAQUE;

        uint32_t mask = i==1 ? (1<<0|1<<2)
                      : i==3 ? (1<<0|1<<6)
                      : i==5 ? (1<<2|1<<8)
                      :        (1<<6|1<<8);
        blocks_reachable_bits |= LAB_SELECT0(!opaque, mask);
    }

    return blocks_reachable_bits;
}


typedef struct LAB_View_Mesh_ShadeCube
{
    // Indexing scheme: face, cube corner; use face=6 for light inside block
    LAB_ColorHDR sides[7][8];
} LAB_View_Mesh_ShadeCube;

#define LAB_View_Mesh_GetShadeCube LAB_View_Mesh_GetShadeCube_Alt3

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_View_Mesh_ShadeCube LAB_View_Mesh_GetShadeCube_Alt0(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I block_pos
)
{
    LAB_View_Mesh_ShadeCube result;

    LAB_DirIndex face;
    LAB_DIR_EACH(LAB_DIR_SET_ALL, face,
    {
        LAB_Vec3I p = LAB_Vec3I_AddDir(block_pos, face);
        LAB_Vec3I a = LAB_Vec3I_Dir_OrthA(face);
        LAB_Vec3I b = LAB_Vec3I_Dir_OrthB(face);

        uint32_t blocks_reachable_bits = LAB_View_Mesh_FaceReachability(ctx, p, a, b);

        LAB_ColorHDR tmp[9];
        int index = 0;
        for(int v = -1; v <= 1; ++v)
        for(int u = -1; u <= 1; ++u, ++index)
        {
            // pos = block_pos + u*a + v*b
            LAB_Vec3I pos = LAB_Vec3I_Add(
                p,
                LAB_Vec3I_Add(
                    LAB_Vec3I_Mul(u, a),
                    LAB_Vec3I_Mul(v, b)
                )
            );

            bool reachable = blocks_reachable_bits & 1<<index;
            LAB_ColorHDR light = LAB_View_Mesh_GetFlatLightFaceHDR(ctx, pos, face);
            tmp[index] = LAB_SELECT0(reachable, light);
        }

        result.sides[face][0] = LAB_MixColorHDR4x25(tmp[0], tmp[1], tmp[3], tmp[4]);
        result.sides[face][1] = LAB_MixColorHDR4x25(tmp[1], tmp[2], tmp[4], tmp[5]);
        result.sides[face][2] = LAB_MixColorHDR4x25(tmp[3], tmp[4], tmp[6], tmp[7]);
        result.sides[face][3] = LAB_MixColorHDR4x25(tmp[4], tmp[5], tmp[7], tmp[8]);

        //result.sides[face][0] = LAB_MulColorHDR(result.sides[face][0], LAB_Float_To_ColorHDR(ctx->cfg.exposure));
        //result.sides[face][1] = LAB_MulColorHDR(result.sides[face][1], LAB_Float_To_ColorHDR(ctx->cfg.exposure));
        //result.sides[face][2] = LAB_MulColorHDR(result.sides[face][2], LAB_Float_To_ColorHDR(ctx->cfg.exposure));
        //result.sides[face][3] = LAB_MulColorHDR(result.sides[face][3], LAB_Float_To_ColorHDR(ctx->cfg.exposure));
    });

    result.sides[6][0] =
    result.sides[6][1] =
    result.sides[6][2] =
    result.sides[6][3] = LAB_View_Mesh_GetFlatLightFaceHDR(ctx, block_pos, LAB_I_UP);

    return result;
}


/*LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_MapColor(LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx, LAB_ColorHDR hc, int shift_factor)
{
    hc = LAB_MulColorHDRExp2(hc, shift_factor);
    LAB_Color c;
    c = LAB_LightToColor(hc, ctx->cfg.exposure);
    c = LAB_ColorSaturation(c, ctx->cfg.saturation);
    return c;
}*/


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_View_Mesh_ShadeCube LAB_View_Mesh_GetShadeCube_Alt1(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I block_pos
)
{
    LAB_View_Mesh_ShadeCube result;

    int face;
    LAB_DIR_EACH(LAB_DIR_SET_ALL, face,
    {
        LAB_Vec3I p = LAB_Vec3I_AddDir(block_pos, face);
        LAB_Vec3I a = LAB_Vec3I_Dir_OrthA(face);
        LAB_Vec3I b = LAB_Vec3I_Dir_OrthB(face);

        uint32_t blocks_reachable_bits = LAB_View_Mesh_FaceReachability(ctx, p, a, b);

        LAB_ColorHDR corners[4] = {0, 0, 0, 0};
        int corner_0 = !LAB_FACE_POSITIVE(face) << LAB_FACE_AXIS(face);
        int corner_a = a.x | a.y << 1 | a.z << 2;
        int corner_b = b.x | b.y << 1 | b.z << 2;

        LAB_Vec3I v0 = LAB_Vec3I_Sub(p, LAB_Vec3I_Add(a, b));

        int index = 0;
        for(int v = 0; v <= 1; ++v, ++index)
        for(int u = 0; u <= 1; ++u, ++index)
        {
            LAB_ColorHDR corner_color = 0;
            for(int dv = 0; dv <= 1; ++dv)
            for(int du = 0; du <= 1; ++du)
            {
                LAB_Vec3I d = LAB_Vec3I_Add(v0, LAB_Vec3I_Add(LAB_Vec3I_Mul(u+du, a), LAB_Vec3I_Mul(v+dv, b)));
                int corner = corner_0 | LAB_SELECT0(!du, corner_a) | LAB_SELECT0(!dv, corner_b);
                const LAB_LightNode* n = LAB_LightNbHood_RefLightNodeVC(ctx->lightnbh, d);
                LAB_ColorHDR c = n->quadrants[corner];

                /*LAB_Vec3I dA = LAB_Vec3I_Add(v0, LAB_Vec3I_Add(a, LAB_Vec3I_Mul(v+dv, b)));
                LAB_Vec3I dB = LAB_Vec3I_Add(v0, LAB_Vec3I_Add(b, LAB_Vec3I_Mul(u+du, a)));
                int cornerA = corner ^ corner_a;
                int cornerB = corner ^ corner_b;
                const LAB_LightNode* nA = LAB_LightNbHood_RefLightNodeVC(ctx->lightnbh, dA);
                const LAB_LightNode* nB = LAB_LightNbHood_RefLightNodeVC(ctx->lightnbh, dB);
                LAB_ColorHDR c2 = LAB_MaxColorHDR(nA->quadrants[cornerA], nB->quadrants[cornerB]);*/
                LAB_ColorHDR c2 = 0;

                int index2 = index + du + dv*3;
                corner_color = LAB_AddColorHDR(corner_color, LAB_SELECT(blocks_reachable_bits & 1<<index2, c, c2));
            }
            corners[u|v<<1] = corner_color;
        }

        //result.sides[face][0] = LAB_MapColor(ctx, corners[0], -2);
        //result.sides[face][1] = LAB_MapColor(ctx, corners[1], -2);
        //result.sides[face][2] = LAB_MapColor(ctx, corners[2], -2);
        //result.sides[face][3] = LAB_MapColor(ctx, corners[3], -2);
        result.sides[face][0] = LAB_MulColorHDRExp2(corners[0], -2);
        result.sides[face][1] = LAB_MulColorHDRExp2(corners[1], -2);
        result.sides[face][2] = LAB_MulColorHDRExp2(corners[2], -2);
        result.sides[face][3] = LAB_MulColorHDRExp2(corners[3], -2);

    });

    result.sides[6][0] =
    result.sides[6][1] =
    result.sides[6][2] =
    result.sides[6][3] = LAB_View_Mesh_GetFlatLightFaceHDR(ctx, block_pos, LAB_I_UP);

    return result;
}


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_View_Mesh_ShadeCube LAB_View_Mesh_GetShadeCube_Alt2(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I block_pos
)
{
    LAB_View_Mesh_ShadeCube result;

    int face;
    LAB_DIR_EACH(LAB_DIR_SET_ALL, face,
    {
        LAB_Vec3I p = LAB_Vec3I_AddDir(block_pos, face);
        LAB_Vec3I a = LAB_Vec3I_Dir_OrthA(face);
        LAB_Vec3I b = LAB_Vec3I_Dir_OrthB(face);

        uint32_t blocks_reachable_bits = LAB_View_Mesh_FaceReachability(ctx, p, a, b);

        LAB_ColorHDR corners[4] = {0, 0, 0, 0};
        int corner_0 = !LAB_FACE_POSITIVE(face) << LAB_FACE_AXIS(face);
        int corner_a = a.x | a.y << 1 | a.z << 2;
        int corner_b = b.x | b.y << 1 | b.z << 2;

        LAB_Vec3I v0 = LAB_Vec3I_Sub(p, LAB_Vec3I_Add(a, b));

        int index = 0;
        for(int v = 0; v <= 1; ++v, ++index)
        for(int u = 0; u <= 1; ++u, ++index)
        {
            LAB_ColorHDR corner_color = 0;
            for(int dv = 0; dv <= 1; ++dv)
            for(int du = 0; du <= 1; ++du)
            {
                LAB_Vec3I d = LAB_Vec3I_Add(v0, LAB_Vec3I_Add(LAB_Vec3I_Mul(u+du, a), LAB_Vec3I_Mul(v+dv, b)));
                int corner = corner_0 | LAB_SELECT0(!du, corner_a) | LAB_SELECT0(!dv, corner_b);
                const LAB_LightNode* n = LAB_LightNbHood_RefLightNodeVC(ctx->lightnbh, d);
                LAB_ColorHDR c = n->quadrants[corner];

                /*LAB_Vec3I dA = LAB_Vec3I_Add(v0, LAB_Vec3I_Add(a, LAB_Vec3I_Mul(v+dv, b)));
                LAB_Vec3I dB = LAB_Vec3I_Add(v0, LAB_Vec3I_Add(b, LAB_Vec3I_Mul(u+du, a)));
                int cornerA = corner ^ corner_a;
                int cornerB = corner ^ corner_b;
                const LAB_LightNode* nA = LAB_LightNbHood_RefLightNodeVC(ctx->lightnbh, dA);
                const LAB_LightNode* nB = LAB_LightNbHood_RefLightNodeVC(ctx->lightnbh, dB);
                LAB_ColorHDR c2 = LAB_MaxColorHDR(nA->quadrants[cornerA], nB->quadrants[cornerB]);*/
                LAB_ColorHDR c2 = 0;

                int index2 = index + du + dv*3;
                corner_color = LAB_MaxColorHDR(corner_color, LAB_SELECT(blocks_reachable_bits & 1<<index2, c, c2));
            }
            corners[u|v<<1] = corner_color;
        }

        result.sides[face][0] = corners[0];
        result.sides[face][1] = corners[1];
        result.sides[face][2] = corners[2];
        result.sides[face][3] = corners[3];

    });

    result.sides[6][0] =
    result.sides[6][1] =
    result.sides[6][2] =
    result.sides[6][3] = LAB_View_Mesh_GetFlatLightFaceHDR(ctx, block_pos, LAB_I_UP);

    return result;
}


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_View_Mesh_ShadeCube LAB_View_Mesh_GetShadeCube_Alt3(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_Vec3I block_pos
)
{
    LAB_View_Mesh_ShadeCube result;
    LAB_View_Mesh_ShadeCube a = LAB_View_Mesh_GetShadeCube_Alt0(ctx, block_pos);
    LAB_View_Mesh_ShadeCube b = LAB_View_Mesh_GetShadeCube_Alt2(ctx, block_pos);

    for(int i = 0; i < 6; ++i)
    for(int j = 0; j < 4; ++j)
    {
        //result.sides[i][j] = LAB_InterpolateColorHDR2i(a.sides[i][j], b.sides[i][j], 100);
        result.sides[i][j] = LAB_MixColorHDR50(a.sides[i][j], b.sides[i][j]);
    }
    return result;
}


///// End Shading Computation /////


///// Shading Modes /////

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
size_t LAB_View_Mesh_BuildBlock_NoShade(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_OUT LAB_Triangle* LAB_RESTRICT tri_preallocated,
    const LAB_Model* LAB_RESTRICT model,
    LAB_Vec3I block_pos,
    LAB_DirSet faces
)
{
    return LAB_PutModelAt(tri_preallocated, model, LAB_Vec3I2F(block_pos), faces, ctx->visibility);
}


LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
size_t LAB_View_Mesh_BuildBlock_FlatShade(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_OUT LAB_Triangle* LAB_RESTRICT tri_preallocated,
    const LAB_Model* LAB_RESTRICT model,
    LAB_Vec3I block_pos,
    LAB_DirSet faces
)
{
    LAB_View_Mesh_FlatShadeCube shade = LAB_View_Mesh_GetFlatShadeCube(ctx, block_pos);
    return LAB_PutModelShadedAt(tri_preallocated, model, LAB_Vec3I2F(block_pos), faces, ctx->visibility, shade.sides);
}

LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
size_t LAB_View_Mesh_BuildBlock_SmoothShade(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    LAB_OUT LAB_Triangle* LAB_RESTRICT tri_preallocated,
    const LAB_Model* LAB_RESTRICT model,
    LAB_Vec3I block_pos,
    LAB_DirSet faces
)
{
    LAB_View_Mesh_ShadeCube shade = LAB_View_Mesh_GetShadeCube(ctx, block_pos);
    return LAB_PutModelSmoothShadedAt(tri_preallocated, model, LAB_Vec3I2F(block_pos), faces, ctx->visibility, (const LAB_ColorHDR(*)[8])shade.sides);
}

///// End Shading Modes /////



LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
bool LAB_View_Mesh_BuildBlock(
    LAB_View_Mesh_BuildArgs* LAB_RESTRICT ctx,
    const LAB_Block* LAB_RESTRICT block,
    LAB_Vec3I block_pos
)
{
    const LAB_Model* LAB_RESTRICT model = block->model;
    if(model == NULL) return true;

    int faces = LAB_View_Mesh_GetVisibleFaces(ctx, block, block_pos);
    if(faces == 0) return true;



    LAB_ASSERT(model->render_pass < LAB_RENDER_PASS_COUNT);
    LAB_View_Mesh* mesh = &ctx->render_passes[model->render_pass];

    LAB_Triangle* tri = LAB_View_Mesh_Alloc(mesh, LAB_Model_TriangleCount(model));
    if(LAB_UNLIKELY(tri == NULL)) return false;

    size_t added;

    if(block->flags & LAB_BLOCK_NOSHADE)
        added = LAB_View_Mesh_BuildBlock_NoShade(ctx, tri, model, block_pos, faces);
    else if((ctx->cfg.flat_shade) || (block->flags&LAB_BLOCK_FLAT_SHADE))
        added = LAB_View_Mesh_BuildBlock_FlatShade(ctx, tri, model, block_pos, faces);
    else
        added = LAB_View_Mesh_BuildBlock_SmoothShade(ctx, tri, model, block_pos, faces);

    mesh->m_size -= LAB_Model_TriangleCount(model)-added;

    return true;
}