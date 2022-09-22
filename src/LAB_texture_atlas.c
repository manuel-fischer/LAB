#include "LAB_texture_atlas.h"
#include "LAB_memory.h"
#include "LAB_arith.h"
#include "LAB_image.h"

#include "LAB_sdl.h" // LAB_SDL_FREE -- TODO
#include "LAB_gl.h"

bool LAB_TexAlloc_Create(LAB_TexAlloc* alloc)
{
    LAB_ARRAY_CREATE(LAB_TexAlloc_Nodes(alloc));

    /*LAB_TexAlloc_Node* node;
    LAB_ARRAY_APPEND(LAB_TexAlloc_Nodes(alloc), 1, node);
    if(!node) return false;
    memset(node[0].states, LAB_TexAlloc_EMPTY, sizeof(node.childs[0]));
    node[0].parent = LAB_TexAlloc_Node_NULL;
    node[0].empty_cells = 4;

    alloc->root_node = 0;
    alloc->root_size = 2;
    alloc->next_unoccupied = LAB_TexAlloc_Node_NULL;*/
    alloc->root_node = LAB_TexAlloc_Node_NULL;
    alloc->root_size = 0;
    alloc->next_unoccupied = LAB_TexAlloc_Node_NULL;

    return true;
}

void LAB_TexAlloc_Destroy(LAB_TexAlloc* alloc)
{
    LAB_ARRAY_DESTROY(LAB_TexAlloc_Nodes(alloc));

}

LAB_STATIC size_t LAB_TexAlloc_NewNode_Alloc(LAB_TexAlloc* alloc)
{
    size_t node;
    #define NODE (alloc->nodes[node])
    if(alloc->next_unoccupied != LAB_TexAlloc_Node_NULL)
    {
        node = alloc->next_unoccupied;
        alloc->next_unoccupied = NODE.next_unoccupied;
    }
    else
    {
        // Allocate new node
        LAB_TexAlloc_Node* node_p;
        LAB_ARRAY_APPEND(LAB_TexAlloc_Nodes(alloc), 1, node_p);
        if(!node_p) return LAB_TexAlloc_Node_NULL;
        node = node_p - alloc->nodes;
    }
    #undef NODE
    return node;
}

// creates and clears node, sets states[] to empty
LAB_STATIC size_t LAB_TexAlloc_NewNode(LAB_TexAlloc* alloc, size_t parent)
{
    size_t node = LAB_TexAlloc_NewNode_Alloc(alloc);
    #define NODE (alloc->nodes[node])
    if(node == LAB_TexAlloc_Node_NULL) return node;

    // memset(&NODE, 0xEE, sizeof(NODE)); // debug

    memset(NODE.states, LAB_TexAlloc_EMPTY, sizeof(NODE.states));
    NODE.parent = parent;
    #undef NODE
    return node;
}

LAB_STATIC void LAB_TexAlloc_DelNode(LAB_TexAlloc* alloc, size_t node)
{
    #define NODE (alloc->nodes[node])
    NODE.next_unoccupied = alloc->next_unoccupied;
    alloc->next_unoccupied = node;
    #undef NODE
}


