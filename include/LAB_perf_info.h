#pragma once

#include "LAB_fps_graph.h"

enum LAB_TimeGraph
{
    LAB_TG_WHOLE,
    LAB_TG_INPUT,
    LAB_TG_WORLD,
    LAB_TG_VIEW,
    LAB_TG_VIEW_RENDER,
    LAB_TG_VIEW_RENDER_UPLOAD,
    LAB_TG_VIEW_RENDER_QUERY,
    LAB_TG_MESH,
// =====================
    LAB_TG_COUNT,

    LAB_TG_NONE = ~0
};

enum LAB_CountGraph
{
    LAB_CG_CHUNK_LOAD
};

typedef struct LAB_PerfInfo
{
    uint64_t enabled; // if 0, all are enabled again
    enum LAB_TimeGraph current_graph;
    LAB_Nanos current_time;

    LAB_FpsGraph fps_graphs[LAB_TG_COUNT];
} LAB_PerfInfo;


int  LAB_PerfInfo_Create(LAB_PerfInfo* perf_info);
void LAB_PerfInfo_Destroy(LAB_PerfInfo* perf_info);
void LAB_PerfInfo_Render(LAB_PerfInfo* perf_info);


void LAB_PerfInfo_Tick(LAB_PerfInfo* perf_info);
void LAB_PerfInfo_Next(LAB_PerfInfo* perf_info, enum LAB_TimeGraph graph);

void LAB_PerfInfo_FinishNS(LAB_PerfInfo* perf_info, enum LAB_TimeGraph graph, LAB_Nanos ns_start);