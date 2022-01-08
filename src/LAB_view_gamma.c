#include "LAB_view_gamma.h"
#include "LAB_math.h"



#define LAB_PP_MAP_IOTA_d3(fn, start) \
    fn((start+0)) \
    fn((start+1)) \
    fn((start+2)) \
    fn((start+3))

#define LAB_PP_MAP_IOTA_d2(fn, start) \
    LAB_PP_MAP_IOTA_d3(fn, start+0) \
    LAB_PP_MAP_IOTA_d3(fn, start+4) \
    LAB_PP_MAP_IOTA_d3(fn, start+8) \
    LAB_PP_MAP_IOTA_d3(fn, start+12)

#define LAB_PP_MAP_IOTA_d1(fn, start) \
    LAB_PP_MAP_IOTA_d2(fn, start+0) \
    LAB_PP_MAP_IOTA_d2(fn, start+16) \
    LAB_PP_MAP_IOTA_d2(fn, start+32) \
    LAB_PP_MAP_IOTA_d2(fn, start+48)

#define LAB_PP_MAP_IOTA_255(fn) \
    LAB_PP_MAP_IOTA_d1(fn, 0) \
    LAB_PP_MAP_IOTA_d1(fn, 64) \
    LAB_PP_MAP_IOTA_d1(fn, 128) \
    LAB_PP_MAP_IOTA_d1(fn, 192)



const LAB_View_GammaMap LAB_gamma_dark =
{
#define LAB_X(l) l,
    .map = {
        [0] = { LAB_PP_MAP_IOTA_255(LAB_X) },
        [1] = { LAB_PP_MAP_IOTA_255(LAB_X) },
        [2] = { LAB_PP_MAP_IOTA_255(LAB_X) },
    },
#undef LAB_X
};

const LAB_View_GammaMap LAB_gamma_light =
{
//#define LAB_X(l) 255-((255-l)*(255-l)*(255-l))/255/255,
#define LAB_X(l) LAB_SQRT_I8(l),
    .map = {
        [0] = { LAB_PP_MAP_IOTA_255(LAB_X) },
        [1] = { LAB_PP_MAP_IOTA_255(LAB_X) },
        [2] = { LAB_PP_MAP_IOTA_255(LAB_X) },
    },
#undef LAB_X
};


#if 0
void LAB_Test_Gamma(void)
{
    int min_diff = 0;
    int max_diff = 0;
    for(int i = 0; i < 256; ++i)
    {
        int ii = LAB_gamma_light.map[0][i]*LAB_gamma_light.map[0][i]/255;
        printf("%3i %3i\n", i, ii);
        if(i-ii < min_diff) min_diff = i-ii;
        if(i-ii > max_diff) max_diff = i-ii;
    }
    printf("MIN: %i, MAX: %i\n", min_diff, max_diff);
}
#endif