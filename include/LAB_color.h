/** \file LAB_color.h
 *
 *  All around handling RGBA colors.
 */

#pragma once

#include "LAB_util.h"
#include "LAB_attr.h"
#include "LAB_check.h"
#include "LAB_arith.h"

#include "LAB_stdinc.h"

#if 0 //!defined __LITTLE_ENDIAN__ || !__LITTLE_ENDIAN__
#error RGB-Format not supported on this platform
#endif

/**
 *  The type used to represent colors
 */
typedef uint32_t LAB_Color;

/**
 *  Create a color from red, green and blue channels, each in the range
 *  [0, 255].
 *  Alpha is set to 255.
 *  LAB_RGBX provides a more compact way to define a color
 *
 *  \see LAB_RGBX
 */
#define LAB_RGB(r, g, b) LAB_CXADD4((uint32_t)(r), (uint32_t)(g) << 8, (uint32_t)(b) << 16, 0xff000000u)
/**
 *  Create a color from red, green, blue and alpha channels, each in the range
 *  [0, 255].
 */
#define LAB_RGBA(r, g, b, a) LAB_CXADD4((uint32_t)(r), (uint32_t)(g) << 8, (uint32_t)(b) << 16, (uint32_t)(a) << 24)
/**
 *  Create a color from red, green and blue channels
 *
 *  Syntax
 *  ```
 *  LAB_RGBX(RRGGBB)
 *  ```
 */
#define LAB_RGBX(hex) LAB_RGB((0x##hex) >> 16 & 0xff, (0x##hex) >> 8 & 0xff, (0x##hex) & 0xff)
/**
 *  Create a color from red, green and blue channels
 *
 *  Syntax
 *  ```
 *  LAB_RGBX(RRGGBBAA)
 *  ```
 */
#define LAB_RGBAX(hex) LAB_RGBA((0x##hex) >> 24 & 0xff, (0x##hex) >> 16 & 0xff, (0x##hex) >> 8 & 0xff, (0x##hex) & 0xff)
/**
 *  Get the red channel of a color
 */
#define LAB_RED(col) ((col)       & 0xffu)
/**
 *  Get the green channel of a color
 */
#define LAB_GRN(col) ((col) >>  8 & 0xffu)
/**
 *  Get the blue channel of a color
 */
#define LAB_BLU(col) ((col) >> 16 & 0xffu)
/**
 *  Get the alpha channel of a color
 */
#define LAB_ALP(col) ((col) >> 24 & 0xffu)

#define LAB_RED_MASK 0x000000ffu
#define LAB_GRN_MASK 0x0000ff00u
#define LAB_BLU_MASK 0x00ff0000u
#define LAB_ALP_MASK 0xff000000u

#define LAB_COL_MASK 0x00ffffffu



/**
 *  Add two colors component wise, saturate values greater than 255 to 255
 */
LAB_CONST
LAB_INLINE LAB_Color LAB_AddColor(LAB_Color a, LAB_Color b)
{
    LAB_Color s0 =  (a&0x00ff00ffu)     +  (b&0x00ff00ffu);
    LAB_Color s1 = ((a&0xff00ff00u)>>1) + ((b&0xff00ff00u)>>1); // needs to be shifted by 1 later

    LAB_Color c0 = s0&0x01000100u;
    LAB_Color c1 = s1&0x80008000u;
    LAB_Color m0 =  c0    -(c0>>8);
    LAB_Color m1 = (c1<<1)-(c1>>7);

    LAB_Color x0 = (s0   &0x00ff00ffu) | m0;
    LAB_Color x1 = (s1<<1&0xff00ff00u) | m1;

    return LAB_XADD(x0, x1);
}

/**
 *  Subtract two colors component wise, saturate values less than 0 to 0
 */
LAB_CONST
LAB_INLINE LAB_Color LAB_SubColor(LAB_Color a, LAB_Color b)
{
    return ~LAB_AddColor(~a, b);
}


/**
 *  fill channels where a < b with 0xff
 */
