#pragma once

#include <stdlib.h>

typedef uint32_t LAB_Color;

#if 1
#define LAB_RGB(r, g, b) ((r) | (g) << 8 | (b) << 16 | 0xff000000u)
#define LAB_RGBA(r, g, b, a) ((r) | (g) << 8 | (b) << 16 | (a) << 24)
// LAB_RGBX(RRGGBB)
#define LAB_RGBX(hex) LAB_RGB((0x##hex) >> 16 & 0xff, (0x##hex) >> 8 & 0xff, (0x##hex) & 0xff)
// LAB_RGBAX(RRGGBBAA)
#define LAB_RGBAX(hex) LAB_RGB((0x##hex) >> 24 & 0xff, (0x##hex) >> 16 & 0xff, (0x##hex) >> 8 & 0xff, (0x##hex) & 0xff)
#define LAB_RED(col) ((col)       & 0xffu)
#define LAB_GRN(col) ((col) >>  8 & 0xffu)
#define LAB_BLU(col) ((col) >> 16 & 0xffu)
#define LAB_ALP(col) ((col) >> 24 & 0xffu)
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


// use in contstant context or when the a, b can be reevaluated
#define LAB_MUL_COLOR(a, b) \
    LAB_RGBA(LAB_RED(a)*LAB_RED(b)/255, \
             LAB_GRN(a)*LAB_GRN(b)/255, \
             LAB_BLU(a)*LAB_BLU(b)/255, \
             LAB_ALP(a)*LAB_ALP(b)/255)

static inline LAB_Color LAB_MulColor(LAB_Color a, LAB_Color b)
{
    return LAB_RGBA(LAB_RED(a)*LAB_RED(b)/255,
                    LAB_GRN(a)*LAB_GRN(b)/255,
                    LAB_BLU(a)*LAB_BLU(b)/255,
                    LAB_ALP(a)*LAB_ALP(b)/255);
}

#if 0
static inline LAB_Color LAB_MulColor_Fast(LAB_Color a, LAB_Color b)
{
    return LAB_RGBA(LAB_RED(a)*(LAB_RED(b)+1)/256,
                    LAB_GRN(a)*(LAB_GRN(b)+1)/256,
                    LAB_BLU(a)*(LAB_BLU(b)+1)/256,
                    LAB_ALP(a)*(LAB_ALP(b)+1)/256);
}

#else
/**
 *
 *  00AA00BB * 00AA00BB
 *
 *  0000AAAA 00000000
 *  00000001 XXXX0000
 *  00000000 0000BBBB
 *      --       --
 *         ^
 *  Does not affect result
 */
static inline LAB_Color LAB_MulColor_Fast(LAB_Color a, LAB_Color b)
{
    uint32_t a02 = a&0x00ff00ffu;
    uint32_t b02 = b&0x00ff00ffu;
    uint32_t a13 = a&0xff00ff00u;
    uint32_t b13 = b&0xff00ff00u;

    uint64_t p02 = (uint64_t)a02*(uint64_t)b02;
    uint64_t p13 = (uint64_t)a13*(uint64_t)b13;

    #if 0
    // 255*255 -> 254
    return (p02 >>   8     & 0xffu    )
         | (p13 >> (24- 8) & 0xffu<< 8)
         | (p02 >> (40-16) & 0xffu<<16)
         | (p13 >> (56-24) & 0xffu<<24);
    #else
    // 255*255 -> 255
    uint32_t r02 = (/*LO*/ (p02 & 0xffffffffu) & 0xffff)
                 | (/*HI*/ (p02 >> 32)         & 0xffff) << 16;
    r02 = (r02 + a02) >> 8;
    uint32_t r13 = (/*LO*/ (p13 & 0xffffffffu) >> 16)
                 | (/*HI*/ (p13 >> 32)         >> 16) << 16;
    r13 = (r13 + (a13>>8));
    return (r02 & 0x00ff00ffu) | (r13 & 0xff00ff00u);

    #endif
}
#endif



static inline LAB_Color LAB_MixColor50(LAB_Color a, LAB_Color b)
{
    uint32_t x = (a>>1 & 0xfefefefeu) + (b>>1 & 0xfefefefeu);
    x+=a&b&0x01010101; // add "carry" of the addition of each ones bit
    return x;
}
