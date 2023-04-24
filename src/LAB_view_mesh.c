#include "LAB_view_mesh.h"

#include "LAB_gl.h"

void LAB_View_Mesh_Destroy(LAB_View_Mesh* mesh)
{
    if(mesh->data)
        LAB_Free(mesh->data);

    if(mesh->vbo)
        LAB_GL_FREE(glDeleteBuffers, 1, &mesh->vbo);
}


LAB_Triangle* LAB_View_Mesh_Alloc(LAB_View_Mesh* mesh, size_t add_size)
{
    size_t mesh_count, new_mesh_count, mesh_capacity;

    mesh_count = mesh->m_size;
    new_mesh_count = mesh_count+add_size;
    mesh_capacity = mesh->capacity;

    if(new_mesh_count > mesh_capacity)
    {
        if(mesh_capacity == 0) mesh_capacity = 1<<3;
        while(new_mesh_count > mesh_capacity) mesh_capacity <<= 1;
        //if(mesh_capacity > (1<<9)) return NULL; else mesh_capacity = 1<<9;// TEST ---
        LAB_Triangle* mesh_data = LAB_ReallocN(mesh->data, mesh_capacity, sizeof *mesh_data);
        if(!mesh_data) {
            return NULL;
        }
        mesh->data = mesh_data;
        mesh->capacity = mesh_capacity;
    }
    mesh->m_size=new_mesh_count;

    return &mesh->data[mesh_count];
}