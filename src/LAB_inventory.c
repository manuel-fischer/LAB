#include "LAB_inventory.h"

#include "LAB_block.h"

LAB_STATIC size_t     (LAB_CheatInventory_GetSize)(void*);
LAB_STATIC LAB_Block* (LAB_CheatInventory_GetSlot)(void*, size_t index);
LAB_STATIC bool       (LAB_CheatInventory_PutSlot)(void*, size_t index);
LAB_STATIC LAB_Block* (LAB_CheatInventory_TakeSlot)(void*, size_t index);

const LAB_IInventory LAB_cheat_inventory = {
    .get_size = &LAB_CheatInventory_GetSize,
    .get_slot = &LAB_CheatInventory_GetSlot,
    .put_slot = &LAB_CheatInventory_PutSlot,
    .take_slot = &LAB_CheatInventory_TakeSlot,
};

size_t     (LAB_CheatInventory_GetSize)(void* user)
{
    return LAB_block_count;
}

LAB_Block* (LAB_CheatInventory_GetSlot)(void* user, size_t index)
{
    return LAB_blocks[index];
}

bool       (LAB_CheatInventory_PutSlot)(void* user, size_t index)
{
    return 0;
}
LAB_Block* (LAB_CheatInventory_TakeSlot)(void* user, size_t index)
{
    *(LAB_Block**)user = LAB_blocks[index];
    return NULL;
}
