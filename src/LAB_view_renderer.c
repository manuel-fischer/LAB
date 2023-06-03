#include "LAB_view_renderer.h"

#include "LAB_gl.h"
#include "LAB_obj.h"

#include "LAB_view_vertex_spec.h"
#include "LAB_model.h" // -> LAB_Vertex

#include "LAB_view_renderer_vertex_spec.h"
#include "LAB_view_renderer_shaders.h"

#include "LAB_vec2.h"



bool LAB_ViewRenderer_Obj(LAB_ViewRenderer* r, LAB_OBJ_Action action)
{
    LAB_BEGIN_OBJ(action);

    LAB_OnGLInfoLog on_info = LAB_GLPrintInfoLog;

    LAB_OBJ(LAB_GL_OBJ_ALLOC(glCreateVertexArrays, &r->blocks_vao),
            LAB_GL_OBJ_FREE(glDeleteVertexArrays, &r->blocks_vao),

    LAB_OBJ(LAB_SetupShaderEnvironment(&LAB_shader_environment),
            (void)0,

    LAB_OBJ_FOR(i, LAB_RENDER_PASS_COUNT,
                LAB_ViewProgram_CreateWithSpec(&r->blocks[i].program, on_info, LAB_blocks_shaders[i], &r->blocks[i]),
                LAB_ViewProgram_Destroy(&r->blocks[i].program),

    LAB_OBJ(LAB_ViewProgram_CreateWithSpec(&r->sky_overworld.program, on_info, &LAB_sky_overworld_shader, &r->sky_overworld),
            LAB_ViewProgram_Destroy(&r->sky_overworld.program),

    LAB_OBJ(LAB_ViewProgram_CreateWithSpec(&r->lines.program, on_info, &LAB_line_shader, &r->lines),
            LAB_ViewProgram_Destroy(&r->lines.program),

    LAB_OBJ(LAB_ViewProgram_CreateWithSpec(&r->gui.program, on_info, &LAB_gui_shader, &r->gui),
            LAB_ViewProgram_Destroy(&r->gui.program),

    LAB_OBJ(LAB_ViewProgram_CreateWithSpec(&r->fps_graph.program, on_info, &LAB_fps_graph_shader, &r->fps_graph),
            LAB_ViewProgram_Destroy(&r->fps_graph.program),

        LAB_YIELD_OBJ(true);
    );););););););

    LAB_END_OBJ(false);
}

bool LAB_ViewRenderer_Create(LAB_ViewRenderer* r)
{
    return LAB_ViewRenderer_Obj(r, LAB_OBJ_CREATE);
}

void LAB_ViewRenderer_Destroy(LAB_ViewRenderer* r)
{
    LAB_ViewRenderer_Obj(r, LAB_OBJ_DESTROY);
}







void LAB_ViewRenderer_Blocks_Prepare(LAB_ViewRenderer* r, LAB_RenderPass pass, LAB_TexAtlas* atlas, LAB_RenderBlocksAttrs attrs)
{
    LAB_GL_CHECK();
    LAB_ViewRenderer_Blocks* b = &r->blocks[pass];
    LAB_ViewProgram_Use(&b->program);
    LAB_GL_CHECK();
    glBindVertexArray(r->blocks_vao.id);
    LAB_GL_CHECK();
    //glUniform2fv(r->blocks.uni_texture_scale.id, 1, (GLfloat[2]) { 1.f/(float)atlas->w, 1.f/(float)atlas->h });
    LAB_Vec2F scale_factor = LAB_TexAtlas_ScaleFactor(atlas);
    glUniform2fv(b->uni_texture_scale.id, 1, LAB_Vec2F_AsCArray(&scale_factor));

    glUniform1f(b->uni_exposure.id, attrs.shading.exposure);
    glUniform1f(b->uni_saturation.id, attrs.shading.saturation);

    glUniform1f(b->uni_fog_start.id, attrs.fog.fog_start);
    glUniform1f(b->uni_fog_end.id, attrs.fog.fog_end);
    LAB_GL_UniformColorHDR(b->uni_fog_color, attrs.fog.fog_color);
    LAB_GL_UniformColorHDR(b->uni_horizon_color, attrs.fog.horizon_color);
    glUniform1f(b->uni_fog_density.id, attrs.fog.fog_density);
    glUniform1f(b->uni_time.id, attrs.time);
    LAB_GL_CHECK();
}

void LAB_ViewRenderer_Blocks_Finish(LAB_ViewRenderer* r, LAB_RenderPass pass) {}

void LAB_ViewRenderer_Blocks_SetCam(LAB_ViewRenderer* r, LAB_RenderPass pass, LAB_Vec3F cam_pos, LAB_Mat4F mat)
{
    LAB_GL_CHECK();
    LAB_ViewRenderer_Blocks* b = &r->blocks[pass];
    glUniform3fv(b->uni_cam_pos.id, 1, LAB_Vec3F_AsCArray(&cam_pos));
    glUniformMatrix4fv(b->uni_modelproj.id, 1, false, LAB_Mat4F_AsCArray(&mat));
    LAB_GL_CHECK();
}

void LAB_ViewRenderer_Blocks_SetCurrentBuffer(LAB_ViewRenderer* r, LAB_GL_Buffer buffer)
{
    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize(r->blocks_vao, buffer, &LAB_blocks_vertex_spec);
    LAB_GL_CHECK();
}
