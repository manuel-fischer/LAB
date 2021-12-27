#include "LAB_view_mesh.h"

void LAB_View_Mesh_Destroy(LAB_View_Mesh* mesh)
{
    if(mesh->data)
        LAB_Free(mesh->data);

    if(mesh->vbo)
        LAB_GL_FREE(glDeleteBuffers, 1, &mesh->vbo);
}
