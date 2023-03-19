#include "LAB_builtin.h"
#include "LAB_builtin_blocks.h"
#include "LAB_builtin_dimensions.h"

void LAB_Builtin_Init(LAB_Assets* assets)
{
    LAB_BuiltinBlocks_Init(assets);
    LAB_BuiltinDimensions_Init();
}