#include "LAB_model.h"
#include "LAB_bits.h"

int LAB_PutModelAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                   float x, float y, float z, unsigned faces)
{
    int count = 0;
    LAB_Triangle* src = model->data;
    LAB_Triangle* src_end = src + model->size;

    for(; src < src_end; ++src)
    {
        unsigned occlusion = LAB_TRIANGLE_CULL(*src);
        if(occlusion&faces)
        {
            memcpy(dst, src, sizeof *src);
            dst->v[0].x += x; dst->v[0].y += y; dst->v[0].z += z;
            dst->v[1].x += x; dst->v[1].y += y; dst->v[1].z += z;
            dst->v[2].x += x; dst->v[2].y += y; dst->v[2].z += z;
            ++dst, ++count;
        }
    }
    return count;
}

int LAB_PutModelShadedAt(LAB_OUT LAB_Triangle* dst, LAB_Model const* model,
                         float x, float y, float z, unsigned faces,
                         LAB_Color light_sides[6])
{
    int count = 0;
    LAB_Triangle* src = model->data;
    LAB_Triangle* src_end = src + model->size;

    for(; src < src_end; ++src)
    {
        unsigned occlusion = LAB_TRIANGLE_CULL(*src);
        if(occlusion&faces)
        {
            memcpy(dst, src, sizeof *src);
            dst->v[0].x += x; dst->v[0].y += y; dst->v[0].z += z;
            dst->v[1].x += x; dst->v[1].y += y; dst->v[1].z += z;
            dst->v[2].x += x; dst->v[2].y += y; dst->v[2].z += z;
            unsigned light = LAB_Ctz(LAB_TRIANGLE_LIGHT(*src));
            // Nightvision: LAB_OversaturateColor light_sides[light]
            dst->v[0].color = LAB_MulColor_Fast(dst->v[0].color, light_sides[light]);
            dst->v[1].color = LAB_MulColor_Fast(dst->v[1].color, light_sides[light]);
            dst->v[2].color = LAB_MulColor_Fast(dst->v[2].color, light_sides[light]);
            ++dst, ++count;
        }
    }
    return count;
}


int LAB_PutModelSmoothShadedAt(LAB_OUT LAB_Triangle* dst,
                               LAB_Model const* model,
                               float x, float y, float z, unsigned faces,
                               LAB_Color light_sides[6][4])
{
    int count = 0;
    LAB_Triangle* src = model->data;
    LAB_Triangle* src_end = src + model->size;

    for(; src < src_end; ++src)
    {
        unsigned occlusion = LAB_TRIANGLE_CULL(*src);
        if(occlusion&faces)
        {
            memcpy(dst, src, sizeof *src);
            dst->v[0].x += x; dst->v[0].y += y; dst->v[0].z += z;
            dst->v[1].x += x; dst->v[1].y += y; dst->v[1].z += z;
            dst->v[2].x += x; dst->v[2].y += y; dst->v[2].z += z;
            unsigned light = LAB_Ctz(LAB_TRIANGLE_LIGHT(*src));
            // Nightvision: LAB_OversaturateColor light_sides[light]
            for(int i = 0; i < 3; ++i)
            {
                #if 0
                float u, v;
                u = LAB_offsetA[light>>1][0]*src->v[i].x +
                    LAB_offsetA[light>>1][1]*src->v[i].y +
                    LAB_offsetA[light>>1][2]*src->v[i].z;
                v = LAB_offsetB[light>>1][0]*src->v[i].x +
                    LAB_offsetB[light>>1][1]*src->v[i].y +
                    LAB_offsetB[light>>1][2]*src->v[i].z;
                #else
                int u, v;
                u = LAB_offsetA[light>>1][0]*(int)(256*src->v[i].x) +
                    LAB_offsetA[light>>1][1]*(int)(256*src->v[i].y) +
                    LAB_offsetA[light>>1][2]*(int)(256*src->v[i].z);
                v = LAB_offsetB[light>>1][0]*(int)(256*src->v[i].x) +
                    LAB_offsetB[light>>1][1]*(int)(256*src->v[i].y) +
                    LAB_offsetB[light>>1][2]*(int)(256*src->v[i].z);
                #endif
                //u = v = 0;

                LAB_Color* l = light_sides[light];
                #if 0
                LAB_Color lint = LAB_InterpolateColor4vf(l, u, v);
                #else
                LAB_Color lint = LAB_InterpolateColor4vi(l, u, v);
                #endif
                dst->v[i].color = LAB_MulColor_Fast(dst->v[i].color, lint);
            }
            ++dst, ++count;
        }
    }
    return count;
}
