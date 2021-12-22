#pragma once

#include "LAB_view_mesh.h"
#include "LAB_model_order.h"
#include "LAB_thread.h"

#define LAB_VIEW_QUERY_IMMEDIATELY 0
#define LAB_VIEW_ENABLE_QUERY 0

typedef struct LAB_Chunk LAB_Chunk;

typedef struct LAB_ViewChunkEntry
{
    int x, y, z;


    /*size_t mesh_count, mesh_capacity;
    LAB_Triangle* mesh;
    LAB_TriangleOrder* mesh_order;*

    unsigned vbo;*/
    //unsigned vbos[LAB_RENDER_PASS_COUNT];

    LAB_View_Mesh render_passes[LAB_RENDER_PASS_COUNT];
    LAB_TriangleOrder* alpha_mesh_order; // for LAB_RENDER_PASS_ALPHA
    size_t             alpha_mesh_size;
    size_t             alpha_mesh_capacity;

    //size_t render_delay;

    /*struct LAB_ViewChunkEntry* neighbors[6]; // - make table entries to pointers, remove occupied flag
                                             // - point to neighbor entries here,
                                             //   when a new entry is created:
                                             //   - iterate neighboring chunk entries
                                             //     by looking them up in the hash table
                                             //   - add the chunk entry itself to them
                                             //   - add neighboring chunk entries to itself
                                             // -> use entry->neighbors[face] instead of table lookup with x+LAB_OX(face), ...*/
    struct LAB_ViewChunkEntry* del_list_next;

    LAB_Chunk* world_chunk; // pointer into the world, is NULL when there is no chunk in the world

    unsigned dirty:2,            // chunk needs update
             visible:1,          // chunk is visible when in sight, changed on every query
                                 // -> used to update chunks
                                 // -> possibly used to render chunks, if the orientation of the camera is stable
             sight_visible:1,    // chunk is currently visible, changed on every query,
                                 // except when chunk was not completely in the view, that is when the chunk was a little
                                 // bit outside of the screen, then the flag is never cleared
                                 // -> used to render chunks; no invisible chunks, when the orientation of the camera changed
                                 // visible ==> sight_visible <==> ~sight_visible ==> ~visible
             do_query:1,         // visibility unknown, a query should be submitted
             upload_vbo:1;       // vbos changed, need reupload, gets set to 0 in last render pass

    LAB_Access is_accessed;  // chunk is currently accessed

    unsigned pad:1;
    unsigned visible_faces:6; // the faces that are currently rendered

    unsigned seethrough_faces:6; // TODO >>> if at least one block at the chunk border faces is not culled
                                 // set when the mesh is built
                                 // Allows fast optimization to check if a chunk is visible from another chunk.
                                 // if a face does not contain nonculled faces, the chunk is not visible from that
                                 // direction.

    #if 0
    unsigned delay_ticks; // delaying ticks to do updates
    #endif
    #if !LAB_VIEW_QUERY_IMMEDIATELY && LAB_VIEW_ENABLE_QUERY
    unsigned query_id; // 0 for no query done in the last frame
                       // gets generated in OrderQueryBlock
                       // gets deleted in FetchQueryBlock
                       // TODO: maybe use an array and allocate
                       //       a fixed number of query objects
    #endif
} LAB_ViewChunkEntry;
