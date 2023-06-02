#include "LAB_view_render_sky.h"

#include "LAB_view_renderer_vertex_spec.h"
#include "LAB_gl.h"
#include "LAB_obj.h"
#include "LAB_vec_algo.h"


/*
 *  3 +
 *    |\
 *    | \
 *  2 |  \
 *    |   \
 *    |    \
 *  1 +-----+
 *    |     |\
 *    |     | \
 *  0 |  0  |  \
 *    |     |   \
 *    |     |    \
 * -1 +-----+-----+
 *   -1  0  1  2  3
 */
static const LAB_SkyVertex sky_triangle[3] = {
    {.screen_pos = {-1.0, -1.0}},
    {.screen_pos = {-1.0,  3.0}},
    {.screen_pos = { 3.0, -1.0}},
};

LAB_STATIC
bool LAB_SkyRenderer_Obj(LAB_SkyRenderer* r, LAB_OBJ_Action action)
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


bool LAB_SkyRenderer_Create(LAB_SkyRenderer* r)
{
    if(!LAB_SkyRenderer_Obj(r, LAB_OBJ_CREATE)) return false;

    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize_Format(r->vao, &LAB_sky_vertex_spec);
    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize_Buffer(r->vao, r->vbo, &LAB_sky_vertex_spec);
    LAB_GL_CHECK();

    glNamedBufferStorage(r->vbo.id, sizeof sky_triangle, sky_triangle, 0);
    //glNamedBufferData(r->vbo.id, sizeof box_vertices, box_vertices, GL_STATIC_DRAW);
    LAB_GL_CHECK();

    return true;
}

void LAB_SkyRenderer_Destroy(LAB_SkyRenderer* r) { LAB_SkyRenderer_Obj(r, LAB_OBJ_DESTROY); }

void LAB_SkyRenderer_Prepare(LAB_SkyRenderer* r, LAB_ViewRenderer* renderer, LAB_RenderSkyAttrs attrs)
{
    LAB_ViewRenderer_Sky* sk = &renderer->sky_overworld;
    LAB_ViewProgram_Use(&sk->program);
    glBindVertexArray(r->vao.id);

    LAB_Mat4F inv_modelproj = LAB_Mat4F_Invert(attrs.modelproj);

    LAB_Vec3F forward = LAB_UnprojectPoint(inv_modelproj, (LAB_Vec3F) {0, 0, 1});
    LAB_Vec3F right = LAB_UnprojectPoint(inv_modelproj, (LAB_Vec3F) {1, 0, 1});
    LAB_Vec3F up = LAB_UnprojectPoint(inv_modelproj, (LAB_Vec3F) {0, 1, 1});

    right = LAB_Vec3F_Sub(right, forward);
    up = LAB_Vec3F_Sub(up, forward);

    glUniform3fv(sk->uni_forward.id, 1, LAB_Vec3F_AsCArray(&forward));
    glUniform3fv(sk->uni_right.id,   1, LAB_Vec3F_AsCArray(&right));
    glUniform3fv(sk->uni_up.id,      1, LAB_Vec3F_AsCArray(&up));

    LAB_GL_UniformColorHDR(sk->uni_fog_color, attrs.fog.fog_color);
    LAB_GL_UniformColorHDR(sk->uni_horizon_color, attrs.fog.horizon_color);
    glUniform1f(sk->uni_fog_density.id, attrs.fog.fog_density);
    glUniform1f(sk->uni_time.id, attrs.time);
}

void LAB_SkyRenderer_Render(LAB_SkyRenderer* r)
{
    glDrawArrays(GL_TRIANGLES, 0, 3);
}