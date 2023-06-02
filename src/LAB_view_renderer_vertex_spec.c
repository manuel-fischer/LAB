#include "LAB_view_renderer_vertex_spec.h"

#include "LAB_view_renderer.h"
#include "LAB_model.h" // -> vertex
#include "LAB_gl.h"

const LAB_View_VertexSpec LAB_blocks_vertex_spec = {
    .vertex_size = sizeof(LAB_Vertex),
    .elements_count = 4,
    .elements = (const LAB_View_VertexElementSpec[4]) {
        {
            .name = "pos",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_Vertex, pos),
            .dimension = 3,
            .offset = offsetof(LAB_Vertex, pos),
            .normalized = false,
            .keep_as_integer = false,
        },
        {
            .name = "tex",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_Vertex, tex),
            .dimension = 2,
            .offset = offsetof(LAB_Vertex, tex),
            .normalized = false,
            .keep_as_integer = false,
        },
        {
            .name = "color",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_Vertex, color),
            .dimension = 4,
            .offset = offsetof(LAB_Vertex, color),
            .normalized = true,
            .keep_as_integer = false,
        },
        {
            .name = "light",
            .type = GL_UNSIGNED_INT,
            .dimension = 1,
            .offset = offsetof(LAB_Vertex, light),
            .normalized = false,
            .keep_as_integer = true,
        },
    },
};

const LAB_View_VertexSpec LAB_sky_vertex_spec = {
    .vertex_size = sizeof(LAB_SkyVertex),
    .elements_count = 1,
    .elements = (const LAB_View_VertexElementSpec[1]) {
        {
            .name = "screen_pos",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_SkyVertex, screen_pos),
            .dimension = 2,
            .offset = offsetof(LAB_SkyVertex, screen_pos),
            .normalized = false,
            .keep_as_integer = false,
        },
    },
};

const LAB_View_VertexSpec LAB_line_vertex_spec = {
    .vertex_size = sizeof(LAB_LineVertex),
    .elements_count = 1,
    .elements = (const LAB_View_VertexElementSpec[1]) {
        {
            .name = "pos",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_LineVertex, pos),
            .dimension = 3,
            .offset = offsetof(LAB_LineVertex, pos),
            .normalized = false,
            .keep_as_integer = false,
        },
    },
};

const LAB_View_VertexSpec LAB_gui_vertex_spec = {
    .vertex_size = sizeof(LAB_GuiVertex),
    .elements_count = 1,
    .elements = (const LAB_View_VertexElementSpec[1]) {
        {
            .name = "pos_tex",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_GuiVertex, pos_tex),
            .dimension = 2,
            .offset = offsetof(LAB_GuiVertex, pos_tex),
            .normalized = false,
            .keep_as_integer = false,
        },
    },
};


const LAB_View_VertexSpec LAB_fps_graph_vertex_spec = {
    .vertex_size = sizeof(LAB_FpsGraphVertex),
    .elements_count = 2,
    .elements = (const LAB_View_VertexElementSpec[2]) {
        {
            .name = "pos",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_FpsGraphVertex, pos),
            .dimension = 2,
            .offset = offsetof(LAB_FpsGraphVertex, pos),
            .normalized = false,
            .keep_as_integer = false,
        },
        {
            .name = "color",
            .type = LAB_GL_MEMBER_ELEMENT_TYPE(LAB_FpsGraphVertex, color),
            .dimension = 4,
            .offset = offsetof(LAB_FpsGraphVertex, color),
            .normalized = true,
            .keep_as_integer = false,
        },
    },
};