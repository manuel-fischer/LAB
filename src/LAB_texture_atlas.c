#include "LAB_texture_atlas.h"
#include "LAB_memory.h"
#include "LAB_arith.h"

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
