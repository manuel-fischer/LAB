#pragma once

#include "LAB_block.h"
#include "LAB_array.h"
#include "LAB_error_state.h"

#define LAB_BID_NO_ITEM ((LAB_BlockID)0)
#define LAB_BID_AIR ((LAB_BlockID)0)
#define LAB_BID_OUTSIDE ((LAB_BlockID)1)

extern LAB_Block* LAB_blocks_data;
extern size_t LAB_block_count;
#define LAB_blocks_array (LAB_Block, LAB_blocks_data, LAB_block_count)

#if 0
extern _Thread_local LAB_BlockID _bid;
#define LAB_BlockP(bid) ( \
    _bid = (bid), \
    LAB_ASSERT(_bid < LAB_block_count), \
    &LAB_blocks_data[_bid] \
)
#elif 0 /*fun*/
//#define LAB_CORRUPTION_NOISE() (rand()|rand()|rand()|rand()|rand()|rand()|rand()|rand())
#define LAB_CORRUPTION_NOISE() (rand()|rand()|rand()|rand())
#define LAB_BlockP(bid) (&LAB_blocks_data[bid & LAB_CORRUPTION_NOISE()])
#else
#define LAB_BlockP(bid) (&LAB_blocks_data[bid])
#endif

extern LAB_BlockID* LAB_item_blocks;
extern size_t LAB_item_block_count;
#define LAB_item_blocks_array (LAB_BlockID, LAB_item_blocks, LAB_item_block_count)

bool LAB_Blocks_Init(void);
void LAB_Blocks_Quit(void);

LAB_Err LAB_RegisterBlocksGen(LAB_BlockID* ids, size_t count);
LAB_Err LAB_AddBlockItems(LAB_BlockID block, size_t count);