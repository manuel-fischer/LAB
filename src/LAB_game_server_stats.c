#include "LAB_game_server_stats.h"

void LAB_GameServerStats_RenderTo(const LAB_GameServerStats* server_stats, const LAB_World* world, const LAB_View* view, LAB_TextRenderer* r)
{
    int chunk_count = world->chunks.size ? world->chunks.size : 1;
    //int divV = LAB_client.view.chunks.size ? LAB_client.view.chunks.size : 1;

    LAB_TextRenderer_VSpace(r, 0.5f);
    LAB_TextRenderer_Printfln(r, "Max Age:%10i", server_stats->max_age);
    LAB_TextRenderer_Printfln(r, "Task Count:%7i", server_stats->completed_task_count);
    LAB_TextRenderer_Printfln(r, "New:%14i", server_stats->new_task_count);
    LAB_TextRenderer_Printfln(r, "Overriden:%8i", server_stats->overriden_task_count);
    LAB_TextRenderer_Printfln(r, "Requeued:%9i", server_stats->requeued_count);
    LAB_TextRenderer_Printfln(r, "Queue:%7i  %2i%%", 
                server_stats->waiting_task_count,
                server_stats->waiting_task_count*100 / chunk_count);
    LAB_TextRenderer_Printfln(r, "Preferred:%8i", server_stats->preferred_use_count);


    LAB_TextRenderer_VSpace(r, 0.5f);
    LAB_TextRenderer_Printfln(r, "Thread Usage: %3i%%", server_stats->runtime_computed*100 / (server_stats->runtime+1));
    LAB_TextRenderer_Printfln(r, "GenCount:%9i", server_stats->update_counts[LAB_CHUNK_STAGE_GENERATE]);
    LAB_TextRenderer_Printfln(r, "LightCount:%7i", server_stats->update_counts[LAB_CHUNK_STAGE_LIGHT]);
    LAB_TextRenderer_Printfln(r, "MeshCount:%8i", server_stats->update_counts[LAB_CHUNK_STAGE_VIEW_MESH]);

    LAB_TextRenderer_Printfln(r, "GenTime:%9i%%", server_stats->update_runtimes[LAB_CHUNK_STAGE_GENERATE]*100 / (server_stats->runtime_computed+1));
    LAB_TextRenderer_Printfln(r, "LightTime:%7i%%", server_stats->update_runtimes[LAB_CHUNK_STAGE_LIGHT]*100 / (server_stats->runtime_computed+1));
    LAB_TextRenderer_Printfln(r, "MeshTime:%8i%%", server_stats->update_runtimes[LAB_CHUNK_STAGE_VIEW_MESH]*100 / (server_stats->runtime_computed+1));
    LAB_TextRenderer_Printfln(r, "GenQCount:%8i", server_stats->gen_queue_size);


    LAB_TextRenderer_VSpace(r, 0.5f);
    LAB_TextRenderer_Printfln(r, "WChunks:%10i", world->chunks.size);
    //LAB_TextRenderer_Printfln(r, "InQueue:%10i", server_stats->waiting_task_count*100 / chunk_count);
    LAB_TextRenderer_Printfln(r, "WProbe: %5i  %2i%%",
                world->chunks.dbg_max_probe,
                world->chunks.dbg_max_probe*100 / chunk_count
    );
    LAB_TextRenderer_Printfln(r, "Capacity:%9i", world->chunks.capacity);


    LAB_TextRenderer_VSpace(r, 0.5f);
    //LAB_TextRenderer_Printfln(r, "WPointer:%9i", game_server.update_pointer);
    LAB_TextRenderer_Printfln(r, "WCycles:%10i", server_stats->update_cycles);
    LAB_TextRenderer_Printfln(r, "UnloadCycles:%5i", server_stats->unload_cycles);
    LAB_TextRenderer_Printfln(r, "UnloadCount:%6i", server_stats->unload_count);
    LAB_TextRenderer_Printfln(r, "VCycles:%10i", server_stats->view_update_cycles);
    LAB_TextRenderer_Printfln(r, "MaxUpdates:%7i", server_stats->max_chunk_updates);
    LAB_TextRenderer_Printfln(r, "AvgUpdates:%7i", (int)(server_stats->avg_chunk_updates));


    LAB_TextRenderer_VSpace(r, 0.5f);
    LAB_TextRenderer_Printfln(r, "VChunks:%10i", view->chunk_array.entries_count);
    LAB_TextRenderer_Printfln(r, "VChunks<:%9i", view->chunk_array.entries_sorted_count);
    LAB_TextRenderer_Printfln(r, "VChunks?:%9i", view->chunk_array.entries_sorted_nonempty_count);
    LAB_TextRenderer_Printfln(r, "V/WChunks:%7i%%", view->chunk_array.entries_sorted_count*100/chunk_count);

    LAB_TextRenderer_Printfln(r, "VDelQueue:%8i", view->chunk_array.del_list_count);
    LAB_TextRenderer_Printfln(r, "VRecovered:%7i", view->stats.recovered_count);
    LAB_TextRenderer_Printfln(r, "VDeleted:%9i", view->stats.deleted);
}