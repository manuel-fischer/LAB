#pragma once

#include "LAB_view_renderer.h"
#include "LAB_vec3.h"
#include "LAB_mat.h"
#include "LAB_gl_types.h"

typedef struct LAB_SkyRenderer
{
    LAB_GL_Array vao;
    LAB_GL_Buffer vbo;
} LAB_SkyRenderer;

typedef struct LAB_RenderSkyAttrs
{
    LAB_Mat4F modelproj;
    LAB_FogAttrs fog;
    float time;
} LAB_RenderSkyAttrs;


bool LAB_SkyRenderer_Create(LAB_SkyRenderer* r);
void LAB_SkyRenderer_Destroy(LAB_SkyRenderer* r);

void LAB_SkyRenderer_Prepare(LAB_SkyRenderer* r, LAB_ViewRenderer* renderer, LAB_RenderSkyAttrs attrs);
void LAB_SkyRenderer_Render(LAB_SkyRenderer* r);