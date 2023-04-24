#include "LAB_view_program.h"

#include "LAB_gl.h"
#include "LAB_obj.h"
#include "LAB_util.h"
#include "LAB_memory.h"
#include "LAB_string.h"

bool LAB_ViewProgram_Obj(LAB_ViewProgram* p, LAB_OBJ_Action action)
{
    LAB_BEGIN_OBJ(action);

    LAB_GL_DBG_CHECK();

    LAB_OBJ_GL((p->program.id = glCreateProgram()) != 0,
               glDeleteProgram(p->program.id),

    LAB_OBJ_FOR(i, LAB_VP_SHADER_COUNT,
        (p->shader[i].id = 0, true),
        LAB_GL_OBJ_FREE1(glDeleteShader, p->shader[i].id),

        LAB_YIELD_OBJ(true);
    ););

    LAB_GL_DBG_CHECK();

    LAB_END_OBJ(false);
}

bool LAB_ViewProgram_Create(LAB_ViewProgram* program)
{
    return LAB_ViewProgram_Obj(program, LAB_OBJ_CREATE);
}

void LAB_ViewProgram_Destroy(LAB_ViewProgram* program)
{
    LAB_ViewProgram_Obj(program, LAB_OBJ_DESTROY);
}


LAB_ViewProgram_ShaderIndex LAB_ViewProgram_MapShader2Index(LAB_GL_Enum shader_type)
{
    switch(shader_type)
    {
        case GL_VERTEX_SHADER: return LAB_VP_VERTEX_SHADER;
        case GL_FRAGMENT_SHADER: return LAB_VP_FRAGMENT_SHADER;
        default: LAB_ASSERT_FALSE("Unsupported shader type");
    }
}

LAB_GL_Shader* LAB_ViewProgram_GetShaderRef(LAB_ViewProgram* program, LAB_GL_Enum shader_type)
{
    return &program->shader[LAB_ViewProgram_MapShader2Index(shader_type)];
}




void LAB_GLPrintInfoLog(const char* filename, const char* info)
{
    if(filename)
        fprintf(stderr, "GLSL shader error: %s:\n%s\n", filename, info);
    else
        fprintf(stderr, "GLSL linking error: \n%s\n", info);
}



LAB_STATIC
void LAB_HandleProgramError(GLuint object, GLenum object_type, const char* filename, LAB_OnGLInfoLog on_info)
{
    if(!on_info) return;

    GLint info_log_length;
    if(object_type == GL_SHADER)
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &info_log_length);
    else if(object_type == GL_PROGRAM)
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &info_log_length);
    else
        LAB_ASSERT_FALSE("Invalid object type");

    if(info_log_length)
    {
        char* buffer = LAB_Malloc(info_log_length);
        if(buffer)
        {
            if(object_type == GL_SHADER)
                glGetShaderInfoLog(object, info_log_length, NULL, buffer);
            else if(object_type == GL_PROGRAM)
                glGetProgramInfoLog(object, info_log_length, NULL, buffer);
            else
                LAB_ASSERT_FALSE("Invalid object type");

            on_info(filename, buffer);
            LAB_Free(buffer);
        }
    }
    //else on_info(filename, "no error");
}



LAB_STATIC
bool LAB_ViewProgram_LinkLocations(LAB_ViewProgram* program, const LAB_ProgramSpec* spec, void* link_table)
{
    LAB_GL_DBG_CHECK();

    LAB_FOREACH_INDEX(const LAB_View_VertexElementSpec, i, e, spec->vertex_spec->elements)
    {
        glBindAttribLocation(program->program.id, i, e->name);
        LAB_GL_DBG_CHECK();
    }
    LAB_GL_DBG_CHECK();

    LAB_FOREACH(const LAB_UniformEntry, e, spec->uniforms)
    {
        LAB_GL_Uniform uniform;
        uniform.id = glGetUniformLocation(program->program.id, e->name);
        if(!LAB_GL_ID_OK(uniform))
        {
            LAB_SetCurrentGLError();
            LAB_ADD_ERROR_MESSAGE_FMT("Uniform '%s' not found", e->name);
            return false;
        }
        LAB_ASSERT(link_table != NULL);
        *(LAB_GL_Uniform*)LAB_PTR_OFFSET(link_table, e->uniform_offset, 1) = uniform;
    }
    LAB_GL_DBG_CHECK();

    return true;
}

LAB_STATIC
void LAB_ViewProgram_DumpShaderSource(LAB_GL_Shader shader)
{
    char buf[1<<12];

    GLint length = 0;
    glGetShaderSource(shader.id, sizeof(buf)-1, &length, buf);
    buf[length] = '\0';
    printf("<SHADER DUMP>%s</SHADER DUMP>\n", buf);
}


