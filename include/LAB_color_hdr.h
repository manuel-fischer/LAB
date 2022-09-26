#pragma once
#define LAB_DBG_HDR 1

#include "LAB_util.h"
#include "LAB_color.h"
#include "LAB_bits.h"

#include <math.h>

// Note: no support for alpha channel, used as exponent instead
typedef uint32_t LAB_ColorHDR;

#define LAB_RGBE_HDR(r, g, b, e) LAB_CXADD4((uint32_t)(r), (uint32_t)(g) << 8, (uint32_t)(b) << 16, (uint32_t)(e) << 24)


#define LAB_HDR_RED(col) ((col)       & 0xffu)
#define LAB_HDR_GRN(col) ((col) >>  8 & 0xffu)
#define LAB_HDR_BLU(col) ((col) >> 16 & 0xffu)
#define LAB_HDR_EXP(col) ((col) >> 24)

#define LAB_HDR_EXP_VALUE(col) (LAB_HDR_EXP(col)-128)

#define LAB_HDR_UNIT_WHITE LAB_RGBE_HDR(128, 128, 128, 129)
#define LAB_HDR_UNIT_RED LAB_RGBE_HDR(128,   0,   0, 129)
#define LAB_HDR_UNIT_GRN LAB_RGBE_HDR(  0, 128,   0, 129)
#define LAB_HDR_UNIT_BLU LAB_RGBE_HDR(  0,   0, 128, 129)

#define LAB_HDR_RGB_F(r, g, b) LAB_ColorHDR_RGB_F(r, g, b)

#if LAB_DBG_HDR
#define LAB_HDR_ASSERT(cond) LAB_ASSERT(cond)
#define LAB_HDR_ASSERT_FMT(cond, ...) LAB_ASSERT_FMT(cond, __VA_ARGS__)
#else
#define LAB_HDR_ASSERT(cond) ((void)0)
#define LAB_HDR_ASSERT_FMT(cond, ...) ((void)0)
#endif


// returns denormalized shifted color
// this is used to bring a smaller color to the same exponent as a larger color
LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_ColorHDR_ShiftRightTo(LAB_ColorHDR c, uint8_t new_exp)
{
    uint8_t old_exp = LAB_HDR_EXP(c);
    LAB_ASSERT_FMT(old_exp <= new_exp, "%i <= %i, c = %08x", old_exp, new_exp, c);

    uint8_t exp_bits = (uint32_t)new_exp << 24;
    uint8_t ds = LAB_MIN_BL(new_exp - old_exp, 8);
    //if(ds == 0) return c;
    //if(ds >= 8) return black;
    //if(ds > 8) ds = 8;
    return exp_bits | ((c >> ds) & ((0x01010100u >> ds) - 0x00010101u));
}


LAB_CONST
LAB_INLINE bool LAB_ColorHDR_IsNormal(LAB_ColorHDR c)
{
    // either one channel is at least 128 or the exponent is minimal
    return !!(c & 0x00808080) | ((c & 0xff000000) == 0);
}

#if LAB_DBG_HDR
#define LAB_ColorHDR_R(c) (LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(c), "c=%8x", c), (c))
#else
#define LAB_ColorHDR_R(c) (c)
#endif


#define LAB_HDR_NORMALIZE_SHIFT_EXPONENT(shift, exponent) do { \
    LAB_ASSERT_FMT((exponent) >= 0, "%i", (exponent)); \
    (exponent) -= (shift); \
    (shift) += LAB_SELECT0((exponent) < 0, (exponent)); /* decrementing */ \
    (exponent) = LAB_BOUND0(exponent); \
} while(0)

LAB_CONST LAB_ColorHDR LAB_ColorHDR_Normalize(LAB_ColorHDR c)
{
    if(!(c & 0x00ffffffu)) return 0; // branch nessecary: Log2 UB for 0
    uint32_t occupied = (c | c >> 8 | c >> 16) & 0xffu;
    //if(!occupied) return 0;
    int shift = 7-LAB_Log2Floor(occupied);

    int new_exp = LAB_HDR_EXP(c);
    LAB_HDR_NORMALIZE_SHIFT_EXPONENT(shift, new_exp);

    uint32_t exp_bits = (uint32_t)new_exp << 24;
    return LAB_ColorHDR_R(exp_bits | c << shift); // no masking nessecary, because bits shifted into larger channels are 0
}


