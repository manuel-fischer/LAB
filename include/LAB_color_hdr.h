#pragma once
#define LAB_DBG_HDR 1

#include "LAB_color.h"
#include "LAB_bits.h"
#include "LAB_functional.h"
#include "LAB_select.h"

#include <math.h>

// Note: no support for alpha channel, used as exponent instead
typedef uint32_t LAB_ColorHDR;

#define LAB_RGBE_HDR(r, g, b, e) LAB_CXADD4((uint32_t)(r), (uint32_t)(g) << 8, (uint32_t)(b) << 16, (uint32_t)(e) << 24)

#define LAB_RGBE_HDR_N(r, g, b, e) LAB_ColorHDR_Normalize(LAB_RGBE_HDR(r, g, b, e))


#define LAB_RGBI_IMPL_RGB(rgb) (((uint32_t)(rgb) << 16 & 0xff0000u) | ((uint32_t)(rgb) & 0xff00u) | ((uint32_t)(rgb) >> 16 & 0xffu))
#define LAB_RGBI_HDR_DEN(rgb, exp_val) (LAB_RGBI_IMPL_RGB(rgb) | (uint32_t)((exp_val)+128) << 24)

// only use in constants!
// should not be subnormal
#define LAB_COLOR_HDR_NORMALIZE(c) ((uint32_t)( \
    (c) & 0xffffffu  \
      ? ( \
            (c) & 0xf0f0f0u \
              ? (c) & 0xc0c0c0u \
                  ? (c) & 0x808080u \
                      ?  (c)      & 0xffffffu \
                      : ((c) << 1 & 0xfefefeu) - 0x01000000u \
                  : (c) & 0x202020u \
                      ? ((c) << 2 & 0xfcfcfcu) - 0x02000000u \
                      : ((c) << 3 & 0xf8f8f8u) - 0x03000000u \
              : (c) & 0x0c0c0cu \
                  ? (c) & 0x080808u \
                      ? ((c) << 4 & 0xf0f0f0u) - 0x04000000u \
                      : ((c) << 5 & 0xe0e0e0u) - 0x05000000u \
                  : (c) & 0x020202u \
                      ? ((c) << 6 & 0xc0c0c0u) - 0x06000000u \
                      : ((c) << 7 & 0x808080u) - 0x07000000u \
        ) + ((c) & 0xff000000u) \
      : 0u \
))

//#define LAB_RGBI_HDR(rgb, exp_val) LAB_COLOR_HDR_NORMALIZE(LAB_RGBI_HDR_DEN(rgb, exp_val))

#define LAB_COLOR_HDR_ENSURE_NORMALIZED(c) (((c) & 0xff000000u) == 0u || ((c) & 0x00808080u) != 0u ? (c) : 0u)


#define LAB_RGBI_HDR(rgb, exp_val) LAB_COLOR_HDR_ENSURE_NORMALIZED(LAB_RGBI_HDR_DEN(rgb, exp_val))


#define LAB_HDR_RED(col) ((col)       & 0xffu)
#define LAB_HDR_GRN(col) ((col) >>  8 & 0xffu)
#define LAB_HDR_BLU(col) ((col) >> 16 & 0xffu)
#define LAB_HDR_EXP(col) ((col) >> 24 & 0xffu)

#define LAB_HDR_EXP_VALUE(col) ((int)LAB_HDR_EXP(col)-128)

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



// Exp set to 0
LAB_CONST
LAB_INLINE uint32_t LAB_ColorHDRMantissa_ShiftRight(uint32_t mantissa, int shift_right)
{
    return ((mantissa >> shift_right) & ((0x01010100u >> shift_right) - 0x00010101u));
}

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
    return exp_bits | LAB_ColorHDRMantissa_ShiftRight(c, ds);
}


