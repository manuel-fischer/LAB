#pragma once

#include <stdlib.h>
#include "LAB_color.h"
#include "LAB_color_hdr.h"

#define LAB_DIRECTIONAL_LIGHT 1
#if LAB_DIRECTIONAL_LIGHT == 0
typedef LAB_Color LAB_LightNode;
#else
typedef struct LAB_LightNode
{
    // A light value is stored for each corner of the cube
    // That light value can only propegate towards the directions
    // which it faces
    LAB_ColorHDR quadrants[8];
} LAB_LightNode;
#endif
