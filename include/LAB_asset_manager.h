#pragma once
/** \file LAB_asset_manager.h
 *
 *  Global asset management
 */

#include <SDL2/SDL_surface.h>

/** \def LAB_TILE_SIZE
 *
 *  size in pixels of a texture tile
 *
 *  \todo remove this
 */
#define LAB_TILE_SIZE 32

/**
 *  boolean flag if mipmaps are enabled
 *
 *  \todo remove this
 */
#define LAB_MIPMAPS 1

/**
 *  Texture atlas
 *
 *  \see LAB_block_terrain_gl_id
 */
SDL_Surface* LAB_block_terrain;

/**
 *  Reference to the OpenGL object of the texture atlas
 *
 *  \see LAB_block_terrain
 */
unsigned     LAB_block_terrain_gl_id;

/**
 *  Init assets
 *
 *  \todo what if it fails?
 */
void LAB_InitAssets(void);
/**
 *  Free assets
 *
 *  \pre \ref LAB_InitAssets was called
 */
void LAB_QuitAssets(void);
