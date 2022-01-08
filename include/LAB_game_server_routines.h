#pragma once
 
#include "LAB_game_server.h"

LAB_ChunkCallback LAB_ChunkStageCallback(int update);

void LAB_ChunkStageNeighbors(int update, LAB_Chunk* chunk, LAB_Chunk* chunks[27]);