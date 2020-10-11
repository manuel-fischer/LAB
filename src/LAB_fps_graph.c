#include "LAB_fps_graph.h"
#include "LAB_gl.h"

#define LAB_FPS_GRAPH_CALC_Y(frame_ms) ((float)(frame_ms)*0.005f)

LAB_STATIC void LAB_FpsGraph_Shift(LAB_FpsGraph* graph)
{
    // shift all the samples to the left, keeping the alpha channel of the
    // color and the x position at the original position
    if(graph->start_pos != 0) graph->start_pos--;
    for(int i = graph->start_pos; i < LAB_FPS_GRAPH_MAX_SAMPLES-1; ++i)
    {
        //graph->samples[i].x kept
        graph->samples[i].y = graph->samples[i+1].y;
        graph->samples[i].color &= LAB_ALP_MASK;
        graph->samples[i].color |= graph->samples[i+1].color & LAB_COL_MASK;
    }
}

bool LAB_FpsGraph_Create(LAB_FpsGraph* graph)
{
    graph->start_pos = LAB_FPS_GRAPH_MAX_SAMPLES;
    // Fill in constant values
    for(int i = 0; i < LAB_FPS_GRAPH_MAX_SAMPLES; ++i)
    {
        graph->samples[i].x = (float)(i+1) / (float)(LAB_FPS_GRAPH_MAX_SAMPLES);
        //graph->samples[i].y -- changed by add sample

        int k = LAB_FPS_GRAPH_MAX_SAMPLES-i-1;
        graph->samples[i].color = LAB_RGBA(0, 0, 0, 255-k*k*256/LAB_FPS_GRAPH_MAX_SAMPLES/LAB_FPS_GRAPH_MAX_SAMPLES);
    }
    return 1;
}

void LAB_FpsGraph_Destroy(LAB_FpsGraph* graph)
{
}

void LAB_FpsGraph_AddSample(LAB_FpsGraph* graph, int frame_ms)
{
    LAB_FpsGraph_Shift(graph);
    graph->samples[LAB_FPS_GRAPH_MAX_SAMPLES-1].y = LAB_FPS_GRAPH_CALC_Y(frame_ms);
    graph->samples[LAB_FPS_GRAPH_MAX_SAMPLES-1].color |= LAB_RGBA(255, 255, 128, 0);
}

void LAB_FpsGraph_Render(LAB_FpsGraph* graph)
{
    if(graph->start_pos==LAB_FPS_GRAPH_MAX_SAMPLES) return;
    glDisable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);


    glVertexPointer(2, LAB_GL_TYPEOF(graph->samples[0].x), sizeof graph->samples[0], &graph->samples[graph->start_pos].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof graph->samples[0], &graph->samples[graph->start_pos].color);

    glDrawArrays(GL_LINE_STRIP, 0, LAB_FPS_GRAPH_MAX_SAMPLES-graph->start_pos);


    static const LAB_FpsGraphSample line60fps[] =
    {
        { 0, LAB_FPS_GRAPH_CALC_Y(1000/60), LAB_RGBA(0, 128, 255,   0), 0 },
        { 1, LAB_FPS_GRAPH_CALC_Y(1000/60), LAB_RGBA(0, 128, 255, 255), 0 },
        { 0, LAB_FPS_GRAPH_CALC_Y(1000/60), LAB_RGBA(0, 128, 255,   0), 0 },
    };

    glVertexPointer(2, LAB_GL_TYPEOF(line60fps[0].x), sizeof line60fps[0], &line60fps[0].x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof line60fps[0], &line60fps[0].color);

    glDrawArrays(GL_LINE_STRIP, 0, sizeof(line60fps)/sizeof(*line60fps));
}




















