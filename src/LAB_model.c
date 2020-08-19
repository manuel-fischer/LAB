#include "LAB_model.h"

#define NO_MMINTRIN // Not ready yet
#ifndef NO_MMINTRIN
#include <mmintrin.h>
#endif

void LAB_PutModelAt(LAB_OUT LAB_Triangle* dst, LAB_Model* model,
                    float x, float y, float z, unsigned faces)
{
    LAB_Triangle* src = model->data;
    LAB_Triangle* src_end = src + model->size;

    for(; src < src_end; ++src, ++dst)
    {
        unsigned occlusion = LAB_TRIANGLE_CULL(*src);
        if(occlusion==0 || (occlusion&faces))
        {
            memcpy(dst, src, sizeof *src);
            dst->v[0].x += x; dst->v[0].y += y; dst->v[0].z += z;
            dst->v[1].x += x; dst->v[1].y += y; dst->v[1].z += z;
            dst->v[2].x += x; dst->v[2].y += y; dst->v[2].z += z;
        }
    }
}