LAB_CONST
LAB_INLINE LAB_Color LAB_CompMask(LAB_Color a, LAB_Color b)
{
    //                                                   wrong
    //                                                     v
//    LAB_Color ga_mask = ((LAB_XADD(a>>8&0x00ff00ffu, 0x00100000u)-(b>>8&0x00ff00ffu))    & 0x01000100u)*0xffu;
//    LAB_Color rb_mask = ((LAB_XADD(a   &0x00ff00ffu, 0x00001000u)-(b   &0x00ff00ffu))>>8 & 0x00010001u)*0xffu;
    /*LAB_Color ga_mask = ((((a>>8&0x00ff00ff)|0x00100000)-(b>>8&0x00ff00ff))    & 0x01000100)*0xff;
    LAB_Color rb_mask = ((((a   &0x00ff00ff)|0x00001000)-(b   &0x00ff00ff))>>8 & 0x00010001)*0xff;*/
    //LAB_Color ga_mask = ((((a>>8&0x00ff00ff)|0x00100000)-(b>>8&0x00ff00ff))    & 0x01000100)*0xff;
    //LAB_Color rb_mask = ((((a   &0x00ff00ff)|0x00001000)-(b   &0x00ff00ff))>>8 & 0x00010001)*0xff;

    //                        Borrow bit, such that a borrow does not bleed into a byte before
    //                                                      v
    LAB_Color ga_mask = ((LAB_XADD(a>>8&0x00ff00ffu, 0x00000200u)-(b>>8&0x00ff00ffu))    & 0x01000100u)*0xffu;
    LAB_Color rb_mask = ((LAB_XADD(a   &0x00ff00ffu, 0x00000200u)-(b   &0x00ff00ffu))>>8 & 0x00010001u)*0xffu;

    return LAB_XADD(rb_mask, ga_mask);
}

// branching, alpha gets 255
LAB_DEPRECATED("use LAB_MaxColor instead")
LAB_CONST
LAB_INLINE LAB_Color LAB_MaxColorB(LAB_Color a, LAB_Color b)
{
    int ar, ag, ab, br, bg, bb;
    ar = LAB_RED(a); ag = LAB_GRN(a); ab = LAB_BLU(a);
    br = LAB_RED(b); bg = LAB_GRN(b); bb = LAB_BLU(b);
    return LAB_RGB(ar<br?br:ar, ag<bg?bg:ag, ab<bb?bb:ab);
}

/**
 *  Get the maximum values for each channel component wise.
 *  It happens without any branches.
 *
 *  \see LAB_MinColor
 */
LAB_PURE
LAB_INLINE LAB_Color LAB_MaxColor(LAB_Color a, LAB_Color b)
{
    //return a ^ ((a^b) & LAB_CompMask(a, b));
    LAB_Color mask = LAB_CompMask(a, b);
    return LAB_XADD(a & ~mask, b & mask);
}

// branching, alpha gets 255
LAB_DEPRECATED("use LAB_MinColor instead")
LAB_CONST
LAB_INLINE LAB_Color LAB_MinColorB(LAB_Color a, LAB_Color b)
{
    int ar, ag, ab, br, bg, bb;
    ar = LAB_RED(a); ag = LAB_GRN(a); ab = LAB_BLU(a);
    br = LAB_RED(b); bg = LAB_GRN(b); bb = LAB_BLU(b);
    return LAB_RGB(ar>br?br:ar, ag>bg?bg:ag, ab>bb?bb:ab);
}

/**
 *  Get the minimum values for each channel component wise.
 *  It happens without any branches.
 *
 *  \see LAB_MaxColor
 */
LAB_CONST
LAB_INLINE LAB_Color LAB_MinColor(LAB_Color a, LAB_Color b)
{
    //return b ^ ((a^b) & LAB_CompMask(a, b));
    LAB_Color mask = LAB_CompMask(a, b);
    return LAB_XADD(a & mask, b & ~mask);
}

/**
 *  Multiply two colors
 *  Use in contstant context or when the a, b can be reevaluated
 *
 *  \see LAB_MulColor
 */
#define LAB_MUL_COLOR(a, b) \
    LAB_RGBA(LAB_RED(a)*LAB_RED(b)/255, \
             LAB_GRN(a)*LAB_GRN(b)/255, \
             LAB_BLU(a)*LAB_BLU(b)/255, \
             LAB_ALP(a)*LAB_ALP(b)/255)

