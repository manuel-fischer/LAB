#include "LAB_texture_atlas.h"
#include "LAB_memory.h"
#include "LAB_arith.h"
#include "LAB_image.h"

#include "LAB_sdl.h" // LAB_SDL_FREE -- TODO

#include <stdio.h> // TODO remove

#if 0
///// LAB_TexClt /////

bool LAB_TexClt_Create(LAB_TexClt* texclt, size_t cell_size)
{
    LAB_ARRAY_CREATE(texclt->texture_sizes);
    texclt->cell_size = cell_size;
    return true;
}

void LAB_TexClt_Destroy(LAB_TexClt* texclt)
{
    LAB_ARRAY_DESTROY(texclt->texture_sizes);
}

LAB_TexID LAB_TexClt_AddTexture(LAB_TexClt* texclt, size_t width, size_t height)
{
    if(!LAB_IsPow2(width) || width != height || width < texclt->cell_size)
        return LAB_TEX_ID_INVALID;

    size_t* tex_sizes;
    LAB_ARRAY_APPEND(texclt->texture_sizes, 1, tex_sizes);
    if(tex_sizes)
    {
        tex_sizes[0] = width;
        return texclt->texture_sizes.size-1;
    }
    else
    {
        return LAB_TEX_ID_INVALID;
    }
}


///// LAB_TexBld /////

bool LAB_TexBld_Create(LAB_TexBld* texbld, LAB_TexClt* texclt)
{
    texbld->texclt = texclt;

    size_t* arr = LAB_MallocN(texclt->texture_sizes.size, 4*sizeof(size_t));
    if(!arr) return false;
    texbld->tex_order = arr;
    texbld->tex_order_inv = arr+texclt->texture_sizes.size;
    texbld->tex_pos = (LAB_TexPos*)arr+texclt->texture_sizes.size*2;
    return true;
}

void LAB_TexBld_Sort(LAB_TexBld* texbld)
{
    LAB_TexClt* texclt = texbld->texclt;
    size_t  size = texclt->texture_sizes.size;
    size_t* sizes = texclt->texture_sizes.data;

    size_t max_cell_size = 0;
    for(size_t i = 0; i < size; ++i)
        LAB_MAX_EQ(max_cell_size, sizes[i]);

    size_t next_index = 0;
    for(size_t cell_size = max_cell_size; cell_size >= texclt->cell_size; cell_size >>= 1)
    {
        for(size_t i = 0; i < size; ++i)
        {
            if(sizes[i] == cell_size)
            {
                texbld->tex_order[next_index] = i;
                ++next_index;
            }
        }
    }
    LAB_ASSUME(next_index == size);

    // the fun part
    // invert the bijective function
    for(size_t i = 0; i < size; ++i)
    {
        texbld->tex_order_inv[texbld->tex_order[i]] = i;
    }
}

