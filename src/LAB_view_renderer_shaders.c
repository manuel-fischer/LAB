#include "LAB_view_renderer_shaders.h"

#include "LAB_view_renderer_vertex_spec.h"
#include "LAB_view_renderer.h"
#include "LAB_gl.h"



const LAB_ShaderEnvironmentSpec LAB_shader_environment =
{
    .includes_count = 4,
    .includes = (const LAB_ShaderIncludeSpec[4]) {
        { .include_name = "/color_hdr.glsl", .filename = "shaders/color_hdr.glsl" },
        { .include_name = "/tone_mapping.glsl", .filename = "shaders/tone_mapping.glsl" },
        { .include_name = "/fog.glsl", .filename = "shaders/fog.glsl" },
        { .include_name = "/render_pass.glsl", .filename = "shaders/render_pass.glsl" },
    }
};



#define LAB_blocks_uniforms_count 8
static const LAB_UniformEntry LAB_blocks_uniforms[LAB_blocks_uniforms_count] =
{
    { "camPos",       offsetof(LAB_ViewRenderer_Blocks, uni_cam_pos) },
    { "modelproj",    offsetof(LAB_ViewRenderer_Blocks, uni_modelproj) },
    { "textureScale", offsetof(LAB_ViewRenderer_Blocks, uni_texture_scale) },
    { "exposure",     offsetof(LAB_ViewRenderer_Blocks, uni_exposure) },
    { "saturation",   offsetof(LAB_ViewRenderer_Blocks, uni_saturation) },
    { "fogColor",     offsetof(LAB_ViewRenderer_Blocks, uni_fog_color) },
    { "fogStart",     offsetof(LAB_ViewRenderer_Blocks, uni_fog_start) },
    { "fogEnd",       offsetof(LAB_ViewRenderer_Blocks, uni_fog_end) },
};


const LAB_ProgramSpec* const LAB_blocks_shaders[LAB_RENDER_PASS_COUNT] =
{
    [LAB_RENDER_PASS_SOLID]  = &LAB_blocks_shader_solid,
    [LAB_RENDER_PASS_MASKED] = &LAB_blocks_shader_masked,
    [LAB_RENDER_PASS_BLIT]   = &LAB_blocks_shader_blit,
    [LAB_RENDER_PASS_ALPHA]  = &LAB_blocks_shader_alpha,
};

const LAB_ProgramSpec LAB_blocks_shader_solid = {
    .shaders_count = 2,
    .shaders = (const LAB_ShaderSpec[2]) {
        {
            .filename = "shaders/blocks.glslv",
            .shader_type = GL_VERTEX_SHADER,
        },
        {
            .filename = "shaders/blocks.glslf",
            .shader_type = GL_FRAGMENT_SHADER,
        },
    },
    .defines_count = 1,
    .defines = (const LAB_DefineEntry[1]) {
        { .name = "COMPILED_RENDER_PASS", .content = "LAB_RENDER_PASS_SOLID" },
    },

    .vertex_spec = &LAB_blocks_vertex_spec,

    .uniforms_count = LAB_blocks_uniforms_count,
    .uniforms = LAB_blocks_uniforms,
};


const LAB_ProgramSpec LAB_blocks_shader_masked = {
    .shaders_count = 2,
    .shaders = (const LAB_ShaderSpec[2]) {
        {
            .filename = "shaders/blocks.glslv",
            .shader_type = GL_VERTEX_SHADER,
        },
        {
            .filename = "shaders/blocks.glslf",
            .shader_type = GL_FRAGMENT_SHADER,
        },
    },
    .defines_count = 1,
    .defines = (const LAB_DefineEntry[1]) {
        { .name = "COMPILED_RENDER_PASS", .content = "LAB_RENDER_PASS_MASKED" },
    },

    .vertex_spec = &LAB_blocks_vertex_spec,

    .uniforms_count = LAB_blocks_uniforms_count,
    .uniforms = LAB_blocks_uniforms,
};

