//#define LAB_COLOR_HDR_TEST

// include it here, such that headers do not need to include them
#include <stdio.h>


#include "LAB_color_hdr.h"

void LAB_DoTestCases(void)
{
    #ifdef LAB_COLOR_HDR_TEST
    LAB_ColorHDR_DoTestCases();
    #endif
}