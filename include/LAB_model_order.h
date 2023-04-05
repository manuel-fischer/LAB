#pragma once

#include "LAB_model.h"

#include "LAB_vec.h"

#include <stdint.h>
#include <assert.h>

typedef struct LAB_TriangleOrder
{
    uint32_t v[3]; // for sorting: [0] is kept [1] gets the distance, [2] is used for buckets
} LAB_TriangleOrder;

static_assert(sizeof(uint32_t)==sizeof(float), "");

#define LAB_TRIANGLE_DIST(tri)   ((tri).v[2])
#define LAB_TRIANGLE_DIST_F(tri) (*(float*)&(tri).v[2])


void LAB_BuildModelOrder(LAB_TriangleOrder* order, size_t triangle_count);

/**
 *  cam2 contains the relative coordinates to the mesh multiplied by two
 */
void LAB_SortModelOrder2(LAB_TriangleOrder* order, LAB_Triangle const* mesh,
                         size_t triangle_count, LAB_Vec3F cam2);

LAB_INLINE
void LAB_SortModelOrder(LAB_TriangleOrder* order, LAB_Triangle const* mesh,
                        size_t triangle_count, LAB_Vec3F cam)
{
    LAB_Vec3F cam2 = LAB_Vec3F_RMul(cam, 2);
    LAB_SortModelOrder2((order), (mesh), (triangle_count), cam2); \
}
