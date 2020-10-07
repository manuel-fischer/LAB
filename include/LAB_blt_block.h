#pragma once

#include "LAB_blt_model.h"

#define LAB_ENM_STONE_BLOCKS(prefix, name) \
    prefix LAB_BLOCK_##name, \
    prefix LAB_BLOCK_##name##_COBBLE, \
    prefix LAB_BLOCK_##name##_BRICKS, \
    prefix LAB_BLOCK_##name##_SMOOTH

#define LAB_DEF_STONE_BLOCKS(name, color) \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name, 0, 0, color); \
    LAB_Block LAB_BLOCK_##name = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 0, 0, \
        .model = &LAB_MODEL_##name, \
    }; \
    \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name##_COBBLE, 1, 0, color); \
    LAB_Block LAB_BLOCK_##name##_COBBLE = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 1, 0, \
        .model = &LAB_MODEL_##name##_COBBLE, \
    }; \
    \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name##_BRICKS, 1, 1, color); \
    LAB_Block LAB_BLOCK_##name##_BRICKS = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 1, 1, \
        .model = &LAB_MODEL_##name##_BRICKS, \
    }; \
    \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name##_SMOOTH, 0, 1, color); \
    LAB_Block LAB_BLOCK_##name##_SMOOTH = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 0, 1, \
        .model = &LAB_MODEL_##name##_SMOOTH, \
    }

