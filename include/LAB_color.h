#pragma once

#include <stdlib.h>

typedef uint32_t LAB_Color;

#if 1
#define LAB_RGB(r, g, b) ((r) << 16 | (g) << 8 | (b) | 0xff000000)
#define LAB_RED(col) ((col) >> 16 & 0xff)
#define LAB_GRN(col) ((col) >>  8 & 0xff)
#define LAB_BLU(col) ((col)       & 0xff)
#else
#error RGB-Format not implemented
#endif


static inline LAB_Color LAB_MaxColor(LAB_Color a, LAB_Color b)
{
    #if 0
    LAB_Color  g_mask = ((a&0xff00)-(b&0xff00)) >> 8 & 0xff;
    LAB_Color rb_mask = ((((a&0xff00ff)|0x1000)-(b&0xff00ff)) >> 8 & 0x1)*0xff;

    return a ^ ((a^b) & (rb_mask|g_mask));
    #else
    int ar, ag, ab, br, bg, bb;
    ar = LAB_RED(a); ag = LAB_GRN(a); ab = LAB_BLU(a);
    br = LAB_RED(b); bg = LAB_GRN(b); bb = LAB_BLU(b);
    return LAB_RGB(ar<br?br:ar, ag<bg?bg:ag, ab<bb?bb:ab);
    #endif
}
