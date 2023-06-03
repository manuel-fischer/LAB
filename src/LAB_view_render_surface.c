#include "LAB_view_render_surface.h"

#include "LAB_view_renderer_vertex_types.h"
#include "LAB_view_renderer_vertex_spec.h"

#include "LAB_gl.h"
#include "LAB_obj.h"
#include "LAB_bits.h"


static const LAB_GuiVertex surface_vertices[] = {
    {.pos_tex = {0, 0}},
    {.pos_tex = {1, 1}},
    {.pos_tex = {1, 0}},

    {.pos_tex = {0, 0}},
    {.pos_tex = {0, 1}},
    {.pos_tex = {1, 1}},
};


LAB_STATIC
bool LAB_SurfaceRenderer_Obj(LAB_SurfaceRenderer* r, LAB_OBJ_Action action)
{
    LAB_BEGIN_OBJ(action);

    LAB_OBJ(LAB_GL_OBJ_ALLOC(glCreateVertexArrays, &r->vao),
            LAB_GL_OBJ_FREE(glDeleteVertexArrays, &r->vao),

    LAB_OBJ(LAB_GL_OBJ_ALLOC(glCreateBuffers, &r->vbo),
            LAB_GL_OBJ_FREE(glDeleteBuffers, &r->vbo),

        LAB_YIELD_OBJ(true);
    ););

    LAB_END_OBJ(false);
}


bool LAB_SurfaceRenderer_Create(LAB_SurfaceRenderer* r)
{
    if(!LAB_SurfaceRenderer_Obj(r, LAB_OBJ_CREATE)) return false;

    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize_Format(r->vao, &LAB_gui_vertex_spec);
    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize_Buffer(r->vao, r->vbo, &LAB_gui_vertex_spec);
    LAB_GL_CHECK();

    glNamedBufferStorage(r->vbo.id, sizeof surface_vertices, surface_vertices, 0);
    //glNamedBufferData(r->vbo.id, sizeof box_vertices, box_vertices, GL_STATIC_DRAW);
    LAB_GL_CHECK();

    return true;
}

void LAB_SurfaceRenderer_Destroy(LAB_SurfaceRenderer* r) { LAB_SurfaceRenderer_Obj(r, LAB_OBJ_DESTROY); }

void LAB_SurfaceRenderer_Prepare(LAB_SurfaceRenderer* r, LAB_ViewRenderer* renderer)
{
    r->uni_origin = renderer->gui.uni_origin;
    r->uni_size = renderer->gui.uni_size;
    r->uni_tex_size = renderer->gui.uni_tex_size;
    r->uni_color = renderer->gui.uni_color;

    LAB_ViewProgram_Use(&renderer->gui.program);
    glBindVertexArray(r->vao.id);
    glDisable(GL_DEPTH_TEST);
}


void LAB_RenderSurface(LAB_SurfaceRenderer* r, LAB_GL_Texture tex, LAB_Box2F surface_rect, LAB_Vec2I tex_size, LAB_Color color)
{
    LAB_Vec2I tex_size2 = { LAB_CeilPow2(tex_size.x), LAB_CeilPow2(tex_size.y) };

    LAB_Vec2F texture_size = LAB_Vec2F_HdDiv(LAB_Vec2I2F(tex_size), LAB_Vec2I2F(tex_size2));


    LAB_Vec2F size = LAB_Box2F_Size(surface_rect);

    LAB_Vec2F shifted_origin = LAB_Vec2F_Sub(LAB_Vec2F_Mul(2, surface_rect.a), (LAB_Vec2F) {1, 1});
    LAB_Vec2F shifted_size = LAB_Vec2F_Mul(2, size);

    glBindTexture(GL_TEXTURE_2D, tex.id);
    glUniform2fv(r->uni_origin.id, 1, LAB_Vec2F_AsArray(&shifted_origin));
    glUniform2fv(r->uni_size.id, 1, LAB_Vec2F_AsArray(&shifted_size));
    glUniform2fv(r->uni_tex_size.id, 1, LAB_Vec2F_AsArray(&texture_size));
    LAB_GL_UniformColor(r->uni_color, color);

    LAB_GL_CHECK();
    glDrawArrays(GL_TRIANGLES, 0, 3*2);
    LAB_GL_CHECK();
}