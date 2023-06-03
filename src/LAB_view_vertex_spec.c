#include "LAB_view_vertex_spec.h"

#include "LAB_gl.h"
#include "LAB_loop.h"

void LAB_View_VertexSpec_Realize(LAB_GL_Array vao, LAB_GL_Buffer vbo, const LAB_View_VertexSpec* spec)
{
    glBindVertexArray(vao.id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
    LAB_FOREACH_INDEX(const LAB_View_VertexElementSpec, i, e, spec->elements)
    {
        if(e->keep_as_integer)
            glVertexAttribIPointer(i, e->dimension, e->type, spec->vertex_size, (void*)e->offset);
        else
            glVertexAttribPointer(i, e->dimension, e->type, e->normalized, spec->vertex_size, (void*)e->offset);
        //glVertexArrayAttribFormat(vao.id, e->attr->id, e->dimension, e->type, e->normalized, e->offset);
        LAB_GL_CHECK();
        glEnableVertexAttribArray(i);
        LAB_GL_CHECK();
    }
    /*LAB_View_VertexSpec_Realize_Format(vao, spec);
    LAB_View_VertexSpec_Realize_Buffer(vao, vbo, spec);*/
}

void LAB_View_VertexSpec_Realize_Format(LAB_GL_Array vao, const LAB_View_VertexSpec* spec)
{
    //glBindVertexArray(vao.id);
    LAB_FOREACH_INDEX(const LAB_View_VertexElementSpec, i, e, spec->elements)
    {
        //glVertexAttribFormat(i, e->dimension, e->type, e->normalized, e->offset);
        if(e->keep_as_integer)
            glVertexArrayAttribIFormat(vao.id, i, e->dimension, e->type, e->offset);
        else
            glVertexArrayAttribFormat(vao.id, i, e->dimension, e->type, e->normalized, e->offset);
        LAB_GL_CHECK();
        //glEnableVertexAttribArray(i);
        glEnableVertexArrayAttrib(vao.id, i);
        LAB_GL_CHECK();
    }
}
void LAB_View_VertexSpec_Realize_Buffer(LAB_GL_Array vao, LAB_GL_Buffer vbo, const LAB_View_VertexSpec* spec)
{
    glVertexArrayVertexBuffer(vao.id, 0, vbo.id, 0, spec->vertex_size);
}
