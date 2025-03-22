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

LAB_Err LAB_RegisterBlocks(LAB_BlockID* first_id, size_t count)
{
    if(LAB_block_count + count > LAB_BID_MAX) return LAB_RAISE("limit of blocks reached");
    LAB_Block* start;
    LAB_ARRAY_APPEND(LAB_blocks_array, count, &start);
    if(!start) return LAB_RAISE_C();

    memset(start, 0, count*sizeof(LAB_Block));

    *first_id = start-LAB_ARRAY_DATA(LAB_blocks_array);

    return LAB_OK;
}

LAB_Err LAB_RegisterBlocksGen(LAB_BlockID* ids, size_t count)
{
    LAB_BlockID first_id;
    LAB_TRY(LAB_RegisterBlocks(&first_id, count));
    for(size_t i = 0; i < count; ++i) ids[i] = first_id+i;
    return LAB_OK;
}

LAB_Err LAB_AddBlockItems(LAB_BlockID block, size_t count)
{
    LAB_ASSERT(count == 1);
    LAB_BlockID* ids;
    LAB_ARRAY_APPEND(LAB_item_blocks_array, count, &ids);
    if(!ids) return LAB_RAISE_C();
    for(size_t i = 0; i < count; ++i) ids[i] = block+i;
    return LAB_OK;
}