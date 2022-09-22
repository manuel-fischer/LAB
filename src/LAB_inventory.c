#include "LAB_inventory.h"

#include "LAB_block.h"
#include "LAB_blocks.h" // -> LAB_blocks, LAB_block_count

LAB_STATIC size_t      (LAB_CheatInventory_GetSize)(void*);
LAB_STATIC LAB_BlockID (LAB_CheatInventory_GetSlot)(void*, size_t index);
LAB_STATIC bool        (LAB_CheatInventory_PutSlot)(void*, size_t index);
LAB_STATIC LAB_BlockID (LAB_CheatInventory_TakeSlot)(void*, size_t index);

const LAB_IInventory LAB_cheat_inventory = {
    .get_size = &LAB_CheatInventory_GetSize,
    .get_slot = &LAB_CheatInventory_GetSlot,
    .put_slot = &LAB_CheatInventory_PutSlot,
    .take_slot = &LAB_CheatInventory_TakeSlot,
};

size_t      (LAB_CheatInventory_GetSize)(void* user)
{
    return LAB_item_block_count;
}

LAB_BlockID (LAB_CheatInventory_GetSlot)(void* user, size_t index)
{
    return LAB_item_blocks[index];
}

bool        (LAB_CheatInventory_PutSlot)(void* user, size_t index)
{
    return 0;
}
LAB_BlockID (LAB_CheatInventory_TakeSlot)(void* user, size_t index)
{
    *(LAB_BlockID*)user = LAB_item_blocks[index];
    return LAB_BID_NO_ITEM;
}
