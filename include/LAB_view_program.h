#pragma once

#include "LAB_stdinc.h"
#include "LAB_gl_types.h"
#include "LAB_error_state.h"

#include "LAB_view_vertex_spec.h"

typedef enum LAB_ViewProgram_ShaderIndex
{
    LAB_VP_VERTEX_SHADER,
    LAB_VP_FRAGMENT_SHADER,
    LAB_VP_SHADER_COUNT
} LAB_ViewProgram_ShaderIndex;

typedef struct LAB_ViewProgram
{
    LAB_GL_Program program;
    LAB_GL_Shader shader[LAB_VP_SHADER_COUNT];
} LAB_ViewProgram;

bool LAB_ViewProgram_Create(LAB_ViewProgram* program);
void LAB_ViewProgram_Destroy(LAB_ViewProgram* program);

LAB_GL_Shader* LAB_ViewProgram_GetShaderRef(LAB_ViewProgram* program, LAB_GL_Enum shader_type);
LAB_ViewProgram_ShaderIndex LAB_ViewProgram_MapShader2Index(LAB_GL_Enum shader_type);

typedef struct LAB_AttrEntry
{
    const char* name;
    LAB_GL_Attr attr;
} LAB_AttrEntry;

typedef struct LAB_UniformEntry
{
    const char* name;
    size_t uniform_offset;
} LAB_UniformEntry;

typedef struct LAB_DefineEntry
{
    const char* name;
    const char* content;
} LAB_DefineEntry;

typedef struct LAB_ShaderSpec
{
    LAB_GL_Enum shader_type;
    const char* filename;
} LAB_ShaderSpec;

typedef struct LAB_ProgramSpec
{
    size_t shaders_count;
    const LAB_ShaderSpec* shaders;

    size_t defines_count;
    const LAB_DefineEntry* defines;

    const LAB_View_VertexSpec* vertex_spec;

    size_t uniforms_count;
    const LAB_UniformEntry* uniforms;
} LAB_ProgramSpec;



typedef struct LAB_ShaderIncludeSpec
{
    const char* include_name;
    const char* filename;
} LAB_ShaderIncludeSpec;

typedef struct LAB_ShaderEnvironmentSpec
{
    size_t includes_count;
    const LAB_ShaderIncludeSpec* includes;
} LAB_ShaderEnvironmentSpec;



typedef void (*LAB_OnGLInfoLog)(const char* filename, const char* info);
void LAB_GLPrintInfoLog(const char* filename, const char* info);

bool LAB_ViewProgram_CreateWithSpec(LAB_ViewProgram* program, LAB_OnGLInfoLog on_info, const LAB_ProgramSpec* program_spec, void* link_table);

void LAB_ViewProgram_Use(LAB_ViewProgram* program);


LAB_Err LAB_SetupShaderEnvironment(const LAB_ShaderEnvironmentSpec* spec);
