#pragma once
#include <stdbool.h>
#include "LAB_block.h"
#include "LAB_blt_block.h"
#include "LAB_assets.h"
#include "LAB_builtin_block_types.h"


#define LAB_BUILTIN_VIS extern
#include "LAB_builtin_blocks_list.h"
#undef LAB_BUILTIN_VIS

bool LAB_BuiltinBlocks_Init(LAB_Assets* assets);
