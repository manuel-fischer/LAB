#include "LAB_world_light.h"
#include "LAB_debug.h"
#include "LAB_chunk_pseudo.h"
// TODO: use pseudo chunks instead of NULL chunks -> no branching required
//       - build chunk neighborhood with these pseudo chunks
// TODO: reduce range of searching for possibly changed block positions
//       - use crammed chunk pos set, stored in the chunks

#include <stdio.h> // DBG

// minimum light level for completely unlit block with that dia
#define LAB_DARK_LIGHT(dia) ((((dia)>>4) & 0x0f0f0fu) | 0xff000000u)
#define LAB_MIN_LIGHT(dia) ((((dia)>>6) & 0x030303u) | 0xff000000u)
//#define LAB_DARK_LIGHT(dia) (dia)
                    //nlum = nlum - (nlum>>2 & 0x3f3f3f);
#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((lum)>>3 & 0x1f1f1f)-((lum)>>4 & 0x0f0f0f))
//#define LAB_LIGHT_FALL_OFF(lum) (LAB_SubColor((lum), LAB_RGBA(16, 18, 20, 0)))


// heuristic lighting algorithm
// run first time when the chunk has been created
LAB_ALWAYS_INLINE
LAB_STATIC void LAB_PrepareLight(LAB_Chunk* chunk, LAB_Chunk* chunk_above, LAB_Color default_color)
{
    for(int z = 0; z < 16; ++z)
    for(int x = 0; x < 16; ++x)
    {
        int xz_off = x | z<<8;
        LAB_Color l = chunk_above ? chunk_above->light[xz_off] : default_color;
        for(int y = 15; y >= 0; --y)
        {
            int xyz_off = xz_off | y<<4;
            LAB_Color dia = chunk->blocks[xyz_off]->dia;
            l = LAB_MulColor(l, dia);
            l = LAB_MaxColor(l, LAB_DARK_LIGHT(dia));
            chunk->light[xyz_off] = l;
        }
    }
}

LAB_ALWAYS_INLINE
LAB_STATIC LAB_Color LAB_CalcLight(LAB_World* world, LAB_Chunk*const chunks[27], int x, int y, int z, LAB_Color default_color, LAB_Color default_color_above)
{
    LAB_ASSUME(x >= -16+1 && x < 32-1);
    LAB_ASSUME(y >= -16+1 && y < 32-1);
    LAB_ASSUME(z >= -16+1 && z < 32-1);

    // "branchless"
    LAB_Color lum;
    //LAB_Chunk* cnk = chunks[1+3+9];
    //int off = LAB_CHUNK_OFFSET(x, y, z);


    //LAB_Color dia = cnk->blocks[off]->dia;
    LAB_Block* b_ctr = LAB_GetNeighborhoodBlock(chunks, x, y, z);
    LAB_Color dia = b_ctr->dia;

    //lum = LAB_RGB(16, 16, 16);
    lum = LAB_DARK_LIGHT(dia);
    //lum = LAB_MaxColor(lum, cnk->blocks[off]->lum);
    //lum = LAB_MaxColor(lum, LAB_GetNeighborhoodLight(chunks, x, y, z, default_color));
    lum = LAB_MaxColor(lum, b_ctr->lum);
    LAB_UNROLL(6)
    for(int i = 0; i < 6; ++i)
    {
        //const int* o = LAB_offset[i];
        int nlum, nlum1, nlum2;

        LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+LAB_OX(i), y+LAB_OY(i), z+LAB_OZ(i));

        nlum1 = block->lum;
        nlum2 = LAB_GetNeighborhoodLight(chunks, x+LAB_OX(i), y+LAB_OY(i), z+LAB_OZ(i), i, i==3?default_color_above:default_color);
        //if(i!=3 || (nlum2&0xffffff) != 0xffffff)
        //    nlum2 = nlum2 - (nlum2>>2 & 0x3f3f3f);

        // simplified
        //uint32_t mask = -(i!=3); // alpha needs to be 0
        uint32_t is_white_24 = ((nlum2&0xffffffu)+1)&0x01000000u;
        uint32_t mask = is_white_24-(unsigned)((!!is_white_24) & (i==3)); // alpha needs to be 0
        //nlum2 = nlum2 - ((nlum2>>2 & 0x3f3f3f)&~mask); // don't care about alpha here
        nlum2 = nlum2 ^ ((nlum2^LAB_LIGHT_FALL_OFF(nlum2)) &~ mask);

        nlum = LAB_MaxColor(nlum1, nlum2);
        lum = LAB_MaxColor(lum, LAB_MulColor_Fast(nlum, dia));
    }

    return lum;
}

