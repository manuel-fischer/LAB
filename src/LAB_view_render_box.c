#include "LAB_view_render_box.h"

#include "LAB_view_renderer_vertex_types.h"
#include "LAB_view_renderer_vertex_spec.h"

#include "LAB_gl.h"
#include "LAB_obj.h"
#include "LAB_functional.h"
#include "LAB_ptr.h"


static const LAB_LineVertex box_vertices[] = {
    {{0, 0, 0}}, /*--*/ {{1, 0, 0}},
    {{0, 0, 0}}, /*--*/ {{0, 1, 0}},
    {{0, 0, 0}}, /*--*/ {{0, 0, 1}},

    {{1, 1, 0}}, /*--*/ {{0, 1, 0}},
    {{1, 1, 0}}, /*--*/ {{1, 0, 0}},
    {{1, 1, 0}}, /*--*/ {{1, 1, 1}},

    {{0, 1, 1}}, /*--*/ {{1, 1, 1}},
    {{0, 1, 1}}, /*--*/ {{0, 0, 1}},
    {{0, 1, 1}}, /*--*/ {{0, 1, 0}},

    {{1, 0, 1}}, /*--*/ {{0, 0, 1}},
    {{1, 0, 1}}, /*--*/ {{1, 1, 1}},
    {{1, 0, 1}}, /*--*/ {{1, 0, 0}},
};


LAB_STATIC
bool LAB_BoxRenderer_Obj(LAB_BoxRenderer* r, LAB_OBJ_Action action)
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


bool LAB_BoxRenderer_Create(LAB_BoxRenderer* r)
{
    if(!LAB_BoxRenderer_Obj(r, LAB_OBJ_CREATE)) return false;

    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize_Format(r->vao, &LAB_line_vertex_spec);
    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize_Buffer(r->vao, r->vbo, &LAB_line_vertex_spec);
    LAB_GL_CHECK();

    glNamedBufferStorage(r->vbo.id, sizeof box_vertices, box_vertices, 0);
    //glNamedBufferData(r->vbo.id, sizeof box_vertices, box_vertices, GL_STATIC_DRAW);
    LAB_GL_CHECK();

    return true;
}

void LAB_BoxRenderer_Destroy(LAB_BoxRenderer* r) { LAB_BoxRenderer_Obj(r, LAB_OBJ_DESTROY); }



void LAB_BoxRenderer_Prepare(LAB_BoxRenderer* r, LAB_ViewRenderer* renderer, LAB_Mat4F modelproj_mat, LAB_Vec3D pos)
{
    r->modelproj_mat = modelproj_mat;
    r->pos = LAB_Vec3D2F_Cast(pos);
    r->uni_modelproj = renderer->lines.uni_modelproj;
    LAB_ViewProgram_Use(&renderer->lines.program);
    glBindVertexArray(r->vao.id);
    glDisable(GL_LINE_SMOOTH);
    glLineWidth(2);
}




#include <stdio.h>
void LAB_RenderBox(LAB_BoxRenderer* r, LAB_Box3F box)
{
    LAB_Vec3F origin = box.a;
    LAB_Vec3F scale = LAB_Box3F_Size(box);

    LAB_Mat4F viewproj = LAB_REDUCE_3(LAB_Mat4F_Chain,
        r->modelproj_mat,
        LAB_Mat4F_Translate3V(LAB_Vec3F_Sub(origin, r->pos)),
        LAB_Mat4F_Scale3V(scale)
    );

    glUniformMatrix4fv(r->uni_modelproj.id, 1, false, LAB_Mat4F_AsCArray(&viewproj));
    glDrawArrays(GL_LINES, 0, LAB_LEN(box_vertices));
}