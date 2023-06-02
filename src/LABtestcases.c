//#define LAB_COLOR_HDR_TEST

// include it here, such that headers do not need to include them
#include <stdio.h>
#include <stdlib.h>


#include "LAB_color_hdr.h"
#include "LAB_mat.h"


//#define LAB_MAT4_TEST

#ifdef LAB_MAT4_TEST
static void LAB_PrintMat4F(LAB_Mat4F a)
{
    const float (*aa)[4] = LAB_Mat4F_AsCArray2(&a);
    printf("[");
    for(int i = 0; i < 4; ++i)
    {
        if(i) printf("\n");
        for(int j = 0; j < 4; ++j)
        {
            printf("%s", "  " + !!(i==0 || j!=0));
            printf("%6.3f", aa[j][i]);
        }
    }
    printf(" ]\n");
}
#endif

void LAB_DoTestCases(void)
{
    #ifdef LAB_COLOR_HDR_TEST
    LAB_ColorHDR_DoTestCases();
    #endif

    #ifdef LAB_MAT4_TEST
    {
        srand(12345);
        LAB_Mat4F a;
        float (*aa)[4] = LAB_Mat4F_AsArray2(&a);
        for(int i = 0; i < 4; ++i)
        for(int j = 0; j < 4; ++j)
            aa[j][i] = ((float)rand() / (float)RAND_MAX)*2-1;

        LAB_PrintMat4F(a);

        LAB_Mat4F inv_a = LAB_Mat4F_Invert(a);
        LAB_PrintMat4F(inv_a);
        LAB_Mat4F ident = LAB_Mat4F_Chain(a, inv_a);
        LAB_PrintMat4F(ident);

    }
    #endif

}