LAB_STATIC
bool LAB_ViewProgram_CreateShader(LAB_ViewProgram* program, LAB_OnGLInfoLog on_info, const LAB_ShaderSpec* spec, LAB_StringView preamble)
{
    if(!GL_ARB_shading_language_include) return false;

    LAB_GL_Shader* shader = LAB_ViewProgram_GetShaderRef(program, spec->shader_type);

    LAB_GL_OBJ_ALLOC1(shader->id = glCreateShader(spec->shader_type));
    LAB_GL_CHECK();
    if(!LAB_GL_ID_OK(*shader))
        return (LAB_SetCurrentGLError(), false);

    LAB_FileContents source = LAB_ReadFile(spec->filename, "r");
    if(!source.success)
    {
        LAB_GL_OBJ_FREE1(glDeleteShader, shader->id);
        return false;
    }


    //printf("<PREAMBLE>\n%s</PREAMBLE>\n", preamble.data);
    //printf("<SHADER>\n%s</SHADER>\n", source.data);


    glShaderSource(shader->id, 2,
        (const char*const[2]){ preamble.data, source.data },
        (const GLint[2]){ preamble.size, source.size });
    glCompileShader(shader->id);

    //LAB_ViewProgram_DumpShaderSource(*shader);

    /*LAB_ASSERT(strncmp(spec->filename, "shaders/", strlen("shaders/")) == 0);

    const char* include_name = spec->filename + strlen("shaders");
    LAB_GL_CHECK();
    glNamedStringARB(GL_SHADER_INCLUDE_ARB, strlen(include_name), include_name, source.size, source.data);
    LAB_GL_CHECK();
    glCompileShaderIncludeARB(shader->id, 1, (const char*const[]) { include_name }, (const int[]) { strlen(include_name) });
    LAB_GL_CHECK();
    glCompileShader(shader->id);
    LAB_GL_CHECK();*/

    LAB_Free(source.data);

    LAB_HandleProgramError(shader->id, GL_SHADER, spec->filename, on_info);

    GLint status;
    glGetShaderiv(shader->id, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        LAB_GL_OBJ_FREE1(glDeleteShader, shader->id);
        return (LAB_SetCurrentGLError(), LAB_ADD_ERROR_MESSAGE_FMT("Shader '%s' couldn't be compiled successfully", spec->filename), false);
    }

    LAB_GL_DBG_CHECK();

    return shader;
}


LAB_STATIC
bool LAB_ViewProgram_InitSpec(LAB_ViewProgram* program, LAB_OnGLInfoLog on_info, const LAB_ProgramSpec* program_spec, void* link_table)
{
    LAB_ASSERT(program_spec->shaders_count != 0);

    LAB_StringBuilder preamble;
    LAB_StringBuilder_Create(&preamble);
    LAB_StringBuilder_CatS(&preamble,
        "#version 400 core\n"
        "#extension GL_ARB_shading_language_include : require\n"
    );
    LAB_FOREACH(const LAB_DefineEntry, defn, program_spec->defines)
        LAB_StringBuilder_PrintF(&preamble, "#define %s %s\n", defn->name, defn->content);

    if(!preamble.success) return false;
    //printf("%s\n", LAB_StringBuilder_AsCStr(&preamble));

    LAB_FOREACH(const LAB_ShaderSpec, spec, program_spec->shaders)
    {
        if(!LAB_ViewProgram_CreateShader(program, on_info, spec, LAB_StringBuilder_AsStringView(&preamble)))
        {
            LAB_StringBuilder_Destroy(&preamble);
            return false;
        }
    }

    LAB_StringBuilder_Destroy(&preamble);

    LAB_FOREACH(const LAB_ShaderSpec, spec, program_spec->shaders)
    {
        LAB_GL_Shader* shader = LAB_ViewProgram_GetShaderRef(program, spec->shader_type);
        glAttachShader(program->program.id, shader->id);
        LAB_GL_DBG_CHECK();
    }
    glLinkProgram(program->program.id);
    LAB_GL_DBG_CHECK();
    LAB_HandleProgramError(program->program.id, GL_PROGRAM, NULL, on_info);

    GLint status;
    glGetProgramiv(program->program.id, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) return (LAB_SetCurrentGLError(), LAB_ADD_ERROR_MESSAGE("Link status"), false);

    LAB_GL_DBG_CHECK();

    if(!LAB_ViewProgram_LinkLocations(program, program_spec, link_table))
    {
        LAB_FOREACH(const LAB_ShaderSpec, spec, program_spec->shaders)
            LAB_ADD_ERROR_MESSAGE_FMT("With shader %s", spec->filename);
        return false;
    }

    LAB_GL_DBG_CHECK();

    return true;
}

bool LAB_ViewProgram_CreateWithSpec(LAB_ViewProgram* program, LAB_OnGLInfoLog on_info, const LAB_ProgramSpec* program_spec, void* link_table)
{
    LAB_OBJ(LAB_ViewProgram_Create(program),
            LAB_ViewProgram_Destroy(program),

    LAB_OBJ(LAB_ViewProgram_InitSpec(program, on_info, program_spec, link_table),
            (void)0,

        return true;
    ););

    return false;
}


void LAB_ViewProgram_Use(LAB_ViewProgram* program)
{
    glUseProgram(program->program.id);
}




bool LAB_SetupShaderEnvironment(const LAB_ShaderEnvironmentSpec* spec)
{
    if(!GL_ARB_shading_language_include) return false;

    LAB_CFOREACH(LAB_ShaderIncludeSpec, inc, spec->includes)
    {
        LAB_FileContents contents = LAB_ReadFile(inc->filename, "r");
        if(!contents.success) return false;

        glNamedStringARB(GL_SHADER_INCLUDE_ARB,
            strlen(inc->include_name), inc->include_name,
            contents.size, contents.data);
    }
    return true;
}