LAB_CONST
LAB_INLINE bool LAB_ColorHDR_IsNormal(LAB_ColorHDR c)
{
    // either one channel is at least 128 or the exponent is minimal
    return !!(c & 0x00808080) | ((c & 0xff000000) == 0);
}

#if LAB_DBG_HDR
#define LAB_ColorHDR_R(c) (LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(c), #c "=%8x", c), (c))
#define LAB_ColorHDR_R_FMT(c,fmt,...) (LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(c), #c "=%8x, " fmt, c, __VA_ARGS__), (c))
#else
#define LAB_ColorHDR_R(c) (c)
#define LAB_ColorHDR_R_FMT(c,fmt,...) (c)
#endif

#define LAB_ColorHDR_R_AB(c) LAB_ColorHDR_R_FMT(c, "a=%8x, b=%8x", a, b)


#define LAB_HDR_NORMALIZE_SHIFT_EXPONENT(shift, exponent) do { \
    LAB_ASSERT_FMT((exponent) >= 0, "%i", (exponent)); \
    (exponent) -= (shift); \
    (shift) += LAB_SELECT0((exponent) < 0, (exponent)); /* decrementing */ \
    (exponent) = LAB_BOUND0(exponent); \
} while(0)

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_ColorHDR_Normalize(LAB_ColorHDR c)
{
    if(!(c & 0x00ffffffu)) return 0; // branch nessecary: Log2 UB for 0
    uint32_t occupied = (c | c >> 8 | c >> 16) & 0xffu;
    //if(!occupied) return 0;
    int shift = 7-LAB_Log2Floor(occupied);

    int new_exp = LAB_HDR_EXP(c);
    LAB_HDR_NORMALIZE_SHIFT_EXPONENT(shift, new_exp);

    // no masking between channels nessecary, because bits shifted into larger channels are 0
    uint32_t rgb_bits = c << shift & 0x00ffffffu;

    uint32_t exp_bits = (uint32_t)new_exp << 24;
    LAB_ASSERT_FMT(!(rgb_bits & ~0x00ffffffu), "exp_bits=%8x, c=%8x", exp_bits, c);
    return LAB_ColorHDR_R(exp_bits | rgb_bits); 
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
    bool shift = !!(0x01000100u&sum_rb) | !!(0x00010000u&sum_g);
    //bool shift = (sum_rb >> 8 | sum_rb >> 24 | sum_g >> 16) & 1;

    // increase exponent and shift channels on overflow
    sum_rb >>= shift; sum_rb &= 0x00ff00ffu;
    sum_g  >>= shift; sum_g  &= 0x0000ff00u;

    uint32_t exp = (b & 0xff000000u) + (shift << 24);

    return LAB_ColorHDR_R_AB(sum_rb | sum_g | exp);
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
LAB_INLINE LAB_ColorHDR LAB_AbsDiffColorHDR(LAB_ColorHDR a, LAB_ColorHDR b)
{
    return LAB_AddColorHDR(LAB_SubColorHDR(a,b), LAB_SubColorHDR(b,a));
}


LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_AddSubColorHDR(LAB_ColorHDR a, LAB_ColorHDR b_pos, LAB_ColorHDR b_neg)
{
    int common_exp = LAB_MAX3_BL(LAB_HDR_EXP(a), LAB_HDR_EXP(b_pos), LAB_HDR_EXP(b_neg));
    
    a = LAB_ColorHDR_ShiftRightTo(a, common_exp);
    b_pos = LAB_ColorHDR_ShiftRightTo(b_pos, common_exp);
    b_neg = LAB_ColorHDR_ShiftRightTo(b_neg, common_exp);

    uint32_t result_r = LAB_BOUND0((int)LAB_HDR_RED(a)+(int)LAB_HDR_RED(b_pos)-(int)LAB_HDR_RED(b_neg));
    uint32_t result_g = LAB_BOUND0((int)LAB_HDR_GRN(a)+(int)LAB_HDR_GRN(b_pos)-(int)LAB_HDR_GRN(b_neg));
    uint32_t result_b = LAB_BOUND0((int)LAB_HDR_BLU(a)+(int)LAB_HDR_BLU(b_pos)-(int)LAB_HDR_BLU(b_neg));

    LAB_HDR_ASSERT_FMT(result_r <= 510u, "%i", result_r);
    LAB_HDR_ASSERT_FMT(result_g <= 510u, "%i", result_g);
    LAB_HDR_ASSERT_FMT(result_b <= 510u, "%i", result_b);

    uint32_t occupied = result_r|result_g|result_b;
    if(!occupied) return 0;

    // check if overflow happened
    bool ov = !!(occupied & 0x100);
    common_exp += ov;
    result_r >>= ov;
    result_g >>= ov;
    result_b >>= ov;
    occupied >>= ov;

    LAB_HDR_ASSERT_FMT(result_r < 256u, "%i", result_r);
    LAB_HDR_ASSERT_FMT(result_g < 256u, "%i", result_g);
    LAB_HDR_ASSERT_FMT(result_b < 256u, "%i", result_b);
    LAB_HDR_ASSERT_FMT(occupied < 256u, "%i", occupied);

    int shift = 7-LAB_Log2Floor(occupied);
    LAB_HDR_NORMALIZE_SHIFT_EXPONENT(shift, common_exp);

    uint32_t c = result_r | result_g << 8 | result_b << 16;

    uint32_t exp_bits = (uint32_t)common_exp << 24;
    return LAB_ColorHDR_R(exp_bits | c << shift);
}


LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_ColorHDR_ImplCompose(uint32_t cr, uint32_t cg, uint32_t cb, int exp, LAB_ColorHDR a, LAB_ColorHDR b)
{
    // combine bits to one integer, such that the exponent can be calculated
    uint32_t occupied = cr|cg|cb;
    if(occupied == 0) return 0; // branch necessary here, because Log2Floor does not accept 0

    int log2 = LAB_Log2Floor(occupied);
    //LAB_HDR_ASSERT_FMT(log2 >= 7, "log2 = %i, a = %8x, b = %8x", log2, a, b);
    LAB_HDR_ASSERT_FMT(log2 < 16, "log2 = %i, a = %8x, b = %8x", log2, a, b);
    int shift = log2-7; // shift from 16 back to 8 bits
    LAB_HDR_ASSERT_FMT(shift <= 8, "shift = %i, a = %8x, b = %8x", shift, a, b);

    exp += shift;

    shift -= LAB_SELECT0(exp < 0, exp); // increment: exp negative
    shift = LAB_MIN_BL(shift, 16); // avoid UB: if shift=16, channels zero anyway
    exp = LAB_SELECT0(exp >= 0, exp);

    int shift_right = LAB_SELECT0(shift > 0, shift);
    int shift_left = LAB_SELECT0(shift < 0, -shift);

    // Case where left shift is necessary: a = 68b00500, b = 7a0001dd

    cr >>= shift_right; cr <<= shift_left;
    cg >>= shift_right; cg <<= shift_left;
    cb >>= shift_right; cb <<= shift_left;

    LAB_HDR_ASSERT_FMT(cr < 256u, "prod_r = %i", cr);
    LAB_HDR_ASSERT_FMT(cg < 256u, "prod_g = %i", cg);
    LAB_HDR_ASSERT_FMT(cb < 256u, "prod_b = %i", cb);

    uint32_t c = cr | cg << 8 | cb << 16 | (uint32_t)exp << 24;
    return LAB_ColorHDR_R_AB(c);
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

    // sub 128 once, because both have 128 added onto their exponent-value
    int exp = (int)(LAB_HDR_EXP(a) + LAB_HDR_EXP(b)) - 128-8;

    return LAB_ColorHDR_ImplCompose(prod_r, prod_g, prod_b, exp, a, b);
}



LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_MulColorHDR_RoundUp(LAB_ColorHDR a, LAB_ColorHDR b)
{
    // just round up to avoid 0 channel value
    LAB_ColorHDR c = LAB_MulColorHDR(a, b);
    // if c == 0 and a != 0 and b != 0
    int cr = LAB_SELECT0(!(c & 0x000000ffu) & !!(a & 0x000000ffu) & !!(b & 0x000000ffu), 0x00000001u);
    int cg = LAB_SELECT0(!(c & 0x0000ff00u) & !!(a & 0x0000ff00u) & !!(b & 0x0000ff00u), 0x00000100u);
    int cb = LAB_SELECT0(!(c & 0x00ff0000u) & !!(a & 0x00ff0000u) & !!(b & 0x00ff0000u), 0x00010000u);
    return LAB_ColorHDR_R_AB(c|cr|cg|cb);
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
LAB_INLINE LAB_ColorHDR LAB_MinColorHDR(LAB_ColorHDR a, LAB_ColorHDR b)
{
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(a), "a = %8x", a);
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(b), "b = %8x", b);

    // ensure a < b
    LAB_COND_SWAP_T(LAB_ColorHDR, a > b, a, b);

    // avoid UB of shift
    int shift = LAB_MIN_BL(LAB_HDR_EXP(b) - LAB_HDR_EXP(a), 8);

    uint32_t ar_bits = a&0x000000ffu, br_bits = (b&0x000000ffu) << shift;
    uint32_t ag_bits = a&0x0000ff00u, bg_bits = (b&0x0000ff00u) << shift;
    uint32_t ab_bits = a&0x00ff0000u, bb_bits = (b&0x00ff0000u) << shift;

    uint32_t r_bits = LAB_MIN_BL(ar_bits, br_bits);
    uint32_t g_bits = LAB_MIN_BL(ag_bits, bg_bits);
    uint32_t b_bits = LAB_MIN_BL(ab_bits, bb_bits);

    LAB_HDR_ASSERT_FMT(!(r_bits & ~0x000000ffu), "%8x", r_bits);
    LAB_HDR_ASSERT_FMT(!(g_bits & ~0x0000ff00u), "%8x", g_bits);
    LAB_HDR_ASSERT_FMT(!(b_bits & ~0x00ff0000u), "%8x", b_bits);

    uint32_t e_bits = a & 0xff000000u;

    uint32_t retval = r_bits | g_bits | b_bits | e_bits;

    LAB_HDR_ASSERT_FMT(retval <= LAB_MIN_BL(a, b), "retval=%8x, a=%8x, b=%8x", retval, a, b);

    return LAB_ColorHDR_Normalize(retval);
}


