#pragma once

#include "LAB_view_renderer.h"
#include "LAB_vec3.h"
#include "LAB_gl_types.h"

typedef struct LAB_SurfaceRenderer
{
    LAB_GL_Array vao;
    LAB_GL_Buffer vbo;

    // STATE:
    LAB_GL_Uniform uni_origin;
    LAB_GL_Uniform uni_size;
    LAB_GL_Uniform uni_tex_size;
    LAB_GL_Uniform uni_color;
} LAB_SurfaceRenderer;


bool LAB_SurfaceRenderer_Create(LAB_SurfaceRenderer* r);
void LAB_SurfaceRenderer_Destroy(LAB_SurfaceRenderer* r);

void LAB_SurfaceRenderer_Prepare(LAB_SurfaceRenderer* r, LAB_ViewRenderer* renderer);

// surface_rect in 0 to 1 range
void LAB_RenderSurface(LAB_SurfaceRenderer* r, LAB_GL_Texture tex, LAB_Box2F surface_rect, LAB_Vec2I tex_size, LAB_Color color);

LAB_INLINE
void LAB_RenderSurface_At(LAB_SurfaceRenderer* r, LAB_GL_Texture tex, LAB_Vec2I screen_size, LAB_Vec2I pos, LAB_Vec2I tex_size, float scale, LAB_Color color)
{
    LAB_Vec2F screen_size_f = LAB_Vec2I2F(screen_size);
    LAB_Vec2F surface_origin = LAB_Vec2F_HdDiv(LAB_Vec2I2F(pos), screen_size_f);
    LAB_Vec2F surface_size = LAB_Vec2F_HdDiv(LAB_Vec2F_Mul(scale, LAB_Vec2I2F(tex_size)), screen_size_f);

    LAB_RenderSurface(r, tex, LAB_Box2F_FromOriginAndSize(surface_origin, surface_size), tex_size, color);
}