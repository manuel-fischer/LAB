#pragma once

enum LAB_ChunkUpdate_Enum
{
    LAB_CHUNK_UPDATE_LIGHT = 1, // light changed, usually by neighboring chunk
    LAB_CHUNK_UPDATE_BLOCK_ = 2, // block changed, usually in combination with light
    LAB_CHUNK_UPDATE_BLOCK = 2+1,
    LAB_CHUNK_UPDATE_LOCAL = 2+0*4, // the chunk itself has changed -> higher priority
                                // when only the neighboring chunk had changed, the update
                                // is not propagated (TODO)
};
typedef int LAB_ChunkUpdate;


/**
 * 
 *   
 * 
 * 
 * 
 * 
 * 
**/


enum LAB_ChunkUpdateStage
{
    LAB_CHUNK_STAGE_GENERATE,
    LAB_CHUNK_STAGE_LIGHT,
    LAB_CHUNK_STAGE_VIEW_NOTIFY,
    LAB_CHUNK_STAGE_VIEW_MESH,
    LAB_CHUNK_STAGE_COMPLETE,
};