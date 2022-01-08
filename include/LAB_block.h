#pragma once

#include "LAB_stdinc.h"
#include "LAB_color.h"
#include "LAB_model.h"

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

#define LAB_BLOCK_TAG_REPLACEABLE  1



typedef struct LAB_Block
{
    LAB_BlockFlags flags;
    uint32_t tags;
    LAB_Color lum;
    LAB_Color dia; // diaphanety/transparency color of the block, the incoming light gets multiplied with this color,
                   // should be #ffffff for completely transparent blocks.

    LAB_Model const* model;
    SDL_Surface* item_texture;
    float bounds[2][3];
} LAB_Block;

//IXO_DECLARE_CLASS(LAB_Block);

