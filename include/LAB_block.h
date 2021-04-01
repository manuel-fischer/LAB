#pragma once

#include "LAB_stdinc.h"
#include "LAB_color.h"
#include "LAB_model.h"

#include "LAB_blt_block.h"
#include "LAB_aabb.h"

//#include "IXO/IXO_classdef.h"

/**
 *  TODO Blockmodels:
 *       - Each Triangle has an occlusion flag, that leads to
 *         the triangle being disabled if all the neighboring blocks
 *         have the flag LAB_BLOCK_SOLID set
 *         +-+-+-+-+-+-+-+-+
 *         | | |S|N|U|D|E|W|
 *         +-+-+-+-+-+-+-+-+
 *         If all flags are set to 0, the triangle is always visible
 *       - Each vertex has a light position, that selects an interpolated color
 *
 *       Renderpasses:
 *       - A model
 */

typedef uint32_t LAB_BlockFlags;

#define LAB_BLOCK_MASSIVE      1u // Entities collide with this
#define LAB_BLOCK_OPAQUE       2u // Any block gets visually covered by this block
#define LAB_BLOCK_OPAQUE_SELF  4u // This block (TODO: Blocks with the same material id or tag) get visually covered by this block
#define LAB_BLOCK_VISUAL       8u // The block is rendered
#define LAB_BLOCK_INTERACTABLE 16u // The block can be interacted with
#define LAB_BLOCK_SOLID        (LAB_BLOCK_MASSIVE|LAB_BLOCK_OPAQUE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL)

#define LAB_BLOCK_TRANSPARENT  32u // The block has some transparent pixels
                                   // LAB_BLOCK_SOLID should not be set, otherwise it has an XRay-effect

#define LAB_BLOCK_FLAT_SHADE   64u // should be in the model
#define LAB_BLOCK_EMISSIVE     128u
#define LAB_BLOCK_GLOWING      (LAB_BLOCK_FLAT_SHADE|LAB_BLOCK_EMISSIVE)
#define LAB_BLOCK_NOSHADE      256u



typedef struct LAB_Block
{
    LAB_BlockFlags flags;
    LAB_Color lum;
    LAB_Color dia; // diaphanety/transparency color of the block, the incoming light gets multiplied with this color,
                   // should be #ffffff for completely transparent blocks.
    LAB_Color item_tint;
    uint8_t item_tx, item_ty; // Offset of the texture tile for the item, might be removed

    LAB_Model const* model;
    float bounds[2][3];
} LAB_Block;

//IXO_DECLARE_CLASS(LAB_Block);


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
                 LAB_BLOCK_BLUE_CRYSTAL,
                 LAB_BLOCK_YELLOW_CRYSTAL,
                 LAB_BLOCK_GREEN_CRYSTAL,
                 LAB_BLOCK_RED_CRYSTAL,
                 LAB_BLOCK_INVISIBLE_LIGHT,
                 LAB_BLOCK_BARRIER;

extern LAB_Block** LAB_blocks;
extern size_t LAB_block_count;
