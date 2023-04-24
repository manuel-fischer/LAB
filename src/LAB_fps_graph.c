#include "LAB_fps_graph.h"
#include "LAB_gl.h"
#include "LAB_debug.h"
#include "LAB_obj.h"

#include "LAB_view_renderer_vertex_spec.h"

#define LAB_FPS_GRAPH_CALC_Y(frame_ms) ((float)(frame_ms)*0.005f)

LAB_STATIC
bool LAB_FpsGraph_Obj(LAB_FpsGraph* graph, LAB_OBJ_Action action)
{
    LAB_PRECONDITION(LAB_PROPER(graph));

    LAB_BEGIN_OBJ(action);

    LAB_OBJ(LAB_GL_OBJ_ALLOC(glCreateVertexArrays, &graph->vao),
            LAB_GL_OBJ_FREE(glDeleteVertexArrays, &graph->vao),

    LAB_OBJ(LAB_GL_OBJ_ALLOC(glCreateBuffers, &graph->vbo),
            LAB_GL_OBJ_FREE(glDeleteBuffers, &graph->vbo),

        LAB_YIELD_OBJ(true);
    ););

    LAB_END_OBJ(false);
}



bool LAB_FpsGraph_Create(LAB_FpsGraph* graph, LAB_Color color)
{
    if(!LAB_FpsGraph_Obj(graph, LAB_OBJ_CREATE)) return false;


    // setup OpenGL array and buffer
    //LAB_View_VertexSpec_Realize_Format(graph->vao, &LAB_fps_graph_vertex_spec);
    //LAB_View_VertexSpec_Realize_Buffer(graph->vao, graph->vbo, &LAB_fps_graph_vertex_spec);
    LAB_GL_CHECK();

    // setup data
    graph->start_pos = LAB_FPS_GRAPH_MAX_SAMPLES;
    //graph->color = color;
    // Fill in constant values
    for(int i = 0; i < LAB_FPS_GRAPH_MAX_SAMPLES; ++i)
    {
        graph->samples[i].pos.x = (float)(i+1) / (float)(LAB_FPS_GRAPH_MAX_SAMPLES);
        graph->samples[i].pos.y = 0;
        //graph->samples[i].y -- changed by add sample

        int k = LAB_FPS_GRAPH_MAX_SAMPLES-i-1;
        //graph->samples[i].color = LAB_RGBA(0, 0, 0, 255-k*k*256/LAB_FPS_GRAPH_MAX_SAMPLES/LAB_FPS_GRAPH_MAX_SAMPLES);
        graph->samples[i].color = LAB_RGBA(0, 0, 0, 255-k*k*256/LAB_FPS_GRAPH_MAX_SAMPLES/LAB_FPS_GRAPH_MAX_SAMPLES)|(color&LAB_COL_MASK);
    }
    return true;
}

void LAB_FpsGraph_Destroy(LAB_FpsGraph* graph)
{
    LAB_FpsGraph_Obj(graph, LAB_OBJ_DESTROY);
}

void LAB_FpsGraph_Fill(LAB_FpsGraph* graph, float frame_ms)
{
    graph->start_pos = 0;
    for(int i = 0; i < LAB_FPS_GRAPH_MAX_SAMPLES; ++i)
    {
        graph->samples[i].pos.y = LAB_FPS_GRAPH_CALC_Y(frame_ms);
    }
}

void LAB_FpsGraph_Shift(LAB_FpsGraph* graph)
{
    LAB_PRECONDITION(LAB_PROPER(graph));

    // shift all the samples to the left, keeping the alpha channel of the
    // color and the x position at the original position
    if(graph->start_pos != 0) graph->start_pos--;
    for(int i = graph->start_pos; i < LAB_FPS_GRAPH_MAX_SAMPLES-1; ++i)
    {
        //graph->samples[i].x kept
        graph->samples[i].pos.y = graph->samples[i+1].pos.y;
        //graph->samples[i].color &= LAB_ALP_MASK;
        //graph->samples[i].color |= graph->samples[i+1].color & LAB_COL_MASK;
    }
}

void LAB_FpsGraph_SetSample(LAB_FpsGraph* graph, float frame_ms)
{
    LAB_PRECONDITION(LAB_PROPER(graph));
    //LAB_PRECONDITION(graph->start_pos!=LAB_FPS_GRAPH_MAX_SAMPLES); // TODO

    graph->samples[LAB_FPS_GRAPH_MAX_SAMPLES-1].pos.y = LAB_FPS_GRAPH_CALC_Y(frame_ms);
    //graph->samples[LAB_FPS_GRAPH_MAX_SAMPLES-1].color |= LAB_RGBA(255, 255, 128, 0);
}

void LAB_FpsGraph_SetSampleOffset(LAB_FpsGraph* graph, float frame_ms)
{
    LAB_PRECONDITION(LAB_PROPER(graph));
    //LAB_PRECONDITION(graph->start_pos!=LAB_FPS_GRAPH_MAX_SAMPLES); // TODO

    graph->samples[LAB_FPS_GRAPH_MAX_SAMPLES-1].pos.y = LAB_FPS_GRAPH_CALC_Y(frame_ms);
    graph->samples[LAB_FPS_GRAPH_MAX_SAMPLES-2].pos.y = LAB_FPS_GRAPH_CALC_Y(frame_ms);
    //graph->samples[LAB_FPS_GRAPH_MAX_SAMPLES-1].color |= LAB_RGBA(255, 255, 128, 0);
}

void LAB_FpsGraph_AddSample(LAB_FpsGraph* graph, float frame_ms)
{
    LAB_PRECONDITION(LAB_PROPER(graph));

    LAB_FpsGraph_Shift(graph);
    LAB_FpsGraph_SetSample(graph, frame_ms);
}

void LAB_FpsGraph_Render_Prepare(LAB_ViewRenderer* r)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    LAB_ViewProgram_Use(&r->fps_graph.program);
}

void LAB_FpsGraph_Render(LAB_FpsGraph* graph)
{
    LAB_PRECONDITION(LAB_PROPER(graph));

    if(graph->start_pos==LAB_FPS_GRAPH_MAX_SAMPLES) return;

    LAB_GL_CHECK();
    glBindVertexArray(graph->vao.id);
    LAB_GL_CHECK();
    LAB_GL_IgnoreInfo(131185);
    glNamedBufferData(graph->vbo.id, sizeof graph->samples, &graph->samples, GL_STREAM_DRAW);
    LAB_GL_ResetIgnoreInfo();
    LAB_GL_CHECK();
    LAB_View_VertexSpec_Realize(graph->vao, graph->vbo, &LAB_fps_graph_vertex_spec);
    LAB_GL_CHECK();
    glDrawArrays(GL_LINE_STRIP, graph->start_pos, LAB_FPS_GRAPH_MAX_SAMPLES-graph->start_pos);
    LAB_GL_CHECK();
}





