bool LAB_TexAlloc_Add(LAB_TexAlloc* alloc, size_t side_cells, LAB_OUT size_t topleft_cell[2])
{
    // traverse the quadtree and find empty space
    size_t required_space = side_cells*side_cells;
    size_t current_size = alloc->root_size;
    size_t cur_x = 0, cur_y = 0;

    size_t node = alloc->root_node;
    #define NODE (alloc->nodes[node])
    if(current_size > side_cells)
    {
        bool downwards = true;
        while(node != LAB_TexAlloc_Node_NULL)
        {
            LAB_ASSUME(current_size);
            LAB_ASSUME(current_size > side_cells); // at least as twice as big
            // Alignment:
            LAB_ASSUME((cur_x & (current_size-1)) == 0u);
            LAB_ASSUME((cur_y & (current_size-1)) == 0u);

            if(downwards)
            {
                if(required_space > NODE.empty_cells)
                {
                    downwards = false;
                    node = NODE.parent;
                    current_size <<= 1;
                    cur_x &= ~(current_size-1);
                    cur_y &= ~(current_size-1);
                    continue;
                }

                NODE.iter_state = 0u;
            }
            else
            {
                LAB_ASSERT(NODE.iter_state < 4);
                ++NODE.iter_state;
                if(NODE.iter_state == 4u) // everything checked in in this node
                {
                    //downwards = false;
                    node = NODE.parent;
                    current_size <<= 1;
                    cur_x &= ~(current_size-1);
                    cur_y &= ~(current_size-1);
                    continue;
                }
            }

            LAB_ASSERT(NODE.iter_state < 4);

            switch(NODE.states[NODE.iter_state])
            {
                case LAB_TexAlloc_EMPTY:
                {
                    if(NODE.iter_state & 1u) cur_x |= current_size>>1;
                    if(NODE.iter_state & 2u) cur_y |= current_size>>1;
                    goto insert_here;
                } break;
                case LAB_TexAlloc_MIXED:
                {
                    if(current_size == 2*side_cells) // it does not fit in anyting smaller
                    {
                        downwards = false;
                        continue;
                    }

                    downwards = true;
                    if(NODE.iter_state & 1u) cur_x |= current_size>>1;
                    if(NODE.iter_state & 2u) cur_y |= current_size>>1;
                    node = NODE.childs[NODE.iter_state];
                    current_size >>= 1;
                    continue;
                } break;
                case LAB_TexAlloc_FULL:
                {
                    downwards = false;
                    continue;
                } break;
            }
        }
        // no space found
    }


    {
        if(alloc->root_node == LAB_TexAlloc_Node_NULL)
        {
            node = LAB_TexAlloc_NewNode(alloc, LAB_TexAlloc_Node_NULL);
            if(node == LAB_TexAlloc_Node_NULL) return false;
            alloc->root_node = node;
            alloc->root_size = side_cells*2u;
            NODE.states[0] = LAB_TexAlloc_FULL;
            NODE.empty_cells = required_space*4u;
            //cur_x = 0;
            //cur_y = 0;
            NODE.iter_state = 0;
        }
        else
        {
            node = alloc->root_node;
            size_t space_root = alloc->root_size*alloc->root_size;
            size_t filled_space_old_root = space_root - NODE.empty_cells;

            do
            {
                size_t new_node = LAB_TexAlloc_NewNode(alloc, LAB_TexAlloc_Node_NULL);
                if(new_node == LAB_TexAlloc_Node_NULL) return false;
                NODE.parent = new_node;
                node = new_node;

                space_root *= 4u;
                NODE.childs[0] = alloc->root_node;
                NODE.states[0] = LAB_TexAlloc_MIXED;
                NODE.empty_cells = space_root - filled_space_old_root;
                alloc->root_node = node;
                alloc->root_size *= 2u;
            }
            while(alloc->root_size < 2*side_cells);

            NODE.iter_state = 1u;
            cur_x = alloc->root_size / 2u;
            cur_y = 0u;
        }

        current_size = alloc->root_size;
        goto insert_here;
    }


    insert_here:
    {
        // TODO: allocate here
        size_t space = current_size*current_size;
        while(true)
        {
            if(current_size == 2u*side_cells) // it fits exactly into a quadrant
            {
                //NODE.states[NODE.iter_state] = LAB_TexAlloc_FULL;
                goto space_found;
            }
            else // its smaller than an empty quadrant
            {
                size_t new_node = LAB_TexAlloc_NewNode(alloc, node);
                if(new_node == LAB_TexAlloc_Node_NULL) return false;
                NODE.childs[NODE.iter_state] = new_node;
                NODE.states[NODE.iter_state] = LAB_TexAlloc_MIXED;

                node = new_node;
                NODE.empty_cells = space;
                NODE.iter_state = 0u;
                current_size >>= 1;
                space >>= 2;
            }
        }
    }


    space_found:
    {
        // backtrack free space on the way up to the root
        bool set_full = true;

        LAB_ASSERT(node != LAB_TexAlloc_Node_NULL);
        LAB_ASSERT(current_size == 2*side_cells);
        LAB_ASSERT(NODE.empty_cells >= required_space);

        while(node != LAB_TexAlloc_Node_NULL)
        {
            if(set_full) NODE.states[NODE.iter_state] = LAB_TexAlloc_FULL;
            NODE.empty_cells -= required_space;
            size_t parent = NODE.parent;
            if((set_full = (NODE.empty_cells == 0)))
                LAB_TexAlloc_DelNode(alloc, node);
            node = parent;
        }
        if(set_full)
        {
            node = LAB_TexAlloc_NewNode(alloc, node);
            LAB_ASSUME(node != LAB_TexAlloc_Node_NULL);
            // ^ should never fail, because this is the previously deleted node
            NODE.states[0] = LAB_TexAlloc_FULL;
            NODE.empty_cells = alloc->root_size * alloc->root_size * 3u;
            alloc->root_node = node;
            alloc->root_size *= 2u;
        }

        topleft_cell[0] = cur_x;
        topleft_cell[1] = cur_y;
        return true;
    }
    #undef NODE
}















/***** TexAtlas *****/

