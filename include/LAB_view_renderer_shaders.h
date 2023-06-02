#pragma once

#include "LAB_view_program.h"
#include "LAB_render_pass.h"

extern const LAB_ShaderEnvironmentSpec LAB_shader_environment;

extern const LAB_ProgramSpec* const LAB_blocks_shaders[LAB_RENDER_PASS_COUNT];

extern const LAB_ProgramSpec LAB_blocks_shader_solid;
extern const LAB_ProgramSpec LAB_blocks_shader_masked;
extern const LAB_ProgramSpec LAB_blocks_shader_blit;
extern const LAB_ProgramSpec LAB_blocks_shader_alpha;

extern const LAB_ProgramSpec LAB_sky_overworld_shader;

extern const LAB_ProgramSpec LAB_line_shader;
extern const LAB_ProgramSpec LAB_gui_shader;
extern const LAB_ProgramSpec LAB_fps_graph_shader;