//#define LAB_SQR_COLOR(a) LAB_MUL_COLOR(a, a)

LAB_CONST
LAB_INLINE LAB_Color LAB_MulColor(LAB_Color a, LAB_Color b)
{
    return LAB_RGBA(LAB_RED(a)*LAB_RED(b)/255,
                    LAB_GRN(a)*LAB_GRN(b)/255,
                    LAB_BLU(a)*LAB_BLU(b)/255,
                    LAB_ALP(a)*LAB_ALP(b)/255);
}

#if 0
LAB_PURE
LAB_INLINE LAB_Color LAB_MulColor_Fast(LAB_Color a, LAB_Color b)
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
LAB_CONST
LAB_INLINE LAB_Color LAB_MulColor_Fast(LAB_Color a, LAB_Color b)
{
    uint32_t a02 = a&0x00ff00ffu;
    uint32_t b02 = b&0x00ff00ffu;
    uint32_t a13 = a&0xff00ff00u;
    uint32_t b13 = b&0xff00ff00u;

    uint64_t p02 = (uint64_t)a02*(uint64_t)b02;
    uint64_t p13 = (uint64_t)a13*(uint64_t)b13;

    #if 0
    // 255*255 -> 254
    return LAB_XADD4(p02 >>   8     & 0xffu    ,
                     p13 >> (24- 8) & 0xffu<< 8,
                     p02 >> (40-16) & 0xffu<<16,
                     p13 >> (56-24) & 0xffu<<24)
           + 0x01010101;
    #else
    // 255*255 -> 255
    uint32_t r02 = LAB_XADD( /*LO*/ (p02 & 0xffffffffu) & 0xffff,
                            (/*HI*/ (p02 >> 32)         & 0xffff) << 16);
    r02 = (r02 + a02) >> 8;
    uint32_t r13 = LAB_XADD( /*LO*/ (p13 & 0xffffffffu) >> 16,
                            (/*HI*/ (p13 >> 32)         >> 16) << 16);
    r13 = (r13 + (a13>>8));
    return LAB_XADD(r02 & 0x00ff00ffu, r13 & 0xff00ff00u);

    #endif
}
#endif


LAB_CONST
LAB_INLINE LAB_Color LAB_BlendColor(LAB_Color dst, LAB_Color src)
{
    // TODO: fast version
    //int alpha = LAB_ALP(src);
    //LAB_Color rgb = LAB_MulColor_Fast(dst, LAB_255^alpha)

    // res_alpha = src_alpha + (1 - src_alpha)*dst_alpha
    // res_color = 1/res_alpha * (src_alpha*src_color + (1-src_alpha)*dst_alpha*dst_color)

    int src_alp = LAB_ALP(src);
    int dst_alp = LAB_ALP(dst);       //  vvvvv fast mul channel -- TODO
    int r_alp = src_alp + (int)(255^src_alp)*dst_alp/255;
    if(r_alp == 0) return dst;

    int r_red = (src_alp*255*LAB_RED(src) + (255^src_alp)*dst_alp*LAB_RED(dst))/(255*r_alp);
    int r_grn = (src_alp*255*LAB_GRN(src) + (255^src_alp)*dst_alp*LAB_GRN(dst))/(255*r_alp);
    int r_blu = (src_alp*255*LAB_BLU(src) + (255^src_alp)*dst_alp*LAB_BLU(dst))/(255*r_alp);
    //LAB_ASSERT(r_red <= 255);
    //LAB_ASSERT(r_grn <= 255);
    //LAB_ASSERT(r_blu <= 255);

    return LAB_RGBA(r_red, r_grn, r_blu, r_alp);
}

// used for tinting images
LAB_CONST
LAB_INLINE LAB_Color LAB_LerpColor(LAB_Color a, LAB_Color b, LAB_Color t)
{
    // TODO faster
    // add color really needed?
    return LAB_AddColor(LAB_MulColor_Fast(a, ~t), LAB_MulColor_Fast(b, t));
}

