#pragma once

#include "LAB_color.h"
#include "LAB_stdinc.h"
#define LAB_FPS_GRAPH_MAX_SAMPLES 256

typedef struct LAB_FpsGraphSample
{
    float x, y;
    LAB_Color color;
    uint32_t pad;
} LAB_FpsGraphSample;

typedef struct LAB_FpsGraph
{
    unsigned start_pos;
    LAB_FpsGraphSample samples[LAB_FPS_GRAPH_MAX_SAMPLES];
} LAB_FpsGraph;

/**
 *  Return 1 on success
 */
bool LAB_FpsGraph_Create(LAB_FpsGraph* graph, LAB_Color color);
void LAB_FpsGraph_Destroy(LAB_FpsGraph* graph);

void LAB_FpsGraph_Shift(LAB_FpsGraph* graph);
void LAB_FpsGraph_SetSample(LAB_FpsGraph* graph, float frame_ms);
void LAB_FpsGraph_SetSampleOffset(LAB_FpsGraph* graph, float frame_ms);
void LAB_FpsGraph_AddSample(LAB_FpsGraph* graph, float frame_ms);

void LAB_FpsGraph_Render_Prepare();
void LAB_FpsGraph_Render(LAB_FpsGraph* graph);
void LAB_FpsGraph_Render_Base();
