#include "LAB_texture_atlas.h"
#include "LAB_memory.h"
#include "LAB_arith.h"

#include "LAB_sdl.h" // LAB_SDL_FREE -- TODO
#include "LAB_gl.h"

bool LAB_TexAlloc_Create(LAB_TexAlloc* alloc)
{
    LAB_ARRAY_CREATE_EMPTY(LAB_TexAlloc_Nodes(alloc));

    /*LAB_TexAlloc_Node* node;
    LAB_ARRAY_APPEND(LAB_TexAlloc_Nodes(alloc), 1, &node);
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
        LAB_ARRAY_APPEND(LAB_TexAlloc_Nodes(alloc), 1, &node_p);
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


LAB_TexAlloc_Result LAB_TexAlloc_Add(LAB_TexAlloc* alloc, size_t side_cells)
{
    LAB_TexAlloc_Result result;

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
            if(node == LAB_TexAlloc_Node_NULL) return (result.success=false, result);
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
                if(new_node == LAB_TexAlloc_Node_NULL) return (result.success=false, result);
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
                if(new_node == LAB_TexAlloc_Node_NULL) return (result.success=false, result);
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

        result.topleft_cell.x = cur_x;
        result.topleft_cell.y = cur_y;
        return (result.success=true, result);
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

    atlas->tex.id = 0;

    return atlas->data != NULL;
}

void LAB_TexAtlas_Destroy(LAB_TexAtlas* atlas)
{
    LAB_GL_OBJ_FREE(glDeleteTextures, &atlas->tex);
    LAB_Free(atlas->data);
}


LAB_ImageView LAB_TexAtlas_AsImageView(LAB_TexAtlas* atlas)
{
    return LAB_ImageView_Create(atlas->w, atlas->h, atlas->data);
}

LAB_ImageView LAB_TexAtlas_ClipImageView(LAB_TexAtlas* atlas, LAB_Box2Z rect_cells)
{
    return LAB_ImageView_Clip(LAB_TexAtlas_AsImageView(atlas), LAB_Box2Z_Mul(atlas->cell_size, rect_cells));
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


bool LAB_TexAtlas_Alloc(LAB_TexAtlas* atlas, LAB_Box2Z rect_cells)
{
    LAB_Box2Z pixel_rect = LAB_Box2Z_Mul(atlas->cell_size, rect_cells);

    while(pixel_rect.b.x > atlas->w)
        if(!LAB_TexAtlas_DoubleWidth(atlas)) return false;

    while(pixel_rect.b.y > atlas->h)
        if(!LAB_TexAtlas_DoubleHeight(atlas)) return false;

    return true;
}


void LAB_TexAtlas_MakeMipmap(LAB_TexAtlas* atlas)
{
    size_t w = atlas->w, h = atlas->h;
    LAB_Color* data = atlas->data;

    size_t csz = atlas->cell_size/2;
    for(; csz; csz /= 2)
    {
        if(csz) LAB_Fix0Alpha(w, h, data, w); // at prev layer including original!

        LAB_MakeMipmap2D(w/2, h/2, data, data+w*h);

        data = data+w*h;
        w /= 2; h /= 2;
    }
}

bool LAB_TexAtlas_Upload2GL(LAB_TexAtlas* atlas)
{
    LAB_GL_DBG_CHECK();

    if(atlas->tex.id)
        LAB_GL_OBJ_FREE(glDeleteTextures, &atlas->tex);

    LAB_GL_OBJ_ALLOC_TARGET(glCreateTextures, GL_TEXTURE_2D, &atlas->tex);
    LAB_GL_DBG_CHECK();

    glTextureParameteri(atlas->tex.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTextureParameteri(atlas->tex.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    LAB_GL_DBG_CHECK();

    const size_t levels = LAB_TexAtlas_Levels(atlas);
    glTextureStorage2D(atlas->tex.id, levels, GL_RGBA8, atlas->w, atlas->h);

    // upload mipmaplevels
    size_t w = atlas->w, h = atlas->h;
    //size_t layer_size = atlas->cell_size/2;
    LAB_Color* data = atlas->data;

    for(size_t i = 0; i < levels; ++i)
    {
        glTextureSubImage2D(atlas->tex.id, i, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        LAB_GL_DBG_CHECK();

        data += w*h;
        w /= 2; h /= 2;
    }

    LAB_GL_CHECK();
    return true;
}

#ifndef NDEBUG
bool LAB_TexAtlas_DbgDumpToFile(LAB_TexAtlas* atlas, const char* prefix)
{
    const size_t levels = LAB_TexAtlas_Levels(atlas);

    // upload mipmaplevels
    size_t w = atlas->w, h = atlas->h;
    //size_t layer_size = atlas->cell_size/2;
    LAB_Color* data = atlas->data;

    for(size_t i = 0; i < levels; ++i)
    {
        LAB_ImageSave_Fmt(w, h, data, "%s_%i.png", prefix, i);

        data += w*h;
        w /= 2; h /= 2;
    }
    return true;
}
#endif




LAB_Vec2F LAB_TexAtlas_ScaleFactor(LAB_TexAtlas* atlas)
{
    const float cell_size_f = atlas->cell_size;
    return (LAB_Vec2F) {
        cell_size_f / (float)atlas->w,
        cell_size_f / (float)atlas->h,
    };
}