// neighborhood coordinates compressed to 3*6 bits
// each from -16 to 31
// each from 0-16 to 47-16 -> 6 bits each
#define LAB_NBH(x, y, z) (((x)+16)|((y)+16)<<6|((z)+16)<<12)
#define LAB_NBH_X(xyz) (((xyz)    &077)-16)
#define LAB_NBH_Y(xyz) (((xyz)>> 6&077)-16)
#define LAB_NBH_Z(xyz) (((xyz)>>12    )-16)

/**
 *  Check for wrong light values in the center chunk and update values
 *  that are to be updated. These positions are pushed into a queue
 *
 *  Return format: 0b!?SNUDEW in binary
 *  !: Redo calculation
 *  ?: Any light changed
 *  S, N, U, D, E, W: Neighboring chunk needs to be updated
 */
LAB_ALWAYS_INLINE
LAB_STATIC int LAB_UpdateLight_First(LAB_World* world, LAB_Chunk*const chunks[27],
                                     LAB_Color default_color, LAB_Color default_color_above,
                                     size_t queue_cap, int* queue, size_t* queue_count,
                                     LAB_CrammedChunkPosSet dirty_blocks)
{
    int changed = 0;

    // TODO: check positions at the border x/y/z==0/15, if the face changed

    // x, y, z in [-1, 16]
    int x, y, z;
    LAB_CCPS_EACH_NEAR_POS(dirty_blocks, x, y, z,
    {
        int off;
        LAB_Chunk* cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &off);
        if(cnk)
        {
            LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, default_color, default_color_above);

            if(cnk->light[off] != lum)
            {
                cnk->light[off] = lum;
                if(x==0)  changed |=  1;
                if(x==15) changed |=  2;
                if(y==0)  changed |=  4;
                if(y==15) changed |=  8;
                if(z==0)  changed |= 16;
                if(z==15) changed |= 32;
                changed |= 64;

                if(*queue_count != queue_cap)
                    queue[(*queue_count)++] = LAB_NBH(x,y,z);
                else
                {
                    changed |= 128;
                    //printf("QUEUE FULL\n");
                }
            }
        }
    });

    return changed;
}


