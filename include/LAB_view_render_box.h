#pragma once

#include "LAB_view_renderer.h"
#include "LAB_vec3.h"
#include "LAB_gl_types.h"

typedef struct LAB_BoxRenderer
{
    LAB_GL_Array vao;
    LAB_GL_Buffer vbo;

    // STATE:
    LAB_Mat4F modelproj_mat;
    LAB_Vec3F pos;
    LAB_GL_Uniform uni_modelproj;
} LAB_BoxRenderer;

bool LAB_BoxRenderer_Create(LAB_BoxRenderer* r);
void LAB_BoxRenderer_Destroy(LAB_BoxRenderer* r);

void LAB_BoxRenderer_Prepare(LAB_BoxRenderer* r, LAB_ViewRenderer* renderer, LAB_Mat4F modelproj_mat, LAB_Vec3D pos);

void LAB_RenderBox(LAB_BoxRenderer* r, LAB_Box3F box);