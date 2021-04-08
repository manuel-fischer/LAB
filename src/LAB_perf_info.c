#include "LAB_perf_info.h"

#include "LAB_debug.h"

static LAB_Color LAB_time_graph_colors[LAB_TG_COUNT] =
{
    [LAB_TG_WHOLE      ] = LAB_RGB(255, 255, 128),
    [LAB_TG_INPUT      ] = LAB_RGB(255, 128, 128),
    [LAB_TG_WORLD      ] = LAB_RGB(128, 255, 128),
    [LAB_TG_VIEW       ] = LAB_RGB(128, 128, 255),
    [LAB_TG_VIEW_RENDER] = LAB_RGB(255,  32, 128),
    [LAB_TG_VIEW_RENDER_UPLOAD] = LAB_RGB(255, 0, 255),
    [LAB_TG_VIEW_RENDER_QUERY] = LAB_RGB(0,  0,  0),
    [LAB_TG_MESH       ] = LAB_RGB(32,  192, 192),
};


int LAB_PerfInfo_Create(LAB_PerfInfo* perf_info)
{
    LAB_PRECONDITION(LAB_PROPER(perf_info));

    for(int i = 0; i < LAB_TG_COUNT; ++i)
        LAB_FpsGraph_Create(&perf_info->fps_graphs[i], LAB_time_graph_colors[i]);

    perf_info->current_graph = LAB_TG_NONE;

    return 1;
}

void LAB_PerfInfo_Destroy(LAB_PerfInfo* perf_info)
{
    LAB_PRECONDITION(LAB_PROPER(perf_info));

    for(int i = 0; i < LAB_TG_COUNT; ++i)
        LAB_FpsGraph_Destroy(&perf_info->fps_graphs[i]);
}



void LAB_PerfInfo_Render(LAB_PerfInfo* perf_info)
{
    LAB_PRECONDITION(LAB_PROPER(perf_info));

    LAB_FpsGraph_Render_Prepare();
    LAB_FpsGraph_Render_Base();
    uint64_t enabled = perf_info->enabled;
    if(!enabled) enabled = ~0;

    for(int i = 0; i < LAB_TG_COUNT; ++i)
        if(enabled & 1<<i)
            LAB_FpsGraph_Render(&perf_info->fps_graphs[i]);
}


void LAB_PerfInfo_Tick(LAB_PerfInfo* perf_info)
{
    LAB_PRECONDITION(LAB_PROPER(perf_info));
    LAB_PRECONDITION(perf_info->current_graph == LAB_TG_NONE);

    for(int i = 0; i < LAB_TG_COUNT; ++i)
        LAB_FpsGraph_Shift(&perf_info->fps_graphs[i]);
}

void LAB_PerfInfo_Next(LAB_PerfInfo* perf_info, enum LAB_TimeGraph graph)
{
    LAB_PRECONDITION(LAB_PROPER(perf_info));
    LAB_PRECONDITION(graph < LAB_TG_COUNT);

    LAB_Nanos t = LAB_NanoSeconds();

    if(perf_info->current_graph != LAB_TG_NONE)
    {
        float x = (float)(t-perf_info->current_time)*0.000001f;
        LAB_FpsGraph_SetSample(&perf_info->fps_graphs[perf_info->current_graph], x);
    }
    perf_info->current_graph = graph;
    perf_info->current_time  = t;
}


void LAB_PerfInfo_FinishNS(LAB_PerfInfo* perf_info, enum LAB_TimeGraph graph, LAB_Nanos ns_start)
{
    LAB_PRECONDITION(LAB_PROPER(perf_info));
    LAB_PRECONDITION(graph < LAB_TG_COUNT);

    LAB_Nanos t = LAB_NanoSeconds();

    float x = (float)(t-ns_start)*0.000001f;
    LAB_FpsGraph_SetSample(&perf_info->fps_graphs[graph], x);
}