// UB if overflow
LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_AddColorHDR(LAB_ColorHDR a, LAB_ColorHDR b)
{
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(a), "a = %8x", a);
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(b), "b = %8x", b);

    // ensure a < b
    LAB_COND_SWAP_T(LAB_ColorHDR, a > b, a, b);
    
    a = LAB_ColorHDR_ShiftRightTo(a, LAB_HDR_EXP(b));

    uint32_t sum_rb = (0x00ff00ffu&a) + (0x00ff00ffu&b);
    uint32_t sum_g  = (0x0000ff00u&a) + (0x0000ff00u&b);

    // check if overflow happened
    bool shift = !!(0x01000100u&sum_rb) | !!(0x00010000u&b);
    //bool shift = (sum_rb >> 8 | sum_rb >> 24 | sum_g >> 16) & 1;

    // increase exponent and shift channels on overflow
    sum_rb >>= shift; sum_rb &= 0x00ff00ffu;
    sum_g  >>= shift; sum_g  &= 0x0000ff00u;

    uint32_t exp = (b & 0xff000000u) + (shift << 24);

    return LAB_ColorHDR_R(sum_rb | sum_g | exp);
}


// Saturates on underflow
LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_SubColorHDR(LAB_ColorHDR a, LAB_ColorHDR b)
{
    int common_exp = LAB_MAX_BL(LAB_HDR_EXP(a), LAB_HDR_EXP(b));
    
    a = LAB_ColorHDR_ShiftRightTo(a, common_exp);
    b = LAB_ColorHDR_ShiftRightTo(b, common_exp);

    uint32_t diff_r = LAB_BOUND0((int)LAB_HDR_RED(a) - (int)LAB_HDR_RED(b));
    uint32_t diff_g = LAB_BOUND0((int)LAB_HDR_GRN(a) - (int)LAB_HDR_GRN(b));
    uint32_t diff_b = LAB_BOUND0((int)LAB_HDR_BLU(a) - (int)LAB_HDR_BLU(b));

    LAB_HDR_ASSERT_FMT(diff_r < 256u, "%i", diff_r);
    LAB_HDR_ASSERT_FMT(diff_g < 256u, "%i", diff_g);
    LAB_HDR_ASSERT_FMT(diff_b < 256u, "%i", diff_b);

    uint32_t occupied = diff_r|diff_g|diff_b;
    if(!occupied) return 0;

    int shift = 7-LAB_Log2Floor(occupied);
    LAB_HDR_NORMALIZE_SHIFT_EXPONENT(shift, common_exp);

    uint32_t c = diff_r | diff_g << 8 | diff_b << 16;

    uint32_t exp_bits = (uint32_t)common_exp << 24;
    return LAB_ColorHDR_R(exp_bits | c << shift); // no masking nessecary, because bits shifted into larger channels are 0
}


LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_MulColorHDR(LAB_ColorHDR a, LAB_ColorHDR b)
{
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(a), "a = %8x", a);
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(b), "b = %8x", b);

    //if(!a | !b) return 0; // wrong, could still create problematic black

    // TODO: exp = 0

    uint32_t prod_r = (a       & 0xffu)*(b       & 0xffu);
    uint32_t prod_g = (a >>  8 & 0xffu)*(b >>  8 & 0xffu);
    uint32_t prod_b = (a >> 16 & 0xffu)*(b >> 16 & 0xffu);
    
    // combine bits to one integer, such that the exponent can be calculated
    uint32_t occupied = prod_r|prod_g|prod_b;
    if(occupied == 0) return 0; // branch necessary here, because Log2Floor does not accept 0

    int log2 = LAB_Log2Floor(occupied);
    LAB_HDR_ASSERT_FMT(log2 >= 8, "log2 = %i", log2);
    LAB_HDR_ASSERT_FMT(log2 < 16, "log2 = %i", log2);
    int shift = log2-7; // shift from 16 back to 8 bits
    LAB_HDR_ASSERT_FMT(shift <= 8, "shift = %i", shift);

    // sub 128 once, because both have 128 added onto their exponent-value
    int exp = (int)(LAB_HDR_EXP(a) + LAB_HDR_EXP(b)) - 128;
    exp -= 8-shift;

    shift -= LAB_SELECT0(exp < 0, exp); // increment: exp negative
    shift = LAB_MIN_BL(shift, 16); // avoid UB: if shift=16, channels zero anyway
    exp = LAB_SELECT0(exp >= 0, exp);

    prod_r >>= shift;
    prod_g >>= shift;
    prod_b >>= shift;

    LAB_HDR_ASSERT_FMT(prod_r < 256u, "prod_r = %i", prod_r);
    LAB_HDR_ASSERT_FMT(prod_g < 256u, "prod_g = %i", prod_g);
    LAB_HDR_ASSERT_FMT(prod_b < 256u, "prod_b = %i", prod_b);

    return prod_r | prod_g << 8 | prod_b << 16 | (uint32_t)exp << 24;
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_MaxColorHDR(LAB_ColorHDR a, LAB_ColorHDR b)
{
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(a), "a = %8x", a);
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(b), "b = %8x", b);

    // ensure a < b
    LAB_COND_SWAP_T(LAB_ColorHDR, a > b, a, b);
    
    a = LAB_ColorHDR_ShiftRightTo(a, LAB_HDR_EXP(b));

    //return LAB_MaxColor(a, b); // hack    

    uint32_t r_bits = LAB_MAX_BL(a&0x000000ffu, b&0x000000ffu);
    uint32_t g_bits = LAB_MAX_BL(a&0x0000ff00u, b&0x0000ff00u);
    uint32_t b_bits = LAB_MAX_BL(a&0x00ff0000u, b&0x00ff0000u);
    uint32_t e_bits = b & 0xff000000u;
    return r_bits | g_bits | b_bits | e_bits;
}


LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_Float_To_ColorHDR_Mask(float f, uint32_t mask)
{
    int exp;
    float frac = frexpf(f, &exp);

    int gray = (int)floorf(frac * 256.0f);
    LAB_ASSERT(gray == 0 || gray >= 128);
    LAB_ASSERT(gray <  256);

    exp += 128;

    if(exp < 8) return 0;
    if(exp < 0) { gray >>= -exp; exp = 0; }
    if(gray == 0) return 0;

    return LAB_ColorHDR_R((mask*gray) | exp << 24);
}

LAB_INLINE LAB_ColorHDR LAB_Float_To_ColorHDR(float f)
{
    return LAB_Float_To_ColorHDR_Mask(f, 0x00010101u);
}




LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_ColorHDR_RGB_F(float r, float g, float b)
{
    LAB_ColorHDR c0 = LAB_Float_To_ColorHDR_Mask(r, 0x00000001u);
    LAB_ColorHDR c1 = LAB_Float_To_ColorHDR_Mask(g, 0x00000100u);
    LAB_ColorHDR c2 = LAB_Float_To_ColorHDR_Mask(b, 0x00010000u);

    //LAB_ColorHDR exp = LAB_LAB_MAX3_BL(rc, gc, bc);
    
    // move largest to c2
    LAB_COND_SWAP_T(LAB_ColorHDR, c0 > c1, c0, c1);
    LAB_COND_SWAP_T(LAB_ColorHDR, c1 > c2, c1, c2);

    uint_fast8_t exp = LAB_HDR_EXP(c2);
    c0 = LAB_ColorHDR_ShiftRightTo(c0, exp);
    c1 = LAB_ColorHDR_ShiftRightTo(c1, exp);
    return c0|c1|c2;
}



LAB_CONST
LAB_INLINE float LAB_ColorHDR_ChannelValueF(uint8_t mantissa, uint8_t exp)
{
    return exp2((float)((int)exp-8-128)) * (float)mantissa;
}

LAB_CONST
LAB_INLINE uint8_t LAB_ColorHDR_ChannelValueU8Saturate(uint8_t mantissa, uint8_t exp)
{
    LAB_HDR_ASSERT_FMT(exp > 128u, "%i, exp");
    if(mantissa == 0) return 0;

    uint_fast8_t exp_val = exp-128u;
    // avoid UB with shift: if exp_val > 8, m results in at least 256
    LAB_COND_MOVE(exp_val >= 8, exp_val, 8);

    uint_fast16_t m = mantissa;
    m <<= exp_val;

    LAB_COND_MOVE(m > 255u, m, 255u);

    return m;
}

#define LAB_HDR_RED_VAL(c) LAB_ColorHDR_ChannelValueF(LAB_HDR_RED(c), LAB_HDR_EXP(c))
#define LAB_HDR_GRN_VAL(c) LAB_ColorHDR_ChannelValueF(LAB_HDR_GRN(c), LAB_HDR_EXP(c))
#define LAB_HDR_BLU_VAL(c) LAB_ColorHDR_ChannelValueF(LAB_HDR_BLU(c), LAB_HDR_EXP(c))