LAB_CONST
LAB_INLINE LAB_Color LAB_MixColor50(LAB_Color a, LAB_Color b)
{
    uint32_t x = (a>>1 & 0x7f7f7f7fu) + (b>>1 & 0x7f7f7f7fu);
    x+=a&b&0x01010101; // add "carry" of the addition of each ones bit
    return x;
}

LAB_CONST
LAB_INLINE LAB_Color LAB_MixColor4x25(LAB_Color a, LAB_Color b,
                                      LAB_Color c, LAB_Color d)
{
    return LAB_MixColor50(LAB_MixColor50(a, b),
                          LAB_MixColor50(c, d));
}



/**
 *    ^ . . . .________
 *    | . . . / . . . .
 *    | . . ./. . . . .
 *    | . . / . . . . .
 *    | . ./. . . . . .
 *    | . / . . . . . .
 *    | ./. . . . . . .
 *    | / . . . . . . .
 *    |/. . . . . . . .
 *    +---------------->
 *
 */
LAB_CONST
LAB_INLINE LAB_Color LAB_MulColor2_Saturate(LAB_Color c)
{
    LAB_Color x = c << 1;
    return (x&0xfefefefeu) | ((x&0x01010100u) - ((c&0x80808080u) >> 7));
    //return (x&0xfefefefeu) | 255u*((c&0x80808080u) >> 7);
}


LAB_CONST
LAB_INLINE LAB_Color LAB_HighColor(LAB_Color c)
{
    // ASSUME: Little endian, 0xAA'CC'CC'CC, where C consists of R G B
    //LAB_Color a = c & 0x80808080u;
    LAB_Color a = c & 0x00808080u;
    LAB_Color m = a - (a >> 7);
    //LAB_Color m = (a >> 7) * 127u;
    LAB_Color o = c & m; // oversaturate value  mask: 0x7f
    LAB_Color avg = ((o&0xffu) + (o>>8&0xffu) + (o>>16&0xffu))*2/3; // avg value
    //LAB_Color avg = ((o&0xffu) + (o>>8&0xffu) + (o>>16&0xffu))/2; // avg value
    LAB_Color s = avg*0x00010101u; // spread

    //return LAB_MaxColor(LAB_MulColor2_Saturate(c), s);
    //return LAB_MulColor2_Saturate(LAB_MixColor50(c, s));
    return LAB_MixColor50(c, LAB_MulColor2_Saturate(LAB_MixColor50(c, s)));
}

LAB_CONST
LAB_INLINE LAB_Color LAB_HighColor2(LAB_Color c)
{
    c = LAB_HighColor(c);
    uint32_t r, g, b;
    r = LAB_RED(c);
    g = LAB_GRN(c);
    b = LAB_BLU(c);
    r^=255; r=(r*(r+1))/256; r^=255;
    g^=255; g=(g*(g+1))/256; g^=255;
    b^=255; b=(b*(b+1))/256; b^=255;
    return LAB_RGBA(r, g, b, LAB_ALP(c));
}

LAB_CONST
LAB_INLINE LAB_Color LAB_OversaturateColor(LAB_Color c)
{
    uint32_t r, g, b, m;
    r = LAB_RED(c);
    g = LAB_GRN(c);
    b = LAB_BLU(c);

    m = LAB_MAX3(r, g, b);
    if(m == 0) return LAB_RGB(255, 255, 255);
    return LAB_RGB(r*255/m, g*255/m, b*255/m);

}


LAB_CONST
LAB_INLINE LAB_Color LAB_InterpolateColor2f(LAB_Color a, LAB_Color b, float m)
{
    return LAB_RGBA(
        (int)(LAB_RED(a)*(1-m) + LAB_RED(b)*m),
        (int)(LAB_GRN(a)*(1-m) + LAB_GRN(b)*m),
        (int)(LAB_BLU(a)*(1-m) + LAB_BLU(b)*m),
        (int)(LAB_ALP(a)*(1-m) + LAB_ALP(b)*m)
    );
}

LAB_CONST
LAB_INLINE LAB_Color LAB_InterpolateColor4vf(LAB_Color* colors,
                                             float u, float v)
{
    return LAB_InterpolateColor2f(
                LAB_InterpolateColor2f(colors[0], colors[1], u),
                LAB_InterpolateColor2f(colors[2], colors[3], u),
                v
    );
}