bool LAB_TexAtlas_Create(LAB_TexAtlas* atlas, size_t cell_size)
{
    LAB_ASSERT(LAB_IsPow2(cell_size));

    // start with a single cell
    atlas->w = atlas->h = cell_size;

    size_t size = 0;
    size_t layer_size = cell_size*cell_size;
    for(; layer_size; layer_size /= 4)
        size += layer_size;
    atlas->capacity = size;
    atlas->cell_size = cell_size;

    atlas->data = LAB_MallocN(atlas->capacity, sizeof(LAB_Color));

    atlas->gl_id = 0;

    return atlas->data != NULL;
}

void LAB_TexAtlas_Destroy(LAB_TexAtlas* atlas)
{
    if(atlas->gl_id) LAB_GL_FREE(glDeleteTextures, 1, &atlas->gl_id);
    LAB_Free(atlas->data);
}



LAB_INLINE bool LAB_TexAtlas_DoubleWidth(LAB_TexAtlas* atlas)
{
    size_t old_cap = atlas->capacity;
    LAB_Color* new_data = LAB_ReallocN(atlas->data, old_cap*2, sizeof(LAB_Color));
    if(!new_data) return false;
    atlas->data = new_data;

    //size_t i_atl = (atlas->h-1)*atlas->w;
    size_t i_atl = atlas->w*atlas->h-atlas->w;
    for(size_t row = atlas->h-1; row > 0; --row)
    {
        LAB_MemCpyColor(atlas->data+i_atl*2, atlas->data+i_atl, atlas->w);
        LAB_MemNoColor(atlas->data+i_atl*2+atlas->w, atlas->w);
        i_atl -= atlas->w;
    }
    LAB_MemNoColor(atlas->data+atlas->w, atlas->w);

    atlas->w *= 2;
    atlas->capacity *= 2;
    return true;
}

LAB_INLINE bool LAB_TexAtlas_DoubleHeight(LAB_TexAtlas* atlas)
{
    size_t old_cap = atlas->capacity;
    LAB_Color* new_data = LAB_ReallocN(atlas->data, old_cap*2, sizeof(LAB_Color));
    if(!new_data) return false;
    atlas->data = new_data;
    LAB_MemNoColor(atlas->data + atlas->w*atlas->h, old_cap);
    atlas->h *= 2;
    atlas->capacity *= 2;
    return true;
}


void LAB_TexAtlas_Clear(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color clear)
{
    size_t i_atl = x + y*atlas->w;

    for(size_t row = 0; row < size; ++row)
    {
        LAB_MemSetColor(atlas->data+i_atl, clear, size);
        i_atl += atlas->w;
    }
}

void LAB_TexAtlas_Draw(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data)
{
    size_t i_src = 0;
    size_t i_atl = x + y*atlas->w;

    for(size_t row = 0; row < size; ++row)
    {
        LAB_MemCpyColor(atlas->data+i_atl, data+i_src, size);
        i_src += size;
        i_atl += atlas->w;
    }
}

void LAB_TexAtlas_DrawBlit(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data, LAB_Color black_tint, LAB_Color white_tint)
{
    size_t i_src = 0;
    size_t i_atl = x + y*atlas->w;

    for(size_t row = 0; row < size; ++row)
    {
        LAB_MemBltColor2(atlas->data+i_atl, data+i_src, black_tint, white_tint, size);
        i_src += size;
        i_atl += atlas->w;
    }
}

bool LAB_TexAtlas_ClearAlloc(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color clear)
{
    while(x+size > atlas->w)
        if(!LAB_TexAtlas_DoubleWidth(atlas)) return false;

    while(y+size > atlas->h)
        if(!LAB_TexAtlas_DoubleHeight(atlas)) return false;

    LAB_TexAtlas_Clear(atlas, x, y, size, clear);

    return true;
}

bool LAB_TexAtlas_DrawAlloc(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data)
{
    while(x+size > atlas->w)
        if(!LAB_TexAtlas_DoubleWidth(atlas)) return false;

    while(y+size > atlas->h)
        if(!LAB_TexAtlas_DoubleHeight(atlas)) return false;

    LAB_TexAtlas_Draw(atlas, x, y, size, data);

    return true;
}


void LAB_TexAtlas_MakeMipmap(LAB_TexAtlas* atlas)
{
    size_t w = atlas->w, h = atlas->h;
    LAB_Color* data = atlas->data;

    size_t csz = atlas->cell_size/2;
    for(; csz; csz /= 2)
    {
        if(csz) LAB_Fix0Alpha(w, h, data); // at prev layer including original!

        LAB_MakeMipmap2D(w/2, h/2, data, data+w*h);

        data = data+w*h;
        w /= 2; h /= 2;
    }
}

