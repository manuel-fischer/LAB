#include "LAB_model.h"
#include "LAB_bits.h"

#include "LAB_memory.h"



void LAB_Model_Destroy(LAB_Model* m)
{
    LAB_ARRAY_DESTROY(LAB_Model_QuadsArray(m));
}

LAB_ModelQuad* LAB_Model_Extend(LAB_Model* m, size_t num_tris)
{
    return LAB_ARRAY_APPEND_SOME(LAB_Model_QuadsArray(m), num_tris);
}








LAB_STATIC int LAB_FaceSetToLightIndex(int face)
{
    //return face ? LAB_Ctz(face) : 6;
    return LAB_Ctz(face|0x40);
}

LAB_INLINE LAB_ALWAYS_INLINE
void LAB_ShiftTriangle_InPlace(LAB_Triangle* t, LAB_Vec3F pos)
{
    t->v[0].pos = LAB_Vec3F_Add(t->v[0].pos, pos);
    t->v[1].pos = LAB_Vec3F_Add(t->v[1].pos, pos);
    t->v[2].pos = LAB_Vec3F_Add(t->v[2].pos, pos);
}

LAB_HOT LAB_INLINE LAB_ALWAYS_INLINE
void LAB_PutQuadAt(LAB_OUT LAB_Triangle dst[restrict 2], LAB_ModelQuad* quad, LAB_Vec3F pos)
{
    dst[0].v[0] = LAB_ModelQuad_MakeVertex(quad, 1, pos);
    dst[0].v[1] = LAB_ModelQuad_MakeVertex(quad, 2, pos);
    dst[0].v[2] = LAB_ModelQuad_MakeVertex(quad, 0, pos);

    dst[1].v[0] = dst[0].v[1];
    dst[1].v[1] = dst[0].v[0];
    //dst[1].v[0] = LAB_ModelQuad_MakeVertex(quad, 2, pos);
    //dst[1].v[1] = LAB_ModelQuad_MakeVertex(quad, 1, pos);
    dst[1].v[2] = LAB_ModelQuad_MakeVertex(quad, 3, pos);
}

LAB_HOT LAB_INLINE LAB_ALWAYS_INLINE
void LAB_PutQuadShadedAt(LAB_OUT LAB_Triangle dst[restrict 2], LAB_ModelQuad* quad, LAB_Vec3F pos, LAB_ColorHDR c)
{
    dst[0].v[0] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 1, pos, c);
    dst[0].v[1] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 2, pos, c);
    dst[0].v[2] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 0, pos, c);

    dst[1].v[0] = dst[0].v[1];
    dst[1].v[1] = dst[0].v[0];
    //dst[1].v[0] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 2, pos, c);
    //dst[1].v[1] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 1, pos, c);
    dst[1].v[2] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 3, pos, c);
}

LAB_HOT LAB_INLINE LAB_ALWAYS_INLINE
void LAB_PutQuadSmoothShadedAt(LAB_OUT LAB_Triangle dst[restrict 2], LAB_ModelQuad* quad, LAB_Vec3F pos, const LAB_ColorHDR c[restrict 4])
{
    LAB_ColorHDR_Comparable d03 = LAB_ColorHDR_LuminanceComparable(LAB_AbsDiffColorHDR(c[0], c[3]));
    LAB_ColorHDR_Comparable d12 = LAB_ColorHDR_LuminanceComparable(LAB_AbsDiffColorHDR(c[1], c[2]));
    int flip = d03 < d12;

    //float l0 = LAB_ColorHDR_Luminance(c[0]);
    //float l1 = LAB_ColorHDR_Luminance(c[1]);
    //float l2 = LAB_ColorHDR_Luminance(c[2]);
    //float l3 = LAB_ColorHDR_Luminance(c[3]);
    //int flip = fabsf(l0-l3) < fabsf(l1-l2);

    // For sorting the vertices in a triangle at [0] and [1] need to correspond
    // to opposing corners in the quad
    dst[0].v[0^flip] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 1^flip, pos, c[1^flip]);
    dst[0].v[1^flip] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 2^flip, pos, c[2^flip]);
    dst[0].v[2     ] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 0^flip, pos, c[0^flip]);

    dst[1].v[0] = dst[0].v[1];
    dst[1].v[1] = dst[0].v[0];
    //dst[1].v[0^flip] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 2^flip, pos, c[2^flip]);
    //dst[1].v[1^flip] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 1^flip, pos, c[1^flip]);
    dst[1].v[2     ] = LAB_ModelQuad_MakeVertexShadedHDR(quad, 3^flip, pos, c[3^flip]);
}


LAB_HOT
int LAB_PutModelAt(LAB_OUT LAB_Triangle*restrict dst, LAB_Model const* model,
                   LAB_Vec3F pos, unsigned faces, unsigned visibility)
{
    int count = 0;
    LAB_ModelQuad* src = model->quads;
    LAB_ModelQuad* src_end = src + model->quad_count;

    LAB_UNROLL(6)
    for(; src < src_end; ++src)
    {
        if((src->visibility&visibility) && (src->cull&faces))
        {
            LAB_PutQuadAt(dst, src, pos);
            dst += 2;
            count += 2;
        }
    }
    return count;
}

LAB_HOT
int LAB_PutModelShadedAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                         LAB_Vec3F pos, unsigned faces, unsigned visibility,
                         const LAB_ColorHDR light_sides[6])
{
    int count = 0;
    LAB_ModelQuad* src = model->quads;
    LAB_ModelQuad* src_end = src + model->quad_count;

    LAB_UNROLL(6)
    for(; src < src_end; ++src)
    {
        if((src->visibility&visibility) && (src->cull&faces))
        {
            unsigned light = LAB_FaceSetToLightIndex(src->light);
            LAB_PutQuadShadedAt(dst, src, pos, light_sides[light]);
            dst += 2;
            count += 2;
        }
    }
    return count;
}


LAB_HOT
int LAB_PutModelSmoothShadedAt(LAB_OUT LAB_Triangle* dst,
                               LAB_Model const* model,
                               LAB_Vec3F pos, unsigned faces, unsigned visibility,
                               const LAB_ColorHDR light_sides[6][8])
{
    int count = 0;
    LAB_ModelQuad* src = model->quads;
    LAB_ModelQuad* src_end = src + model->quad_count;

    LAB_UNROLL(6)
    for(; src < src_end; ++src)
    {
        if((src->visibility&visibility) && (src->cull&faces))
        {
            unsigned light = LAB_FaceSetToLightIndex(src->light);

            LAB_ColorHDR corners[4];
            LAB_UNROLL(4)
            for(int i = 0; i < 4; ++i)
            {
                int u, v;
                int li = light>>1;
                static const uint8_t iota3[] = {0, 1, 2, 0, 1, 2};
                int ui = iota3[li+1];
                int vi = iota3[li+2];
                u = (int)(256*LAB_Vec3F_Get(src->v[i].pos, ui));
                v = (int)(256*LAB_Vec3F_Get(src->v[i].pos, vi));

                const LAB_ColorHDR* l = light_sides[light];
                LAB_ColorHDR lint = LAB_InterpolateColorHDR4vi(l, u, v);
                corners[i] = lint;
            }

            LAB_PutQuadSmoothShadedAt(dst, src, pos, corners);
            dst += 2;
            count += 2;
        }
    }
    return count;
}
