#pragma once

#include "LAB_stdinc.h"
#include "LAB_block.h"

typedef struct LAB_IInventory
{
    size_t      (*get_size)(void*);
    LAB_BlockID (*get_slot)(void*, size_t index);
    bool        (*put_slot)(void*, size_t index);
    LAB_BlockID (*take_slot)(void*, size_t index);
} LAB_IInventory;
