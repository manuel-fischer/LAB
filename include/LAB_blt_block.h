#pragma once

#include "LAB_blt_model.h"


// TODO: remove LAB_BLOCK_ prefix

#define LAB_ENM_BLOCK_CUBE(prefix, name) \
    prefix LAB_BLOCK_##name

#define LAB_DEF_BLOCK_CUBE(name, tx, ty, color, block_dia, model_render_pass, block_flags) \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name, tx, ty, color, \
                    .render_pass = model_render_pass); \
    LAB_Block LAB_BLOCK_##name = { \
        .flags = block_flags, \
        .dia = block_dia, \
        .item_tint = color, \
        .item_tx = tx, ty, \
        .model = &LAB_MODEL_##name, \
        .bounds = LAB_AABB_FULL_CUBE, \
    }


#define LAB_ENM_BLOCK_GROUP_STONE(prefix, name) \
    prefix LAB_BLOCK_##name, \
    prefix LAB_BLOCK_##name##_COBBLE, \
    prefix LAB_BLOCK_##name##_BRICKS, \
    prefix LAB_BLOCK_##name##_SMOOTH

#define LAB_DEF_BLOCK_GROUP_STONE(name, color) \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name, 0, 0, color, \
                    .render_pass = LAB_RENDER_PASS_SOLID); \
    LAB_Block LAB_BLOCK_##name = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 0, 0, \
        .model = &LAB_MODEL_##name, \
        .bounds = LAB_AABB_FULL_CUBE, \
    }; \
    \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name##_COBBLE, 1, 0, color, \
                    .render_pass = LAB_RENDER_PASS_SOLID); \
    LAB_Block LAB_BLOCK_##name##_COBBLE = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 1, 0, \
        .model = &LAB_MODEL_##name##_COBBLE, \
        .bounds = LAB_AABB_FULL_CUBE, \
    }; \
    \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name##_BRICKS, 1, 1, color, \
                    .render_pass = LAB_RENDER_PASS_SOLID); \
    LAB_Block LAB_BLOCK_##name##_BRICKS = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 1, 1, \
        .model = &LAB_MODEL_##name##_BRICKS, \
        .bounds = LAB_AABB_FULL_CUBE, \
    }; \
    \
    LAB_DEF_MODEL_CUBE_ALL(LAB_MODEL_##name##_SMOOTH, 0, 1, color, \
                    .render_pass = LAB_RENDER_PASS_SOLID); \
    LAB_Block LAB_BLOCK_##name##_SMOOTH = { \
        .flags = LAB_BLOCK_SOLID, \
        .item_tint = color, \
        .item_tx = 0, 1, \
        .model = &LAB_MODEL_##name##_SMOOTH, \
        .bounds = LAB_AABB_FULL_CUBE, \
    }

