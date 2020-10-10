#pragma once

typedef struct LAB_View_Mesh
{
    size_t size, capacity;
    LAB_Triangle* data;

    unsigned vbo;
} LAB_View_Mesh;


static inline void LAB_View_Mesh_Destroy(LAB_View_Mesh* mesh)
{
    if(mesh->data)
        LAB_Free(mesh->data);

    if(mesh->vbo)
        glDeleteBuffers(1, &mesh->vbo); LAB_GL_DEBUG_FREE(1);
}
