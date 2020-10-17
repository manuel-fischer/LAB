#include "LAB_model.h"
#include "LAB_bits.h"

LAB_STATIC int LAB_FaceSetToLightIndex(int face)
{
    //return face ? LAB_Ctz(face) : 6;
    return LAB_Ctz(face|0x40);
}

LAB_HOT
int LAB_PutModelAt(LAB_OUT LAB_Triangle*restrict dst, LAB_Model const* model,
                   float x, float y, float z, unsigned faces, unsigned visibility)
{
    int count = 0;
    LAB_Triangle* src = model->data;
    LAB_Triangle* src_end = src + model->size;

    for(; src < src_end; ++src)
    {
        unsigned occlusion = LAB_TRIANGLE_CULL(*src);
        unsigned tri_visibility = LAB_TRIANGLE_VISIBILITY(*src);
        if((tri_visibility&visibility) && (occlusion&faces))
        {
            // main loop: 45 instructions
            //memcpy(dst, src, sizeof *src);
            *dst = *src;
            dst->v[0].x += x; dst->v[0].y += y; dst->v[0].z += z;
            dst->v[1].x += x; dst->v[1].y += y; dst->v[1].z += z;
            dst->v[2].x += x; dst->v[2].y += y; dst->v[2].z += z;
            // main loop: 48 instructions
            //dst->v[0].x = src->v[0].x+x; dst->v[0].y = src->v[0].y+y; dst->v[0].z = dst->v[0].z+z;
            //dst->v[1].x = src->v[1].x+x; dst->v[1].y = src->v[1].y+y; dst->v[1].z = dst->v[1].z+z;
            //dst->v[2].x = src->v[2].x+x; dst->v[2].y = src->v[2].y+y; dst->v[2].z = dst->v[2].z+z;
            //dst->v[0].color = src->v[0].color; dst->v[0].u = src->v[0].u; dst->v[0].v = src->v[0].v;
            //dst->v[1].color = src->v[1].color; dst->v[1].u = src->v[1].u; dst->v[1].v = src->v[1].v;
            //dst->v[2].color = src->v[2].color; dst->v[2].u = src->v[2].u; dst->v[2].v = src->v[2].v;
            ++dst, ++count;
        }
    }
    return count;
}

LAB_HOT
int LAB_PutModelShadedAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                         float x, float y, float z, unsigned faces, unsigned visibility,
                         const LAB_Color light_sides[6])
{
    int count = 0;
    LAB_Triangle* src = model->data;
    LAB_Triangle* src_end = src + model->size;

    for(; src < src_end; ++src)
    {
        unsigned occlusion = LAB_TRIANGLE_CULL(*src);
        unsigned tri_visibility = LAB_TRIANGLE_VISIBILITY(*src);
        if((tri_visibility&visibility) && (occlusion&faces))
        {
            //memcpy(dst, src, sizeof *src);
            *dst = *src;
            dst->v[0].x += x; dst->v[0].y += y; dst->v[0].z += z;
            dst->v[1].x += x; dst->v[1].y += y; dst->v[1].z += z;
            dst->v[2].x += x; dst->v[2].y += y; dst->v[2].z += z;
            unsigned light = LAB_FaceSetToLightIndex(LAB_TRIANGLE_LIGHT(*src));
            // Nightvision: LAB_OversaturateColor light_sides[light]
            dst->v[0].color = LAB_MulColor_Fast(dst->v[0].color, light_sides[light]);
            dst->v[1].color = LAB_MulColor_Fast(dst->v[1].color, light_sides[light]);
            dst->v[2].color = LAB_MulColor_Fast(dst->v[2].color, light_sides[light]);
            ++dst, ++count;
        }
    }
    return count;
}


LAB_HOT
int LAB_PutModelSmoothShadedAt(LAB_OUT LAB_Triangle* dst,
                               LAB_Model const* model,
                               float x, float y, float z, unsigned faces, unsigned visibility,
                               const LAB_Color light_sides[6][4])
{
    int count = 0;
    LAB_Triangle* src = model->data;
    LAB_Triangle* src_end = src + model->size;

    for(; src < src_end; ++src)
    {
        unsigned occlusion = LAB_TRIANGLE_CULL(*src);
        unsigned tri_visibility = LAB_TRIANGLE_VISIBILITY(*src);
        if((tri_visibility&visibility) && (occlusion&faces))
        {
            //memcpy(dst, src, sizeof *src);
            *dst = *src;
            dst->v[0].x += x; dst->v[0].y += y; dst->v[0].z += z;
            dst->v[1].x += x; dst->v[1].y += y; dst->v[1].z += z;
            dst->v[2].x += x; dst->v[2].y += y; dst->v[2].z += z;
            unsigned light = LAB_FaceSetToLightIndex(LAB_TRIANGLE_LIGHT(*src));
            // Nightvision: LAB_OversaturateColor light_sides[light]
            for(int i = 0; i < 3; ++i)
            {
                int u, v;
                int li = light>>1;
                static const uint8_t iota3[] = {0, 1, 2, 0, 1, 2};
                int ui = iota3[li+1];
                int vi = iota3[li+2];
                u = (int)(256*(&src->v[i].x)[ui]);
                v = (int)(256*(&src->v[i].x)[vi]);

                const LAB_Color* l = light_sides[light];
                LAB_Color lint = LAB_InterpolateColor4vi(l, u, v);
                dst->v[i].color = LAB_MulColor_Fast(dst->v[i].color, lint);
            }
            ++dst, ++count;
        }
    }
    return count;
}