LAB_CONST
LAB_INLINE LAB_Color LAB_InterpolateColor2i(LAB_Color a, LAB_Color b, int m)
{
    #if 0
    // 32 imul instructions, 2 mulss instructions in LAB_model.o
    return LAB_RGBA(
        (int)(LAB_RED(a)*(256-m) + LAB_RED(b)*m)>>8&0xff,
        (int)(LAB_GRN(a)*(256-m) + LAB_GRN(b)*m)>>8&0xff,
        (int)(LAB_BLU(a)*(256-m) + LAB_BLU(b)*m)>>8&0xff,
        (int)(LAB_ALP(a)*(256-m) + LAB_ALP(b)*m)>>8&0xff
    );
    #elif 1
    // 20 imul instructions, 2 mulss instructions in LAB_model.o
    return LAB_RGBA(
        (int)(LAB_RED(a)*256 + (LAB_RED(b)-LAB_RED(a))*m)>>8&0xff,
        (int)(LAB_GRN(a)*256 + (LAB_GRN(b)-LAB_GRN(a))*m)>>8&0xff,
        (int)(LAB_BLU(a)*256 + (LAB_BLU(b)-LAB_BLU(a))*m)>>8&0xff,
        (int)(LAB_ALP(a)*256 + (LAB_ALP(b)-LAB_ALP(a))*m)>>8&0xff
    );
    #else
    // 30 imul instructions, 6 mulss instructions in LAB_model.o
    // TODO: use 2 imuls
    //uint32_t x = (a&0x00ff00ff)-((((b&0x00ff00ff|0x00000100)-(a&0x00ff00ff))*m>>8)&0x00ff00ff);
    //uint32_t y = (a&0xff00ff00)-((((b&0xff00ff00|0x00010000)-(a&0xff00ff00))*m>>8)&0xff00ff00);

    uint32_t x = ((a&0x00ff00ff)+(((LAB_XADD(b&0x00ff00ff, 0x00000100)-(a&0x00ff00ff))*m)>>8))&0x00ff00ff;
    uint32_t y = ((a&0xff00ff00)+(((LAB_XADD(b&0xff00ff00, 0x00010000)-(a&0xff00ff00))>>8)*m))&0xff00ff00;

    //uint32_t x = (a&0xff)+(((b&0xff)-(a&0xff))*m>>8);
    //uint32_t x = (a&0x00ff00ff)-((((b&0x00ff00ff|0x00000100)-(a&0x00ff00ff))*m>>8)&0x00ff00ff);
    //uint32_t y = 0;//(a&0xff00ff00)-((((b&0xff00ff00|0x00010000)-(a&0xff00ff00))*m>>8)&0xff00ff00);
    return x|y;
    #endif
}

LAB_CONST
LAB_INLINE LAB_Color LAB_InterpolateColor4vi(const LAB_Color* colors,
                                             int u, int v)
{
    return LAB_InterpolateColor2i(
                LAB_InterpolateColor2i(colors[0], colors[1], u),
                LAB_InterpolateColor2i(colors[2], colors[3], u),
                v
    );
}


LAB_CONST
LAB_INLINE LAB_Color LAB_ColorHI4(LAB_Color c)
{
    //return (c&0xf0f0f0f0)|(c&0xf0f0f0f0)>>4;
    //return c-(((c&0xf0f0f0f0)^0xf0f0f0f0)>>4);
    LAB_Color a = (c&0xff00ff00)-(((c&0xf000f000)^0xf000f000)>>4);
    LAB_Color b = (c&0x00ff00ff)-(((c&0x00f000f0)^0x00f000f0)>>4);
    //a &= (LAB_CompMask(a, c)&0xff00ff00);
    //b &= (LAB_CompMask(b, c)&0x00ff00ff);
    a &= (LAB_CompMask(c, a)&0xff00ff00)^0xff00ff00;
    b &= (LAB_CompMask(c, b)&0x00ff00ff)^0x00ff00ff;
    return a|b;
}