// TODO return bitset of all 27 chunks if they're changed
LAB_HOT                                                // TODO: |--------------------| not used
int LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27], int cx, int cy, int cz)
{
    //static int i = 0;
    //printf("LAB_TickLight %i @ %i %i %i\n", i++, cx, cy, cz);
    LAB_Chunk* ctr_cnk = chunks[1+3+9];
    if(!ctr_cnk) return 0;

    LAB_Color default_color_above = cy <  -2 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);
    LAB_Color default_color = cy <  0 ? LAB_RGB(16, 16, 16) : LAB_RGB(255, 255, 255);

    if(!ctr_cnk->light_generated)
    {
        LAB_PrepareLight(ctr_cnk, chunks[1+2*3+9], default_color_above);
        ctr_cnk->light_generated = 1;
        ctr_cnk->dirty_blocks = ~0; // check all blocks again
        //return ~0; // TODO: multiple updates to the view
        return 128;
    }
    //return 0; // DBG

    #define LAB_LIGHT_QUEUE_SIZE (1<<14)
    #define LAB_LIGHT_QUEUE_MASK (LAB_LIGHT_QUEUE_SIZE-1)
    static int queue[LAB_LIGHT_QUEUE_SIZE]; // TODO, FIXME: thread local, dynamic?
    size_t queue_first = 0;
    size_t queue_count = 0;

    LAB_CrammedChunkPosSet dirty_blocks = chunks[1+3+9]->dirty_blocks;
    chunks[1+3+9]->dirty_blocks = 0;

    // for simplicity the queue starts at 0, this is not passed to the function
    // because no elements are taken out of the queue
    int faces_changed = LAB_UpdateLight_First(world, chunks,
                                              default_color, default_color_above,
                                              LAB_LIGHT_QUEUE_SIZE, queue, &queue_count,
                                              dirty_blocks);
    if(!faces_changed) return 0;
    //faces_changed &= 63; // remove change bit
    faces_changed=0; // TODO:

    while(queue_count)
    {
        int xyz = queue[queue_first];
        int qx = LAB_NBH_X(xyz);
        int qy = LAB_NBH_Y(xyz);
        int qz = LAB_NBH_Z(xyz);
        queue_first++;
        queue_first&=LAB_LIGHT_QUEUE_MASK;
        queue_count--;

        LAB_UNROLL(6)
        for(int face = 0; face < 6; ++face)
        {
            int x = qx+LAB_OX(face);
            int y = qy+LAB_OY(face);
            int z = qz+LAB_OZ(face);
            #if 0
            if(x==-16||x==31) goto next_face;
            if(y==-16||y==31) goto next_face;
            if(z==-16||z==31) goto next_face;
            #else
            // only one check:
            switch(face)
            {
                /*case 0: if(x==-16) { faces_changed |=  1; goto next_face; } else break;
                case 1: if(x== 31) { faces_changed |=  2; goto next_face; } else break;
                case 2: if(y==-16) { faces_changed |=  4; goto next_face; } else break;
                case 3: if(y== 31) { faces_changed |=  8; goto next_face; } else break;
                case 4: if(z==-16) { faces_changed |= 16; goto next_face; } else break;
                case 5: if(z== 31) { faces_changed |= 32; goto next_face; } else break;*/
                case 0: if(x==-16) { goto needs_relight; } else break;
                case 1: if(x== 31) { goto needs_relight; } else break;
                case 2: if(y==-16) { goto needs_relight; } else break;
                case 3: if(y== 31) { goto needs_relight; } else break;
                case 4: if(z==-16) { goto needs_relight; } else break;
                case 5: if(z== 31) { goto needs_relight; } else break;
                default: break;

                needs_relight:
                {
                    int block_index;
                    LAB_Chunk* cnk;
                    cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &block_index);
                    if(cnk)
                    {
                        //cnk->needs_relight = 1;
                        cnk->dirty |= LAB_CHUNK_UPDATE_LIGHT;
                    }
                    goto next_face;
                }
            }
            #endif


            int block_index;
            LAB_Chunk* cnk;
            cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &block_index);
            if(cnk)
            {
                LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, default_color, default_color_above);

                if(cnk->light[block_index] != lum)
                {
                    cnk->light[block_index] = lum;
                    // TODO REMOVE
                    if(x<=0)  faces_changed |=  1;
                    if(x>=15) faces_changed |=  2;
                    if(y<=0)  faces_changed |=  4;
                    if(y>=15) faces_changed |=  8;
                    if(z<=0)  faces_changed |= 16;
                    if(z>=15) faces_changed |= 32;
                    faces_changed |= 64;
                    // TODO REMOVE END

                    //cnk->dirty_blocks = LAB_CCPS_AddPos(cnk->dirty_blocks, x&15, y&15, z&15);
                    cnk->relit_blocks |= LAB_CCPS_Pos(x&15, y&15, z&15);

                    if(queue_count != LAB_LIGHT_QUEUE_SIZE)
                    {
                        queue[(queue_first+(queue_count++))&LAB_LIGHT_QUEUE_MASK] = LAB_NBH(x, y, z);
                    }
                    else
                    {
                        faces_changed |= 128;
                        //printf("FULL QUEUE\n");
                    }
                }
            }

            next_face:;
        }
    }

    return faces_changed;
}
