#define LAB_ColorHDR uint
#define LAB_HDR_RED(col) ((col)       & 0xffu)
#define LAB_HDR_GRN(col) ((col) >>  8 & 0xffu)
#define LAB_HDR_BLU(col) ((col) >> 16 & 0xffu)
#define LAB_HDR_EXP(col) ((col) >> 24)

#define LAB_HDR_EXP_VALUE(col) (int(LAB_HDR_EXP(col))-128-8)


vec3 LAB_ColorHDR_ToVec3(LAB_ColorHDR c)
{
    vec3 mantissa = vec3(LAB_HDR_RED(c), LAB_HDR_GRN(c), LAB_HDR_BLU(c));
    ivec3 exponent = ivec3(LAB_HDR_EXP_VALUE(c));
    return ldexp(mantissa, exponent);
}

vec4 LAB_ColorHDR_ToVec4(LAB_ColorHDR c)
{
    return vec4(LAB_ColorHDR_ToVec3(c), 1.0f);
}
