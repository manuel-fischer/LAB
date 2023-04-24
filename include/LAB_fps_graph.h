#pragma once

#include "LAB_color.h"
#include "LAB_stdinc.h"
#include "LAB_gl_types.h"
#define LAB_FPS_GRAPH_MAX_SAMPLES 256

#include "LAB_view_renderer_vertex_types.h"
#include "LAB_view_renderer.h"

typedef struct LAB_FpsGraph
{
    unsigned start_pos;
    LAB_GL_Array vao;
    LAB_GL_Buffer vbo;
    LAB_FpsGraphVertex samples[LAB_FPS_GRAPH_MAX_SAMPLES];
} LAB_FpsGraph;

/**
 *  Return 1 on success
 */
bool LAB_FpsGraph_Create(LAB_FpsGraph* graph, LAB_Color color);
void LAB_FpsGraph_Destroy(LAB_FpsGraph* graph);

void LAB_FpsGraph_Fill(LAB_FpsGraph* graph, float frame_ms);

void LAB_FpsGraph_Shift(LAB_FpsGraph* graph);
void LAB_FpsGraph_SetSample(LAB_FpsGraph* graph, float frame_ms);
void LAB_FpsGraph_SetSampleOffset(LAB_FpsGraph* graph, float frame_ms);
void LAB_FpsGraph_AddSample(LAB_FpsGraph* graph, float frame_ms);

void LAB_FpsGraph_Render_Prepare(LAB_ViewRenderer* r);
void LAB_FpsGraph_Render(LAB_FpsGraph* graph);