const LAB_ProgramSpec LAB_blocks_shader_blit = {
    .shaders_count = 2,
    .shaders = (const LAB_ShaderSpec[2]) {
        {
            .filename = "shaders/blocks.glslv",
            .shader_type = GL_VERTEX_SHADER,
        },
        {
            .filename = "shaders/blocks.glslf",
            .shader_type = GL_FRAGMENT_SHADER,
        },
    },
    .defines_count = 1,
    .defines = (const LAB_DefineEntry[1]) {
        { .name = "COMPILED_RENDER_PASS", .content = "LAB_RENDER_PASS_BLIT" },
    },

    .vertex_spec = &LAB_blocks_vertex_spec,

    .uniforms_count = LAB_blocks_uniforms_count,
    .uniforms = LAB_blocks_uniforms,
};

const LAB_ProgramSpec LAB_blocks_shader_alpha = {
    .shaders_count = 2,
    .shaders = (const LAB_ShaderSpec[2]) {
        {
            .filename = "shaders/blocks.glslv",
            .shader_type = GL_VERTEX_SHADER,
        },
        {
            .filename = "shaders/blocks.glslf",
            .shader_type = GL_FRAGMENT_SHADER,
        },
    },
    .defines_count = 1,
    .defines = (const LAB_DefineEntry[1]) {
        { .name = "COMPILED_RENDER_PASS", .content = "LAB_RENDER_PASS_ALPHA" },
    },

    .vertex_spec = &LAB_blocks_vertex_spec,

    .uniforms_count = LAB_blocks_uniforms_count,
    .uniforms = LAB_blocks_uniforms,
};







const LAB_ProgramSpec LAB_line_shader = {
    .shaders_count = 2,
    .shaders = (const LAB_ShaderSpec[2]) {
        {
            .filename = "shaders/line.glslv",
            .shader_type = GL_VERTEX_SHADER,
        },
        {
            .filename = "shaders/line.glslf",
            .shader_type = GL_FRAGMENT_SHADER,
        },
    },
    .defines_count = 0,
    .defines = NULL,

    .vertex_spec = &LAB_line_vertex_spec,

    .uniforms_count = 2,
    .uniforms = (const LAB_UniformEntry[2]) {
        { "modelproj", offsetof(LAB_ViewRenderer_Lines, uni_modelproj) },
        { "color",     offsetof(LAB_ViewRenderer_Lines, uni_color) },
    },
};


const LAB_ProgramSpec LAB_gui_shader = {
    .shaders_count = 2,
    .shaders = (const LAB_ShaderSpec[2]) {
        {
            .filename = "shaders/gui.glslv",
            .shader_type = GL_VERTEX_SHADER,
        },
        {
            .filename = "shaders/gui.glslf",
            .shader_type = GL_FRAGMENT_SHADER,
        },
    },
    .defines_count = 0,
    .defines = NULL,

    .vertex_spec = &LAB_gui_vertex_spec,

    .uniforms_count = 4,
    .uniforms = (const LAB_UniformEntry[4]) {
        { "origin", offsetof(LAB_ViewRenderer_Gui, uni_origin) },
        { "size", offsetof(LAB_ViewRenderer_Gui, uni_size) },
        { "tex_size", offsetof(LAB_ViewRenderer_Gui, uni_tex_size) },
        { "color", offsetof(LAB_ViewRenderer_Gui, uni_color) },
    },
};


const LAB_ProgramSpec LAB_fps_graph_shader = {
    .shaders_count = 2,
    .shaders = (const LAB_ShaderSpec[2]) {
        {
            .filename = "shaders/fps_graph.glslv",
            .shader_type = GL_VERTEX_SHADER,
        },
        {
            .filename = "shaders/fps_graph.glslf",
            .shader_type = GL_FRAGMENT_SHADER,
        },
    },
    .defines_count = 0,
    .defines = NULL,

    .vertex_spec = &LAB_fps_graph_vertex_spec,

    .uniforms_count = 0,
    .uniforms = NULL,
};

