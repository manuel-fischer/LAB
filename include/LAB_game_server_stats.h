#pragma once

#include "LAB_stdinc.h"
#include "LAB_util.h"
#include "LAB_chunk_update.h"

#include "LAB_text_renderer.h"
#include "LAB_world.h"
#include "LAB_view.h"

typedef struct LAB_GameServerStats
{
    size_t completed_mainthread;

    size_t noop_spins;
    size_t spins;

    size_t timestamp;
    size_t max_age; // maximum age of chunks currently in the queue

    size_t completed_task_count;
    size_t waiting_task_count;
    size_t requeued_count;
    size_t new_task_count;
    size_t overriden_task_count;
    size_t preferred_use_count;

    size_t update_cycles;
    size_t unload_cycles;
    size_t unload_count;

    size_t view_update_cycles;

    LAB_Nanos runtime;
    LAB_Nanos runtime_computed;

    size_t max_chunk_updates;
    float  avg_chunk_updates;

    size_t gen_queue_size;

    size_t update_counts[LAB_CHUNK_STAGE_COUNT];
    LAB_Nanos update_runtimes[LAB_CHUNK_STAGE_COUNT];

} LAB_GameServerStats;


void LAB_GameServerStats_RenderTo(const LAB_GameServerStats* server_stats, const LAB_World* world, const LAB_View* client, LAB_TextRenderer* r);
