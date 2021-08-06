#pragma once

#include "LAB_block.h"

extern LAB_Block** LAB_blocks;
extern size_t LAB_block_count;

bool LAB_Blocks_Init();
void LAB_Blocks_Quit();

bool LAB_RegisterBlock(LAB_Block* block);