bool LAB_TexAtlas_Upload2GL(LAB_TexAtlas* atlas)
{
    LAB_GL_REQUIRE(GL_TEXTURE_2D);

    if(!atlas->gl_id)
    {
        LAB_GL_ALLOC(glGenTextures, 1, &atlas->gl_id);

        //if(!atlas->gl_id) return false;
    }

    glBindTexture(GL_TEXTURE_2D, atlas->gl_id);

    if(atlas->cell_size > 1) // Mipmaps
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        const size_t num_mipmaps = LAB_Log2OfPow2(atlas->cell_size);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, num_mipmaps);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas->w, atlas->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas->data);
        LAB_ImageSave(atlas->w, atlas->h, atlas->data, "dbg_terrain_0.png");

        // upload mipmaplevels
        {
            size_t w = atlas->w, h = atlas->h;
            //size_t layer_size = atlas->cell_size/2;
            LAB_Color* data = atlas->data;

            for(size_t i = 1; i <= num_mipmaps; ++i)
            {
                data += w*h;
                w /= 2; h /= 2;

                glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                LAB_ImageSave_Fmt(w, h, data, "dbg_terrain_%i.png", i);
            }
        }

    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas->w, atlas->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas->data);
    }

    LAB_GL_CHECK();
    return true;
}

void LAB_TexAtlas_LoadTexMatrix(LAB_TexAtlas* atlas)
{
    LAB_GL_REQUIRE(GL_TEXTURE_2D);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    const float cell_size_f = atlas->cell_size;
    glScalef((float)cell_size_f / (float)atlas->w, (float)cell_size_f / (float)atlas->h, 1);

    LAB_GL_CHECK();
}




#include <stdio.h>
void LAB_TestTextureAtlas()
{
    /*static const char* textures[] =
    {
        "assets/blocks/stone.png",
        "assets/blocks/cobble.png",
        "assets/blocks/bricks.png",
        "assets/blocks/smooth_stone.png",
        "assets/blocks/oak_log.png",
        "assets/blocks/leaves.png",

        // tst
        //"assets/terrain.png",
        "assets/blocks/leaves.png",
        "assets/blocks/metal.png",
        //"assets/gui.png",
        NULL
    };*/

    static const char* textures[] =
    {
        "assets/blocks/stone.png",
        "assets/blocks/cobble.png",
        "assets/blocks/smooth_stone.png",
        "assets/blocks/bricks.png",
        "assets/blocks/oak_log.png",
        "assets/blocks/planks.png",
        "assets/blocks/grass.png",
        "assets/blocks/sand.png",
        "assets/blocks/cold_light.png",
        "assets/blocks/warm_light.png",
        "assets/blocks/torch.png",
        "assets/blocks/glass.png",
        "assets/blocks/leaves.png",
        "assets/blocks/tall_grass.png",
        "assets/blocks/crystal.png",
        "assets/blocks/taller_grass.png",
    };

    LAB_DBG_PRINTF("LAB_TestTextureAtlas\n");

    LAB_TexAlloc alloc;
    LAB_ASSERT_OR_ABORT(LAB_TexAlloc_Create(&alloc));

    LAB_TexAtlas atlas;
    LAB_ASSERT_OR_ABORT(LAB_TexAtlas_Create(&atlas, 32));

    //for(size_t i = 0; textures[i]; ++i)
    for(size_t i = 0, k = 0; k < 1024; ++k, i=rand()%(sizeof(textures)/sizeof(*textures)-1))
    {
        SDL_Surface* surf = LAB_ImageLoad(textures[i]);
        LAB_ASSERT_OR_ABORT(surf);
        LAB_ASSERT_OR_ABORT(surf->w == surf->h);
        LAB_ASSERT_OR_ABORT(LAB_IsPow2(surf->w));

        //#define CELL_SIZE 32
        #define CELL_SIZE 1

        size_t pos[2];
        LAB_ASSERT_OR_ABORT(LAB_TexAlloc_Add(&alloc, surf->w/CELL_SIZE, pos));
        LAB_DBG_PRINTF("%s at %zu, %zu\n", textures[i], pos[0], pos[1]);
        //LAB_MemSetColor((LAB_Color*)surf->pixels, LAB_RGBX(0000ff), i);
        LAB_ASSERT_OR_ABORT(LAB_TexAtlas_DrawAlloc(&atlas, pos[0]*CELL_SIZE, pos[1]*CELL_SIZE, surf->w, (LAB_Color*)surf->pixels));

        LAB_SDL_FREE(SDL_FreeSurface, &surf);
    }

    LAB_ImageSave(atlas.w, atlas.h, atlas.data, "dbg_TestTextureAtlas.png");

    LAB_TexAtlas_Destroy(&atlas);
    LAB_TexAlloc_Destroy(&alloc);

}
