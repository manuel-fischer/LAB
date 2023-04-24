#pragma once

#include "LAB_view_program.h"
#include "LAB_render_pass.h"
#include "LAB_texture_atlas.h"
#include "LAB_mat.h"
#include "LAB_color_hdr.h"

typedef struct LAB_ShadingAttrs
{
    float exposure, saturation;
} LAB_ShadingAttrs;

typedef struct LAB_FogAttrs
{
    float fog_start, fog_end;
    LAB_ColorHDR fog_color;
} LAB_FogAttrs;

typedef struct LAB_ViewRenderer_Blocks
{
    LAB_ViewProgram program;

    LAB_GL_Uniform uni_cam_pos;
    LAB_GL_Uniform uni_modelproj;
    LAB_GL_Uniform uni_texture_scale;
    LAB_GL_Uniform uni_exposure;
    LAB_GL_Uniform uni_saturation;

    LAB_GL_Uniform uni_fog_color;
    LAB_GL_Uniform uni_fog_start;
    LAB_GL_Uniform uni_fog_end;
} LAB_ViewRenderer_Blocks;

bool LAB_ViewRenderer_Blocks_Create(LAB_ViewRenderer_Blocks* r, const LAB_ProgramSpec* spec);
void LAB_ViewRenderer_Blocks_Destroy(LAB_ViewRenderer_Blocks* r);


typedef struct LAB_ViewRenderer_Lines
{
    LAB_ViewProgram program;

    LAB_GL_Uniform uni_modelproj;
    LAB_GL_Uniform uni_color;
} LAB_ViewRenderer_Lines;


typedef struct LAB_ViewRenderer_Gui
{
    LAB_ViewProgram program;

    LAB_GL_Uniform uni_origin;
    LAB_GL_Uniform uni_size;
    LAB_GL_Uniform uni_tex_size;
    LAB_GL_Uniform uni_color;
} LAB_ViewRenderer_Gui;


typedef struct LAB_ViewRenderer_FpsGraph
{
    LAB_ViewProgram program;
} LAB_ViewRenderer_FpsGraph;


typedef struct LAB_ViewRenderer
{
    LAB_ViewRenderer_Blocks blocks[LAB_RENDER_PASS_COUNT];
    LAB_GL_Array blocks_vao;

    LAB_ViewRenderer_Lines lines;

    LAB_ViewRenderer_Gui gui;
    LAB_ViewRenderer_FpsGraph fps_graph;
} LAB_ViewRenderer;


bool LAB_ViewRenderer_Create(LAB_ViewRenderer* r);
void LAB_ViewRenderer_Destroy(LAB_ViewRenderer* r);

void LAB_ViewRenderer_Blocks_Prepare(LAB_ViewRenderer* r, LAB_RenderPass pass, LAB_TexAtlas* atlas, LAB_ShadingAttrs shading, LAB_FogAttrs fog);
void LAB_ViewRenderer_Blocks_Finish(LAB_ViewRenderer* r, LAB_RenderPass pass);
void LAB_ViewRenderer_Blocks_SetCam(LAB_ViewRenderer* r, LAB_RenderPass pass, LAB_Vec3F cam_pos, LAB_Mat4F mat);
void LAB_ViewRenderer_Blocks_SetCurrentBuffer(LAB_ViewRenderer* r, LAB_GL_Buffer buffer);