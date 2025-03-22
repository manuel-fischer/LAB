#include "LAB_builtin.h"
#include "LAB_builtin_blocks.h"
#include "LAB_builtin_dimensions.h"
#include "LAB_error_state.h"

LAB_Err LAB_Builtin_Init(LAB_Assets* assets)
{
    LAB_TRY(LAB_BuiltinBlocks_Init(assets));
    LAB_TRY(LAB_BuiltinDimensions_Init());
    return LAB_OK;
}