#pragma once

#include "LAB_attr.h"
#include "LAB_stdinc.h"
#include "LAB_model.h"
#include "LAB_memory.h"

//#define m_size size

// If capacity == 0/data == NULL, the data might be changed in another thread
typedef struct LAB_View_Mesh
{
    size_t m_size, capacity;
    LAB_Triangle* data;

    unsigned vbo;
    size_t vbo_size;
} LAB_View_Mesh;


void LAB_View_Mesh_Destroy(LAB_View_Mesh* mesh);
LAB_Triangle* LAB_View_Mesh_Alloc(LAB_View_Mesh* mesh, size_t add_size);