bool LAB_TexBld_Arrange(LAB_TexBld* texbld)
{
    LAB_TexClt* texclt = texbld->texclt;
    size_t  size = texclt->texture_sizes.size;
    size_t* sizes = texclt->texture_sizes.data;
    if(!size) return true;

    size_t max_cell_size = sizes[texbld->tex_order[0]];
    size_t max_cell_ratio = LAB_MAX(8, max_cell_size/texclt->cell_size);

    size_t cell_cols = max_cell_ratio, cell_rows = max_cell_ratio;

    size_t   occupied_cap = cell_cols*cell_rows/8;
    uint8_t* occupied = LAB_Calloc(occupied_cap, 1);
    if(!occupied) return false;

    for(size_t i = 0; i < size; ++i)
    {
        size_t tex_size = sizes[texbld->tex_order[i]];
        size_t tex_cell_ratio = tex_size/texclt->cell_size;

        size_t x, y;
        size_t yi;
        while(true)
        {
            for(y = 0, yi = 0; y < cell_rows; ++y, yi += cell_cols)
            {
                uint8_t mask = 1u;
                for(x = 0; x < cell_cols; ++x, mask = LAB_XADD(mask<<1, mask>>7))
                {
                    size_t idx = LAB_XADD(yi, x);
                    // because of the order the textures are arranged, larger
                    // textures are aligned at coordinates that are multiples
                    // of their size, so only the left upper most cell needs
                    // to be tested
                    //if(occupied[idx/8] & 1<<(idx%8)) continue;
                    if(occupied[idx/8] & mask) continue;

                    goto found;
                }
            }
            // not found
            // double space

            {
                uint8_t* new_occupied = LAB_Realloc(occupied, occupied_cap<<1);
                if(!new_occupied)
                {
                    LAB_Free(occupied);
                    return false;
                }
                occupied = new_occupied;
            }

            if(cell_rows <= cell_cols)
            {
                // expand vertically
                memset(occupied+occupied_cap, 0, occupied_cap);

                occupied_cap <<= 1;
                cell_rows <<= 1;
            }
            else
            {
                // expand horizontally

                // for this go from the bottom rows up to the top
                // and copy the rows
                // fill new columns with zeros

                // Move rows except the first one
                for(size_t c_y = 0, c_yi = 0; c_y < cell_rows; ++c_y, c_yi += cell_cols)
                {
                    size_t idx_old =   c_yi;
                    size_t idx_new = 2*c_yi;
                    memcpy(occupied+idx_new/8, occupied+idx_old/8, cell_cols/8);
                    memset(occupied+idx_new/8+cell_cols/8, 0, cell_cols/8);
                }
                // clear new rows in the first row
                memset(occupied+cell_cols/8, 0, cell_cols/8);

                occupied_cap <<= 1;
                cell_cols <<= 1;
            }
        }

        found:;
        // set information
        size_t index = texbld->tex_order[i];
        texbld->tex_pos[index].x = x*texclt->cell_size;
        texbld->tex_pos[index].y = y*texclt->cell_size;

        // fill bits in occupied

        if(tex_cell_ratio < 8)
        {
            uint8_t bit_mask = ((1u << tex_cell_ratio) - 1u) << (x%8);

            for(size_t rc_y = 0, c_yi = yi; rc_y < tex_cell_ratio; ++rc_y, c_yi += cell_cols)
            {
                size_t idx = c_yi + x;
                LAB_XADD_EQ(occupied[idx/8], bit_mask);
            }
        }
        else
        {
            for(size_t rc_y = 0, c_yi = yi; rc_y < tex_cell_ratio; ++rc_y, c_yi += cell_cols)
            for(size_t rc_x = 0; rc_x < tex_cell_ratio; rc_x += 8)
            {
                size_t idx = LAB_XADD(c_yi, rc_x + x);
                LAB_ASSUME(occupied[idx/8] == 0);
                occupied[idx/8] = 0xffu;
            }
        }
    }

    LAB_Free(occupied);

    texbld->at_width = cell_cols * texclt->cell_size;
    texbld->at_height = cell_rows * texclt->cell_size;

    return true;
}

void LAB_TexBld_Destroy(LAB_TexBld* texbld)
{
    LAB_Free(texbld->tex_order);
}
#endif





