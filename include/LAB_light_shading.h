#pragma once

#include "LAB_light_node.h"
#include "LAB_chunk_neighborhood.h"
#include <math.h>


/*#define LAB_LIGHT_HDR_TO_RGBA(c) \
    LAB_ColorHDR_To_Color_Overflow((c), LAB_HDR_RGB_F(0.009f, 0.01f, 0.008f))*/
/*#define LAB_LightToColor(c) \
    LAB_ColorHDR_To_Color_Overflow((c), 0)*/

LAB_INLINE LAB_CONST
LAB_Color LAB_LightToColor(LAB_ColorHDR c, float exposure)
{
    #define LAB_LightToColor_MapChannel(ch) \
        ((ch) > 1e-25f ? log2(ch)*32.f/exposure+255.f : 0.f)

    float rf = LAB_LightToColor_MapChannel(LAB_HDR_RED_VAL(c));
    float gf = LAB_LightToColor_MapChannel(LAB_HDR_GRN_VAL(c));
    float bf = LAB_LightToColor_MapChannel(LAB_HDR_BLU_VAL(c));

    int r = LAB_CLAMP(rf+LAB_MAX((gf+bf-2.f)*0.09f, 0), 0, 255);
    int g = LAB_CLAMP(gf+LAB_MAX((bf+rf-2.f)*0.10f, 0), 0, 255);
    int b = LAB_CLAMP(bf+LAB_MAX((rf+gf-2.f)*0.08f, 0), 0, 255);
    return LAB_RGB(r, g, b);
}




LAB_HOT LAB_ALWAYS_INLINE LAB_INLINE
LAB_Color LAB_GetVisualNeighborhoodLight(LAB_LightNbHood* n, int x, int y, int z, LAB_Dir face, float exposure)
{
    LAB_LightNode* ln = LAB_LightNbHood_RefLightNode(n, x, y, z);

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

    //c = LAB_MulColorHDR_RoundUp(c, LAB_Float_To_ColorHDR(exposure));
    //max = LAB_MulColorHDR_RoundUp(max, LAB_Float_To_ColorHDR(exposure));

    c = LAB_LightToColor(c, exposure);
    max = LAB_LightToColor(max, exposure);

    return c;
}