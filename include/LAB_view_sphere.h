#pragma once
#error

#include "LAB_view_chunk.h"

struct LAB_ViewSphere_YElement
{
    size_t x_radius;
    struct LAB_ViewChunkEntry* x_arr;
};

struct LAB_ViewSphere_ZElement
{
    size_t y_radius; // exclusive
    struct LAB_ViewSphere_YElement* y_arr;
};


typedef struct LAB_ViewSphere
{
    size_t radius;
    struct LAB_ViewSphere_ZElement* z_arr;

} LAB_ViewSphere;