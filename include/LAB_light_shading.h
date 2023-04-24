#pragma once

#include "LAB_light_node.h"
#include "LAB_chunk_neighborhood.h"
#include <math.h>
#include "LAB_attr.h"
#include "LAB_opt.h"


/*#define LAB_LIGHT_HDR_TO_RGBA(c) \
    LAB_ColorHDR_To_Color_Overflow((c), LAB_HDR_RGB_F(0.009f, 0.01f, 0.008f))*/
/*#define LAB_LightToColor(c, exposure) \
    LAB_ColorHDR_To_Color_Overflow((c), 0)*/

// TODO: remove, implemented by shader now
LAB_INLINE LAB_CONST
LAB_Color LAB_LightToColor(LAB_ColorHDR c, float exposure)
{
    /*#define LAB_LightToColor_MapChannel(ch) \
        ((ch) > 1e-25f ? (log2(ch)*64.f)/exposure+256.f : 0.f)*/

    #define LAB_LightToColor_MapChannel(ch) \
        ((ch) > 1e-25f ? pow(ch, 1/exposure) : 0.f)

    float rf = LAB_LightToColor_MapChannel(LAB_HDR_RED_VAL(c));
    float gf = LAB_LightToColor_MapChannel(LAB_HDR_GRN_VAL(c));
    float bf = LAB_LightToColor_MapChannel(LAB_HDR_BLU_VAL(c));

    int r = LAB_CLAMP(255.f*(rf+LAB_MAX((gf+bf-2.f)*0.09f, 0)), 0, 255);
    int g = LAB_CLAMP(255.f*(gf+LAB_MAX((bf+rf-2.f)*0.10f, 0)), 0, 255);
    int b = LAB_CLAMP(255.f*(bf+LAB_MAX((rf+gf-2.f)*0.08f, 0)), 0, 255);
    return LAB_RGB(r, g, b);
}




LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_ColorHDR LAB_GetVisualNeighborhoodLightHDR(const LAB_LightNbHood* LAB_RESTRICT n, LAB_Vec3I pos, LAB_DirIndex face)
{
    const LAB_LightNode* ln = LAB_LightNbHood_RefLightNodeVC(n, pos);

    LAB_ColorHDR c = 0;
    LAB_ColorHDR max = 0;

    int mask =        1  << (face>>1);
    int bit  = !(face&1) << (face>>1);
    LAB_UNROLL(8)
    for(int i = 0; i < 8; ++i)
    {
        LAB_ColorHDR cf = ln->quadrants[i];
        if((i & mask) != bit)
        {
            cf = LAB_MixColorHDR50(cf, 0);
        }
        max = LAB_MaxColorHDR(max, cf);
        c = LAB_AddColorHDR(c, LAB_MulColorHDRExp2(cf, 2));
    }

    return max;
}


// TODO: remove
LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetVisualNeighborhoodLight(const LAB_LightNbHood* LAB_RESTRICT n, LAB_Vec3I pos, LAB_DirIndex face, float exposure, float saturation)
{
    LAB_ColorHDR hdr = LAB_GetVisualNeighborhoodLightHDR(n, pos, face);

    hdr = LAB_MulColorHDR_RoundUp(hdr, LAB_Float_To_ColorHDR(exposure));

    LAB_Color c = LAB_LightToColor(c, exposure);

    c = LAB_ColorSaturation(c, saturation);

    return c;
}
