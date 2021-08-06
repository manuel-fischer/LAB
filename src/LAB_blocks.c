#include "LAB_blocks.h"
#include "LAB_memory.h"

LAB_Block** LAB_blocks = NULL;
size_t LAB_block_count = 0;
static size_t LAB_block_capacity = 0;


bool LAB_Blocks_Init()
{
    return true;
}

void LAB_Blocks_Quit()
{
    LAB_Free(LAB_blocks);
}

bool LAB_RegisterBlock(LAB_Block* block)
{
    if(LAB_block_count == LAB_block_capacity) // Allocate memory
    {
        size_t new_capacity = LAB_block_capacity << 1;
        if(new_capacity == 0) new_capacity = 64;

        LAB_Block** new_blocks = LAB_ReallocN(LAB_blocks, new_capacity, sizeof(*new_blocks));
        if(new_blocks == NULL) return NULL; // NO MEMORY
        LAB_blocks = new_blocks;
        LAB_block_capacity = new_capacity;
    }

    LAB_blocks[LAB_block_count++] = block;
    return true;
}