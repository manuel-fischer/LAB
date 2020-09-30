#pragma once

#include "LAB_stdinc.h"
#include "LAB_color.h"
#include "LAB_model.h"

#include "IXO/IXO_classdef.h"

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
#define LAB_BLOCK_OPAQUE       2u // Other blocks get visually covered by this block
#define LAB_BLOCK_VISUAL       4u // The block is rendered
#define LAB_BLOCK_INTERACTABLE 8u // The block can be interacted with
#define LAB_BLOCK_SOLID        (LAB_BLOCK_MASSIVE|LAB_BLOCK_OPAQUE|LAB_BLOCK_INTERACTABLE|LAB_BLOCK_VISUAL)

#define LAB_BLOCK_TRANSPARENT  16u // The block has some transparent pixels
                                   // LAB_BLOCK_SOLID should not be set, otherwise it has an XRay-effect

#define LAB_BLOCK_FLAT_SHADE   64u // should be in the model
#define LAB_BLOCK_EMISSIVE     128u
#define LAB_BLOCK_GLOWING      (LAB_BLOCK_FLAT_SHADE|LAB_BLOCK_EMISSIVE)



typedef struct LAB_Block
{
    LAB_BlockFlags flags;
    LAB_Color tint; // REMOVE
    LAB_Color lum;
    LAB_Color dia; // diaphanety/transparency color of the block, the incoming light gets multiplied with this color,
                   // should be #ffffff for completely transparent blocks.
    uint8_t tx, ty; // Offset of the texture tile // REMOVE

    LAB_Model const* model;
} LAB_Block;

IXO_DECLARE_CLASS(LAB_Block);


extern LAB_Block LAB_BLOCK_OUTSIDE,
                 LAB_BLOCK_AIR,
                 LAB_BLOCK_BASALT,
                 LAB_BLOCK_BASALTCOBBLE,
                 LAB_BLOCK_BASALTBRICKS,
                 LAB_BLOCK_SMOOTHBASALT,
                 LAB_BLOCK_STONE,
                 LAB_BLOCK_COBBLESTONE,
                 LAB_BLOCK_STONEBRICKS,
                 LAB_BLOCK_SMOOTHSTONE,
                 LAB_BLOCK_MARBLE,
                 LAB_BLOCK_MARBLECOBBLE,
                 LAB_BLOCK_MARBLEBRICKS,
                 LAB_BLOCK_SMOOTHMARBLE,
                 LAB_BLOCK_SANDSTONE,
                 LAB_BLOCK_SANDCOBBLE,
                 LAB_BLOCK_SANDBRICKS,
                 LAB_BLOCK_SMOOTHSAND,
                 LAB_BLOCK_SAND,
                 LAB_BLOCK_LEAVES,
                 LAB_BLOCK_WOOD,
                 LAB_BLOCK_WOOD_PLANKS,
                 LAB_BLOCK_WOOD_PLANKS_DARK,
                 LAB_BLOCK_GRASS,
                 LAB_BLOCK_DIRT,
                 LAB_BLOCK_LIGHT,
                 LAB_BLOCK_WARM_LIGHT,
                 LAB_BLOCK_BLUE_LIGHT,
                 LAB_BLOCK_YELLOW_LIGHT,
                 LAB_BLOCK_GREEN_LIGHT,
                 LAB_BLOCK_RED_LIGHT,
                 LAB_BLOCK_INVISIBLE_LIGHT,
                 LAB_BLOCK_BARRIER;

LAB_Block** LAB_blocks;
extern size_t LAB_block_count;
