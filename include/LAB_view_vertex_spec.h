#pragma once

#include "LAB_stdinc.h"

#include "LAB_gl_types.h"

typedef struct LAB_View_VertexElementSpec
{
    const char* name;
    LAB_GL_Enum type;
    size_t dimension;
    size_t offset;
    bool normalized;
    bool keep_as_integer;
} LAB_View_VertexElementSpec;

typedef struct LAB_View_VertexSpec
{
    size_t vertex_size;
    size_t elements_count;
    const LAB_View_VertexElementSpec* elements;
} LAB_View_VertexSpec;


void LAB_View_VertexSpec_Realize(LAB_GL_Array vao, LAB_GL_Buffer vbo, const LAB_View_VertexSpec* spec);
void LAB_View_VertexSpec_Realize_Format(LAB_GL_Array vao, const LAB_View_VertexSpec* spec);
void LAB_View_VertexSpec_Realize_Buffer(LAB_GL_Array vao, LAB_GL_Buffer vbo, const LAB_View_VertexSpec* spec);
void LAB_View_VertexSpec_Disable(LAB_GL_Array vao, const LAB_View_VertexSpec* spec);