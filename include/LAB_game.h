/** 
 * Hold information about game content
 * 
 * Childs
 * - LAB_blocks.h
 * - LAB_game_dimension.h
 * 
 * TODO
 * - rename LAB_game_server* to LAB_core_server (to avoid confusion, no childs)
 * - make datastructures (blocks, biomes...) part of this
**/

#pragma once

#include "LAB_stdinc.h"

#include "LAB_blocks.h"
#include "LAB_game_dimension.h"

/**
 * Specifies that the game content is changed by the function
 */
#define LAB_MUT_Game

bool LAB_Game_Init(void);
void LAB_Game_Quit(void);
