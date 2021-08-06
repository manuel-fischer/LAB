#pragma once
#include <stdbool.h>
#include "LAB_block.h"
#include "LAB_blt_block.h"


bool LAB_BuiltinBlocks_Init();

extern LAB_Block LAB_BLOCK_OUTSIDE,
                 LAB_BLOCK_AIR,
                 LAB_ENM_BLOCK_GROUP_STONE(,BASALT),
                 LAB_ENM_BLOCK_GROUP_STONE(,STONE),
                 LAB_ENM_BLOCK_GROUP_STONE(,MARBLE),
                 LAB_ENM_BLOCK_GROUP_STONE(,CLAY),
                 LAB_ENM_BLOCK_GROUP_STONE(,LAPIZ),
                 LAB_ENM_BLOCK_GROUP_STONE(,SANDSTONE),
                 LAB_BLOCK_SAND,
                 LAB_BLOCK_GLASS,
                 LAB_BLOCK_LEAVES,
                 LAB_BLOCK_WOOD,
                 LAB_BLOCK_WOOD_PLANKS,
                 LAB_BLOCK_WOOD_PLANKS_DARK,
                 LAB_BLOCK_BIRCH_LEAVES,
                 LAB_BLOCK_BIRCH_WOOD,
                 LAB_BLOCK_SPRUCE_LEAVES,
                 LAB_BLOCK_SPRUCE_WOOD,
                 LAB_BLOCK_TALLGRASS,
                 LAB_BLOCK_TALLERGRASS,
                 LAB_BLOCK_GRASS,
                 LAB_BLOCK_RED_TULIP,
                 LAB_BLOCK_YELLOW_TULIP,
                 LAB_BLOCK_FALLEN_LEAVES,
                 //LAB_BLOCK_GRASS_PLAINS,
                 LAB_BLOCK_DIRT,
                 LAB_BLOCK_METAL,
                 LAB_BLOCK_TORCH,
                 LAB_BLOCK_LIGHT,
                 LAB_BLOCK_WARM_LIGHT,
                 LAB_BLOCK_BLUE_LIGHT,
                 LAB_BLOCK_YELLOW_LIGHT,
                 LAB_BLOCK_GREEN_LIGHT,
                 LAB_BLOCK_RED_LIGHT,
                 LAB_BLOCK_ORANGE_LIGHT,
                 LAB_BLOCK_BLUE_CRYSTAL,
                 LAB_BLOCK_YELLOW_CRYSTAL,
                 LAB_BLOCK_GREEN_CRYSTAL,
                 LAB_BLOCK_RED_CRYSTAL,
                 LAB_BLOCK_INVISIBLE_LIGHT,
                 LAB_BLOCK_BARRIER;