/*LAB_CONST
LAB_INLINE uint32_t LAB_Fix1Dot8_Sqrt(uint32_t x)
{
}


LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_ColorHDR_FastSqrt(LAB_ColorHDR c)
{
    uint32_t exp_bits = (c >> 1 & 0xff000000) + (64 << 24);
    bool uneven_shift = !!(exp_bits & 0x01000000);
    // 9 bit numbers: 1.8 fixed point
    int r = LAB_HDR_RED(c) << uneven_shift;
    int g = LAB_HDR_GRN(c) << uneven_shift;
    int b = LAB_HDR_BLU(c) << uneven_shift;
}*/


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

LAB_CONST
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
    return LAB_ColorHDR_R_FMT(c0|c1|c2, "r=%f, g=%f, b=%f", r, g, b);
}



LAB_CONST
LAB_INLINE float LAB_ColorHDR_ChannelValueF(uint8_t mantissa, uint8_t exp)
{
    return ldexp((float)mantissa, (int)exp-8-128);
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


typedef uint32_t LAB_ColorHDR_Comparable;
LAB_CONST
LAB_INLINE LAB_ColorHDR_Comparable LAB_ColorHDR_MaxChannelComparable(LAB_ColorHDR c)
{
    uint32_t v = LAB_MAX3_BL(LAB_HDR_RED(c), LAB_HDR_GRN(c), LAB_HDR_BLU(c));
    return v | (c & 0xff000000u);
}

LAB_CONST
LAB_INLINE LAB_ColorHDR_Comparable LAB_ColorHDR_LuminanceComparable(LAB_ColorHDR c)
{
    uint32_t r = LAB_HDR_RED(c);
    uint32_t g = LAB_HDR_GRN(c);
    uint32_t b = LAB_HDR_BLU(c);

    //uint32_t l = 2126*r + 7152*g + 722*b;
    uint32_t l = 54*r + 183*g + 19*b; // ratios relative to 1<<8

    int shift = 23-LAB_Log2Floor(l); // factor to shift left
    int e = LAB_HDR_EXP(c)-shift;
    shift = LAB_SELECT0(e > 0, shift);
    e = LAB_SELECT0(e > 0, e);

    int shift_l = LAB_SELECT0(shift > 0,  shift);
    int shift_r = LAB_SELECT0(shift < 0, -shift);

    l <<= shift_l; l >>= shift_r;

    return l | e<<24;
}


LAB_CONST
LAB_INLINE float LAB_ColorHDR_Luminance(LAB_ColorHDR c)
{
    uint32_t r = LAB_HDR_RED(c);
    uint32_t g = LAB_HDR_GRN(c);
    uint32_t b = LAB_HDR_BLU(c);
    int exp = LAB_HDR_EXP(c);

    //float l = (2126*r + 7152*g + 722*b)*0.0001;
    //return ldexp((float)l, (int)exp-8-128);

    //uint32_t l = 3483*r + 11718*g + 1183*b; // ratios relative to 1<<14
    //return ldexp((float)l, exp-8-128-14);

    uint32_t l = 54*r + 183*g + 19*b; // ratios relative to 1<<8
    return ldexp((float)l, exp-8-128-8);
}


LAB_CONST
LAB_INLINE LAB_Color LAB_ColorHDR_To_Color_Overflow(LAB_ColorHDR c, LAB_ColorHDR overflow_color)
{
    #define LAB_ColorHDR_R_HERE(retval) (retval) //LAB_ColorHDR_R_FMT(retval, "c=%8x, oc=%8x", c, overflow_color)
    if(c < 0x81000000u) // up to exp = 128
    {
        c = LAB_ColorHDR_ShiftRightTo(c, 128);
        return LAB_ColorHDR_R_HERE(c | 0xff000000u); // alpha = 255
    }
    else
    {
        LAB_ColorHDR ov = LAB_SubColorHDR(c, LAB_HDR_UNIT_WHITE);
        ov = LAB_ColorHDR_MaxChannelGray(ov);

        c = LAB_AddColorHDR(c, LAB_MulColorHDR(ov, overflow_color));

        uint8_t r = LAB_ColorHDR_ChannelValueU8Saturate(LAB_HDR_RED(c), LAB_HDR_EXP(c));
        uint8_t g = LAB_ColorHDR_ChannelValueU8Saturate(LAB_HDR_GRN(c), LAB_HDR_EXP(c));
        uint8_t b = LAB_ColorHDR_ChannelValueU8Saturate(LAB_HDR_BLU(c), LAB_HDR_EXP(c));

        return LAB_ColorHDR_R_HERE(r | g << 8 | b << 16 | 0xff000000u);
    }
    #undef LAB_ColorHDR_R_HERE
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_Color_To_ColorHDR_Fast(LAB_Color c)
{
    LAB_ColorHDR rgb = c & 0x00ffffff;

    return LAB_ColorHDR_Normalize(rgb | 0x80000000);
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_Color_To_ColorHDR(LAB_Color c)
{
    int r = LAB_RED(c); r += r >> 7;
    int g = LAB_GRN(c); g += g >> 7;
    int b = LAB_BLU(c); b += b >> 7;

    bool shr = !!((r|g|b) & 0x100);
    r >>= shr;
    g >>= shr;
    b >>= shr;

    return LAB_RGBE_HDR_N(r, g, b, 0x80 + shr);
}



LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_MulColorHDRExp2(LAB_ColorHDR c, int shift_left)
{
    LAB_HDR_ASSERT_FMT(LAB_ColorHDR_IsNormal(c), "c = %8x", c);

    int new_exp = (int)LAB_HDR_EXP(c) + shift_left;
    int mantissa_shift = LAB_SELECT0(new_exp < 0, -new_exp);
    new_exp = LAB_SELECT0(new_exp >= 0, new_exp);
    new_exp = LAB_SELECT0(c, new_exp); // Special case: black

    uint32_t exp_bits = (uint32_t)new_exp << 24;
    uint32_t mnt_bits = LAB_ColorHDRMantissa_ShiftRight(c, mantissa_shift);

    LAB_HDR_ASSERT_FMT(!(mnt_bits & ~0x00ffffffu), "c=%8x, mnt_bits=%8x", c, mnt_bits);

    //return LAB_ColorHDR_R_FMT(exp_bits | mnt_bits, "c=%8x, shift_left=%8x", c, shift_left);
    // Not always normalized, example: darkest red: c=1, shift = 2
    return LAB_ColorHDR_Normalize(exp_bits | mnt_bits);
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_MixColorHDR50(LAB_ColorHDR a, LAB_ColorHDR b)
{
    return LAB_MulColorHDRExp2(LAB_AddColorHDR(a, b), -1);
}

LAB_CONST
LAB_INLINE LAB_Color LAB_MixColorHDR4x25(LAB_Color a, LAB_Color b,
                                      LAB_Color c, LAB_Color d)
{
    return LAB_MulColorHDRExp2(LAB_REDUCE_4(LAB_AddColorHDR, a, b, c, d), -2);
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_InterpolateColorHDR2i(LAB_ColorHDR a, LAB_ColorHDR b, int m)
{
    // ensure a < b
    // compute 256-m conditionally
    m -= LAB_SELECT0(a > b, 1);
    m ^= LAB_SELECT0(a > b, 0xff);
    m &= 0x1ff;
    LAB_COND_SWAP_T(LAB_ColorHDR, a > b, a, b);

    a = LAB_ColorHDR_ShiftRightTo(a, LAB_HDR_EXP(b));

    int cr = ((int)LAB_HDR_RED(a) << 8) + ((int)LAB_HDR_RED(b)-(int)LAB_HDR_RED(a))*m;
    int cg = ((int)LAB_HDR_GRN(a) << 8) + ((int)LAB_HDR_GRN(b)-(int)LAB_HDR_GRN(a))*m;
    int cb = ((int)LAB_HDR_BLU(a) << 8) + ((int)LAB_HDR_BLU(b)-(int)LAB_HDR_BLU(a))*m;

    int exp = LAB_HDR_EXP(b)-8;

    return LAB_ColorHDR_ImplCompose(cr, cg, cb, exp, a, b);
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_InterpolateColorHDR4vi(const LAB_ColorHDR colors[LAB_RESTRICT 4],
                                             int u, int v)
{
    return LAB_InterpolateColorHDR2i(
                LAB_InterpolateColorHDR2i(colors[0], colors[1], u),
                LAB_InterpolateColorHDR2i(colors[2], colors[3], u),
                v
    );
}

LAB_CONST
LAB_INLINE LAB_ColorHDR LAB_InterpolateColorHDR8vi(const LAB_ColorHDR colors[LAB_RESTRICT 8],
                                             int u, int v, int w)
{
    return LAB_InterpolateColor2i(
                LAB_InterpolateColorHDR4vi(colors, u, v),
                LAB_InterpolateColorHDR4vi(colors+4, u, v),
                w
    );
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