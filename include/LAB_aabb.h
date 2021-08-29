#pragma once

#include "LAB_vec.h"

/*typedef struct LAB_AABB
{
    float min[3], max[3];
} LAB_AABB;*/

#define LAB_AABB_FULL_CUBE {{0, 0, 0}, {1, 1, 1}}



#define LAB_AABB2_Assign(aabb, x0, y0, x1, y1) do \
{ \
    (aabb)[0][0] = (x0); \
    (aabb)[0][1] = (y0); \
    (aabb)[1][0] = (x1); \
    (aabb)[1][1] = (y1); \
} \
while(0)

#define LAB_AABB2_AssignSized(aabb, x0, y0, w, h) do \
{ \
    (aabb)[0][0] = (x0); \
    (aabb)[0][1] = (y0); \
    (aabb)[1][0] = (x0)+(w); \
    (aabb)[1][1] = (y0)+(h); \
} \
while(0)


#define LAB_AABB3_Assign(aabb, x0, y0, z0, x1, y1, z1) do \
{ \
    (aabb)[0][0] = (x0); \
    (aabb)[0][1] = (y0); \
    (aabb)[0][2] = (z0); \
    (aabb)[1][0] = (x1); \
    (aabb)[1][1] = (y1); \
    (aabb)[1][2] = (z1); \
} \
while(0)
