#pragma once

#include "LAB_model.h"

#include <stdint.h>

typedef struct LAB_TriangleOrder
{
    uint32_t v[3]; // for sorting: [0] is kept [1] gets the distance, [2] is used for buckets
} LAB_TriangleOrder;

_Static_assert(sizeof(uint32_t)==sizeof(float), "");

#define LAB_TRIANGLE_DIST(tri)   ((tri).v[2])
#define LAB_TRIANGLE_DIST_F(tri) (*(float*)&(tri).v[2])


void LAB_BuildModelOrder(LAB_TriangleOrder* order, size_t triangle_count);

/**
 *  cam2 contains the relative coordinates to the mesh multiplied by two
 */
void LAB_SortModelOrder2(LAB_TriangleOrder* order, LAB_Triangle const* mesh,
                         size_t triangle_count, float cam2[3]);

#define LAB_SortModelOrder(order, mesh, triangle_count, cam) do \
{ \
    float cam2[3]; \
    memcpy(cam2, (cam), sizeof(cam2)); \
    cam2[0] *= 2; \
    cam2[1] *= 2; \
    cam2[2] *= 2; \
    LAB_SortModelOrder2((order), (mesh), (triangle_count), cam2); \
} while(0)