bool LAB_TexAlloc_Create(LAB_TexAlloc* alloc)
{
    LAB_ARRAY_CREATE(alloc->nodes);

    /*LAB_TexAlloc_Node* node;
    LAB_ARRAY_APPEND(alloc->nodes, 1, node);
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
    LAB_ARRAY_DESTROY(alloc->nodes);

}

LAB_STATIC size_t LAB_TexAlloc_NewNode_Alloc(LAB_TexAlloc* alloc)
{
    size_t node;
    #define NODE (alloc->nodes.data[node])
    if(alloc->next_unoccupied != LAB_TexAlloc_Node_NULL)
    {
        node = alloc->next_unoccupied;
        alloc->next_unoccupied = NODE.next_unoccupied;
    }
    else
    {
        // Allocate new node
        LAB_TexAlloc_Node* node_p;
        LAB_ARRAY_APPEND(alloc->nodes, 1, node_p);
        if(!node_p) return LAB_TexAlloc_Node_NULL;
        node = node_p - alloc->nodes.data;
    }
    #undef NODE
    return node;
}

// creates and clears node, sets states[] to empty
LAB_INLINE size_t LAB_TexAlloc_NewNode(LAB_TexAlloc* alloc, size_t parent)
{
    size_t node = LAB_TexAlloc_NewNode_Alloc(alloc);
    #define NODE (alloc->nodes.data[node])
    if(node == LAB_TexAlloc_Node_NULL) return node;

    // memset(&NODE, 0xEE, sizeof(NODE)); // debug

    memset(NODE.states, LAB_TexAlloc_EMPTY, sizeof(NODE.states));
    NODE.parent = parent;
    #undef NODE
    return node;
}

LAB_STATIC void LAB_TexAlloc_DelNode(LAB_TexAlloc* alloc, size_t node)
{
    #define NODE (alloc->nodes.data[node])
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
    #define NODE (alloc->nodes.data[node])
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

bool LAB_TexAtlas_Create(LAB_TexAtlas* atlas)
{
    atlas->width = atlas->height = 1;
    atlas->data = LAB_MallocN(atlas->width*atlas->height, sizeof(LAB_Color));
    return atlas->data != NULL;
}

void LAB_TexAtlas_Destroy(LAB_TexAtlas* atlas)
{
    LAB_Free(atlas->data);
}



LAB_INLINE bool LAB_TexAtlas_DoubleWidth(LAB_TexAtlas* atlas)
{
    size_t old_cap = atlas->width*atlas->height;
    LAB_Color* new_data = LAB_ReallocN(atlas->data, old_cap*2, sizeof(LAB_Color));
    if(!new_data) return false;
    atlas->data = new_data;

    //size_t i_atl = (atlas->height-1)*atlas->width;
    size_t i_atl = old_cap-atlas->width;
    for(size_t row = atlas->height-1; row > 0; --row)
    {
        LAB_MemCpyColor(atlas->data+i_atl*2, atlas->data+i_atl, atlas->width);
        LAB_MemSetColor(atlas->data+i_atl*2+atlas->width, LAB_RGBX(FF0000), atlas->width);
        i_atl -= atlas->width;
    }
    LAB_MemSetColor(atlas->data+atlas->width, LAB_RGBX(FF0000), atlas->width);

    atlas->width *= 2;
    return true;
}

LAB_INLINE bool LAB_TexAtlas_DoubleHeight(LAB_TexAtlas* atlas)
{
    size_t old_cap = atlas->width*atlas->height;
    LAB_Color* new_data = LAB_ReallocN(atlas->data, old_cap*2, sizeof(LAB_Color));
    if(!new_data) return false;
    atlas->data = new_data;
    LAB_MemSetColor(atlas->data + old_cap, LAB_RGBX(FF0000), old_cap);
    atlas->height *= 2;
    return true;
}


void LAB_TexAtlas_Draw(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data)
{
    size_t i_src = 0;
    size_t i_atl = x + y*atlas->width;

    for(size_t row = 0; row < size; ++row)
    {
        LAB_MemCpyColor(atlas->data+i_atl, data+i_src, size);
        i_src += size;
        i_atl += atlas->width;
    }
}

bool LAB_TexAtlas_DrawAlloc(LAB_TexAtlas* atlas, size_t x, size_t y, size_t size, LAB_Color* data)
{
    while(x+size > atlas->width)
        if(!LAB_TexAtlas_DoubleWidth(atlas)) return false;

    while(y+size > atlas->height)
        if(!LAB_TexAtlas_DoubleHeight(atlas)) return false;

    LAB_TexAtlas_Draw(atlas, x, y, size, data);

    return true;
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

    printf("LAB_TestTextureAtlas\n");

    LAB_TexAlloc alloc;
    LAB_ASSERT(LAB_TexAlloc_Create(&alloc));

    LAB_TexAtlas atlas;
    LAB_ASSERT(LAB_TexAtlas_Create(&atlas));


    //for(size_t i = 0; textures[i]; ++i)
    for(size_t i = 0, k = 0; k < 1024; ++k, i=rand()%(sizeof(textures)/sizeof(*textures)-1))
    {
        SDL_Surface* surf = LAB_ImageLoad(textures[i]);
        LAB_ASSERT(surf);
        LAB_ASSERT(surf->w == surf->h);
        LAB_ASSERT(LAB_IsPow2(surf->w));

        //#define CELL_SIZE 32
        #define CELL_SIZE 1

        size_t pos[2];
        LAB_ASSERT(LAB_TexAlloc_Add(&alloc, surf->w/CELL_SIZE, pos));
        printf("%s at %i, %i\n", textures[i], pos[0], pos[1]);
        //LAB_MemSetColor((LAB_Color*)surf->pixels, LAB_RGBX(0000ff), i);
        LAB_ASSERT(LAB_TexAtlas_DrawAlloc(&atlas, pos[0]*CELL_SIZE, pos[1]*CELL_SIZE, surf->w, (LAB_Color*)surf->pixels));

        LAB_SDL_FREE(SDL_FreeSurface, &surf);
    }

    LAB_ImageSaveData("dbg_TestTextureAtlas.png", atlas.width, atlas.height, atlas.data);

    LAB_TexAtlas_Destroy(&atlas);
    LAB_TexAlloc_Destroy(&alloc);

}
