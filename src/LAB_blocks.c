#include "LAB_blocks.h"
#include "LAB_memory.h"

LAB_Block* LAB_blocks_data = NULL;
size_t LAB_block_count = 0;
//static size_t LAB_block_capacity = 0;
LAB_BlockID* LAB_item_blocks = NULL;
size_t LAB_item_block_count = 0;

#if 0
_Thread_local LAB_BlockID _bid;
#endif

bool LAB_Blocks_Init(void)
{
    LAB_ARRAY_CREATE_EMPTY(LAB_blocks_array);
    LAB_ARRAY_CREATE_EMPTY(LAB_item_blocks_array);
    return true;
}

void LAB_Blocks_Quit(void)
{
    LAB_ARRAY_DESTROY(LAB_item_blocks_array);
    LAB_ARRAY_DESTROY(LAB_blocks_array);
}

LAB_BlockID LAB_RegisterBlocks(size_t count)
{
    if(LAB_block_count + count > LAB_BID_MAX) return LAB_BID_INVALID;
    LAB_Block* start;
    LAB_ARRAY_APPEND(LAB_blocks_array, count, &start);
    if(!start) return LAB_BID_INVALID;

    memset(start, 0, count*sizeof(LAB_Block));

    return start-LAB_ARRAY_DATA(LAB_blocks_array);
}

bool LAB_RegisterBlocksGen(LAB_BlockID* ids, size_t count)
{
    LAB_BlockID bid = LAB_RegisterBlocks(count);
    if(bid == LAB_BID_INVALID) return false;
    for(size_t i = 0; i < count; ++i) ids[i] = bid+i;

    return true;
}

bool LAB_AddBlockItems(LAB_BlockID block, size_t count)
{
    LAB_ASSERT(count == 1);
    LAB_BlockID* ids;
    LAB_ARRAY_APPEND(LAB_item_blocks_array, count, &ids);
    if(!ids) return false;
    for(size_t i = 0; i < count; ++i) ids[i] = block+i;
    return true;
}