LAB_CONST
LAB_INLINE float LAB_ColorHDR_MaxChannelValueF(LAB_ColorHDR c)
{
    return LAB_ColorHDR_ChannelValueF(LAB_MAX3_BL(LAB_HDR_RED(c), LAB_HDR_GRN(c), LAB_HDR_BLU(c)), LAB_HDR_EXP(c));
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_ColorHDR_MaxChannelGray(LAB_ColorHDR c)
{
    uint32_t v = LAB_MAX3_BL(LAB_HDR_RED(c), LAB_HDR_GRN(c), LAB_HDR_BLU(c));
    return LAB_ColorHDR_R(0x00010101u*v | (c & 0xff000000u));
}



LAB_CONST
LAB_INLINE LAB_Color LAB_ColorHDR_To_Color_Overflow(LAB_ColorHDR c, LAB_ColorHDR overflow_color)
{
    if(c < 0x81000000u) // up to exp = 128
    {
        c = LAB_ColorHDR_ShiftRightTo(c, 128);
        return c | 0xff000000u; // alpha = 255
    }
    else
    {
        LAB_ColorHDR ov = LAB_SubColorHDR(c, LAB_HDR_UNIT_WHITE);
        ov = LAB_ColorHDR_MaxChannelGray(ov);

        c = LAB_AddColorHDR(c, LAB_MulColorHDR(ov, overflow_color));

        uint8_t r = LAB_ColorHDR_ChannelValueU8Saturate(LAB_HDR_RED(c), LAB_HDR_EXP(c));
        uint8_t g = LAB_ColorHDR_ChannelValueU8Saturate(LAB_HDR_GRN(c), LAB_HDR_EXP(c));
        uint8_t b = LAB_ColorHDR_ChannelValueU8Saturate(LAB_HDR_BLU(c), LAB_HDR_EXP(c));

        return r | g << 8 | b << 16 | 0xff000000u;
    }
}




#ifdef LAB_COLOR_HDR_TEST
void LAB_ColorHDR_DoTestCases(void)
{
    struct { float r, g, b; } colors[] = {
        { 0.1, 0.2, 0.3 },
        { 1.0, 2.0, 3.0 },
        //{ 1, 100, 1024  },
        //{ 1, 104, 1024  },
        //{ 1, 128, 1024  },
        //{ 1, 100, 10000 },
        { 0.9, 1, 0.8 },
        { 0.2, 0.9, 1 },
        { 10, 12, 14  },
        { 2, 3, 5 },
        { 5, 4, 3 },
    };
    size_t color_count = sizeof(colors)/sizeof(*colors);

    for(size_t i = 0; i < color_count; ++i)
    {
        float r = colors[i].r, g = colors[i].g, b = colors[i].b;
        LAB_ColorHDR c = LAB_HDR_RGB_F(r, g, b);
        printf("%f, %f, %f = %f, %f, %f\n", r, g, b, LAB_HDR_RED_VAL(c), LAB_HDR_GRN_VAL(c), LAB_HDR_BLU_VAL(c));
    }

    for(size_t i = 0; i < color_count; ++i)
    for(size_t j = i+1; j < color_count; ++j)
    {
        float ri = colors[i].r, gi = colors[i].g, bi = colors[i].b;
        float rj = colors[j].r, gj = colors[j].g, bj = colors[j].b;

        LAB_ColorHDR ci = LAB_HDR_RGB_F(ri, gi, bi);
        LAB_ColorHDR cj = LAB_HDR_RGB_F(rj, gj, bj);
        LAB_ColorHDR cs = LAB_AddColorHDR(ci, cj);
        LAB_ColorHDR cp = LAB_MulColorHDR(ci, cj);
        LAB_ColorHDR c1 = LAB_SubColorHDR(ci, cj);
        LAB_ColorHDR c2 = LAB_SubColorHDR(cj, ci);
        LAB_ColorHDR cx = LAB_MaxColorHDR(ci, cj);

        printf("ci(%f, %f, %f), cj(%f, %f, %f)\n", ri, gi, bi, rj, gj, bj);
        printf("+: %f, %f, %f = %f, %f, %f\n", ri+rj, gi+gj, bi+bj, LAB_HDR_RED_VAL(cs), LAB_HDR_GRN_VAL(cs), LAB_HDR_BLU_VAL(cs));
        printf("*: %f, %f, %f = %f, %f, %f\n", ri*rj, gi*gj, bi*bj, LAB_HDR_RED_VAL(cp), LAB_HDR_GRN_VAL(cp), LAB_HDR_BLU_VAL(cp));
        printf("-: %f, %f, %f = %f, %f, %f\n", ri-rj, gi-gj, bi-bj, LAB_HDR_RED_VAL(c1), LAB_HDR_GRN_VAL(c1), LAB_HDR_BLU_VAL(c1));
        printf("-: %f, %f, %f = %f, %f, %f\n", rj-ri, gj-gi, bj-bi, LAB_HDR_RED_VAL(c2), LAB_HDR_GRN_VAL(c2), LAB_HDR_BLU_VAL(c2));
        printf(">: %f, %f, %f = %f, %f, %f\n", LAB_MAX(rj,ri), LAB_MAX(gj,gi), LAB_MAX(bj,bi), LAB_HDR_RED_VAL(cx), LAB_HDR_GRN_VAL(cx), LAB_HDR_BLU_VAL(cx));
    }
}
#endif