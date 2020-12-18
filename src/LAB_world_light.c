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
/*LAB_STATIC void LAB_PrepareLight(LAB_Chunk* chunk, LAB_Color default_color)
{
    for(int z = 0; z < 16; ++z)
    for(int x = 0; x < 16; ++x)
    {
        int xz_off = x | z<<8;
        LAB_Color l = default_color;
        for(int y = 15; y >= 0; --y)
        {
            int xyz_off = xz_off | y<<4;
            LAB_Color dia = chunk->blocks[xyz_off]->dia;
            l = LAB_MulColor(l, dia);
            l = LAB_MaxColor(l, LAB_DARK_LIGHT(dia));
            chunk->light[xyz_off] = l;
        }
    }
}*/

#if LAB_DIRECTIONAL_LIGHT == 0
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

    //return ~0;
    #if 0
    LAB_Color lum;
    //lum = LAB_RGB(16, 16, 16);
    //lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
    LAB_Chunk* cnk = chunks[1+3+9];
    int off = LAB_CHUNK_OFFSET(x, y, z);
    if(!(cnk->blocks[off]->flags & LAB_BLOCK_OPAQUE))
    {
        LAB_Color dia = cnk->blocks[off]->dia;
        lum = LAB_RGB(16, 16, 16);
        for(int i = 0; i < 6; ++i)
        {
            const int* o = LAB_offset[i];
            int nlum;

            LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+o[0], y+o[1], z+o[2]);
            if(block->flags & LAB_BLOCK_EMISSIVE)
                nlum = block->lum;
            else
            {
                nlum = LAB_GetNeighborhoodLight(chunks, x+o[0], y+o[1], z+o[2], i==3?default_color_above:default_color);
                if(i!=3 || (nlum&0xffffff) != 0xffffff)
                    nlum = LAB_LIGHT_FALL_OFF(nlum);
            }
            lum = LAB_MaxColor(lum, LAB_MulColor_Fast(nlum, dia));
        }
    }
    else
    {
        lum = LAB_GetNeighborhoodBlock(chunks, x,y,z)->lum;
    }
    return lum;
    #else
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
    #endif
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

    #if 0
    #if 1
    // check borders (this is needed if light blocks are placed at the border)
    LAB_UNROLL(6)
    for(int face = 0; face<6; ++face)
    {
        //int c_offs[6] = { 0+3+9, 2*1+3+9, 1+0+9, 1+2*3+9, 1+3+0, 1+3+2*9 };
        //int c_off = c_offs[face]
        // 27 -> 5 bits for each
        int c_off = (((0+3+9)     | (2*1+3+9)<< 5 |
                      (1+0+9)<<10 | (1+2*3+9)<<15 |
                      (1+3+0)<<20 | (1+3+2*9)<<25) >> (face*5)) & 0x1f;

        // t offset in tuv coordinate system
        LAB_Chunk* chunk = chunks[c_off];
        if(!chunk) goto next_side;
        // absolute t value
        //int a_t = (15|32<<6)>>6*(face&1);
        //int a_t = (15|32<<4)>>4*(face&1);
        int a_t = 15+17*(face&1);
        // xyz -- bitfield: 0bzzzzzzyyyyyyxxxxxx;
        //                = 0bvvvvvvuuuuuutttttt %%18<< shift
        // where a%%b<<c is rotation around b bits
        const int shift = 6*(face>>1);
        int xyz_0 = a_t << shift;
        for(int u = 16; u<32; ++u)
        {
            int xyz_1 = xyz_0 | ((unsigned)(u|u<<3*6)<<shift)>>12;
            for(int v = 16; v<32; ++v)
            {
                unsigned xyz = xyz_1 | ((unsigned)(v|v<<3*6)<<shift)>>6;
                int x = (int)(xyz     & 077)-16;
                int y = (int)(xyz>> 6 & 077)-16;
                int z = (int)(xyz>>12 & 077)-16;
                int off;
                LAB_Chunk* cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &off);
                if(cnk)
                {
                    LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, default_color, default_color_above);
                    if(cnk->light[off] != lum)
                    {
                        changed |= 1<<face;
                        goto next_side;
                    }
                }
            }
        }
        next_side:;
        LAB_ASSUME(changed <= 63);
    }
    #endif

    // check and update center chunk
    LAB_Chunk* cnk = chunks[1+3+9];
    int x, y, z;
    //LAB_CCPS_EACH_POS(dirty_blocks, x, y, z,
    for(z = 0; z < 16; ++z)
    for(y = 0; y < 16; ++y)
    for(x = 0; x < 16; ++x)
    {
        int off = LAB_CHUNK_OFFSET(x, y, z);

        LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, default_color, default_color_above);

        if(cnk->light[off] != lum)
        {
            cnk->light[off] = lum;
            if(x<=0)  changed |=  1;
            if(x>=15) changed |=  2;
            if(y<=0)  changed |=  4;
            if(y>=15) changed |=  8;
            if(z<=0)  changed |= 16;
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
    //});
    #endif

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



#if 0
/**
 *  Return format: !?SNUDEW in binary
 *  !: Redo calculation
 *  ?: Any light changed
 *  S, N, U, D, E, W: Neighboring chunk needs to be updated
 */
LAB_HOT
LAB_STATIC int LAB_CheckLight(LAB_World* world, LAB_Chunk*const chunks[27], LAB_Color default_color)
{
    int changed = 0;

    #if 0
    LAB_Chunk* cnk = chunks[1+3+9];
    for(int z = 0; z < 16; ++z)
    for(int y =15; y >= 0; --y)
    for(int x = 0; x < 16; ++x)
    {
        int off = LAB_CHUNK_OFFSET(x, y, z);

        if(cnk->light[off] != LAB_CalcLight(world, chunks, x, y, z, default_color))
        {
            if(x==0)  changed |=  1;
            if(x==15) changed |=  2;
            if(y==0)  changed |=  4;
            if(y==15) changed |=  8;
            if(z==0)  changed |= 16;
            if(z==15) changed |= 32;
            changed |= 64;
        }
    }
    return changed;
    #else
    for(int z = 0-1; z < 16+1; ++z)
    for(int y =15+1; y >= 0-1; --y)
    for(int x = 0-1; x < 16+1; ++x)
    {
        int off;
        LAB_Chunk* cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &off);
        if(cnk && cnk->light[off] != LAB_CalcLight(world, chunks, x, y, z, default_color, default_color_above))
        {
            if(x<=0)  changed |=  1;
            if(x>=15) changed |=  2;
            if(y<=0)  changed |=  4;
            if(y>=15) changed |=  8;
            if(z<=0)  changed |= 16;
            if(z>=15) changed |= 32;
            changed |= 64;
        }
    }
    return changed;
    #endif
}
#endif

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
                case 0: if(x==-16) { faces_changed |=  1; goto next_face; } else break;
                case 1: if(x== 31) { faces_changed |=  2; goto next_face; } else break;
                case 2: if(y==-16) { faces_changed |=  4; goto next_face; } else break;
                case 3: if(y== 31) { faces_changed |=  8; goto next_face; } else break;
                case 4: if(z==-16) { faces_changed |= 16; goto next_face; } else break;
                case 5: if(z== 31) { faces_changed |= 32; goto next_face; } else break;
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
                    /*if(x<=0)  faces_changed |=  1;
                    if(x>=15) faces_changed |=  2;
                    if(y<=0)  faces_changed |=  4;
                    if(y>=15) faces_changed |=  8;
                    if(z<=0)  faces_changed |= 16;
                    if(z>=15) faces_changed |= 32;*/
                    faces_changed |= 64;

                    cnk->dirty_blocks = LAB_CCPS_AddPos(cnk->dirty_blocks, x&15, y&15, z&15);

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


    #if 0
    // expanded by 15 blocks version
    int changed = 1;
    int change_count = 0;
    while(changed)
    {
        changed = 0;
        for(int z =  0-15; z < 16+15; ++z)
        for(int z =  0-15; z < 16+15; ++y)
        for(int x =  0-15; x < 16+15; ++x)
        {
            int block_index;
            LAB_Chunk* cnk;
            cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &block_index);
            if(cnk)
            {
                LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, default_color);

                if(cnk->light[block_index] != lum)
                {
                    cnk->light[block_index] = lum;
                    if(x<=0)  faces_changed |=  1;
                    if(x>=15) faces_changed |=  2;
                    if(y<=0)  faces_changed |=  4;
                    if(y>=15) faces_changed |=  8;
                    if(z<=0)  faces_changed |= 16;
                    if(z>=15) faces_changed |= 32;
                    changed = 1;
                }
            }
        }
        change_count++;
    }
    #endif
    return faces_changed;
}
#else // if LAB_DIRECTIONAL_LIGHT == 1

// heuristic lighting algorithm
// run first time when the chunk has been created
LAB_ALWAYS_INLINE
LAB_STATIC void LAB_PrepareLight(LAB_Chunk* chunk, LAB_Chunk* chunk_above, LAB_Color default_color)
{
    for(int z = 0; z < 16; ++z)
    for(int x = 0; x < 16; ++x)
    {
        int xz_off = x | z<<8;
        LAB_Color l = chunk_above ? chunk_above->light[xz_off].faces[3] : default_color;
        for(int y = 15; y >= 0; --y)
        {
            int xyz_off = xz_off | y<<4;
            LAB_Color dia = chunk->blocks[xyz_off]->dia;
            l = LAB_MulColor(l, dia);
            l = LAB_MaxColor(l, LAB_MIN_LIGHT(dia));

            chunk->light[xyz_off].faces[0] =
            chunk->light[xyz_off].faces[1] =
            chunk->light[xyz_off].faces[2] =
            chunk->light[xyz_off].faces[4] =
            chunk->light[xyz_off].faces[5] = LAB_MIN_LIGHT(dia);
            chunk->light[xyz_off].faces[3] = l;
        }
    }
}










LAB_ALWAYS_INLINE
LAB_STATIC LAB_Color LAB_CalcLight(LAB_World* world, LAB_Chunk*const chunks[27], int x, int y, int z, int face, LAB_Color default_color, LAB_Color default_color_above)
{
    LAB_ASSUME(x >= -16+1 && x < 32-1);
    LAB_ASSUME(y >= -16+1 && y < 32-1);
    LAB_ASSUME(z >= -16+1 && z < 32-1);

    // "branchless"
    LAB_Color lum;

    LAB_Block* b_ctr = LAB_GetNeighborhoodBlock(chunks, x, y, z);
    LAB_Color dia = b_ctr->dia;

    lum = LAB_MIN_LIGHT(dia);
    lum = LAB_MaxColor(lum, b_ctr->lum);

    //const int* o = LAB_offset[i];
    int nlum, nlum1, nlum2;
    #if 0

    LAB_Block* block = LAB_GetNeighborhoodBlock(chunks, x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face));

    nlum1 = block->lum;
    nlum2 = LAB_GetNeighborhoodLight(chunks, x+LAB_OX(face), y+LAB_OY(face), z+LAB_OZ(face), face, face==3?default_color_above:default_color);

    nlum = LAB_MaxColor(nlum1, nlum2);
    lum = LAB_MaxColor(lum, LAB_MulColor_Fast(nlum, dia));
    #else
    /*#define BLOCKLIGHT(x, y, z) \
        LAB_MaxColor( \
            LAB_GetNeighborhoodBlock(chunks, (x), (y), (z))->lum, \
            LAB_GetNeighborhoodLight(chunks, (x), (y), (z), face, face==3?default_color_above:default_color) \
        )   */
    LAB_Block* tmp_block,* tmp_block_before;
    #define BLOCKLIGHT(x, y, z) (\
        tmp_block = LAB_GetNeighborhoodBlock(chunks, (x), (y), (z)), \
        tmp_block_before = LAB_GetNeighborhoodBlock(chunks, (x)-LAB_OX(face), (y)-LAB_OY(face), (z)-LAB_OZ(face)), \
        LAB_MulColor( \
            LAB_MaxColor( \
                LAB_GetNeighborhoodLight(chunks, (x), (y), (z), face, face==3?default_color_above:default_color), \
                tmp_block->lum \
            ), \
            tmp_block_before->dia \
        ) \
    )

    int xx = x+LAB_OX(face),  yy = y+LAB_OY(face),  zz = z+LAB_OZ(face);
    int ax = LAB_AX(face>>1), ay = LAB_AY(face>>1), az = LAB_AZ(face>>1);
    int bx = LAB_BX(face>>1), by = LAB_BY(face>>1), bz = LAB_BZ(face>>1);
    #define BLOCKLIGHT_UV(u, v) \
        BLOCKLIGHT(xx-(u)*ax-(v)*bx, yy-(u)*ay-(v)*by, zz-(u)*az-(v)*bz)

    #if 0
    /**
     *  1 2 1      1 2 3
     *  2 4 2  ==  4 5 6
     *  1 2 1      7 8 9
     *
     *  .25(1+3 + 7+9) -- x1
     *  .25(2+4 + 6+8) -- x2
     *   5             -- x4
     *
     *  .5(.5(x1+x4)+x2)
    **/
    #define KERNEL_RADIUS 1

    LAB_Color c1, c2, c3, c4, c5, c6, c7, c8, c9;

    c1 = BLOCKLIGHT(xx-ax-bx, yy-ay-by, zz-az-bz);
    c2 = BLOCKLIGHT(xx   -bx, yy   -by, zz   -bz);
    c3 = BLOCKLIGHT(xx+ax-bx, yy+ay-by, zz+az-bz);

    c4 = BLOCKLIGHT(xx-ax   , yy-ay   , zz-az   );
    c5 = BLOCKLIGHT(xx      , yy      , zz      );
    c6 = BLOCKLIGHT(xx+ax   , yy+ay   , zz+az   );

    c7 = BLOCKLIGHT(xx-ax+bx, yy-ay+by, zz-az+bz);
    c8 = BLOCKLIGHT(xx   +bx, yy   +by, zz   +bz);
    c9 = BLOCKLIGHT(xx+ax+bx, yy+ay+by, zz+az+bz);

    nlum = LAB_MixColor50(LAB_MixColor50(LAB_MixColor4x25(c1,c3, c7,c9), c5), LAB_MixColor4x25(c2,c4, c6,c8));
    #elif 0
    /**
     *    1          2
     *  1 4 1  ==  4 5 6
     *    1          8
     *
     *  .25(1+3 + 7+9) -- x1
     *  .25(2+4 + 6+8) -- x2
     *   5             -- x4
     *
     *  .5(.5(x1+x4)+x2)
    **/

    LAB_Color c1, c2, c3, c4, c5, c6, c7, c8, c9;

    c2 = BLOCKLIGHT(xx   -bx, yy   -by, zz   -bz);

    c4 = BLOCKLIGHT(xx-ax   , yy-ay   , zz-az   );
    c5 = BLOCKLIGHT(xx      , yy      , zz      );
    c6 = BLOCKLIGHT(xx+ax   , yy+ay   , zz+az   );

    c8 = BLOCKLIGHT(xx   +bx, yy   +by, zz   +bz);

    nlum = LAB_MixColor50(c5, LAB_MixColor4x25(c2,c4, c6,c8));
    #elif 0
    /**
     *    3          2
     *  3 4 3  ==  4 5 6
     *    3          8
     *
     *  .25(1+3 + 7+9) -- x1
     *  .25(2+4 + 6+8) -- x2
     *   5             -- x4
     *
     *  .5(.5(x1+x4)+x2)
    **/

    LAB_Color c1, c2, c3, c4, c5, c6, c7, c8, c9;

    c2 = BLOCKLIGHT(xx   -bx, yy   -by, zz   -bz);

    c4 = BLOCKLIGHT(xx-ax   , yy-ay   , zz-az   );
    c5 = BLOCKLIGHT(xx      , yy      , zz      );
    c6 = BLOCKLIGHT(xx+ax   , yy+ay   , zz+az   );

    c8 = BLOCKLIGHT(xx   +bx, yy   +by, zz   +bz);

    LAB_Color c = LAB_MixColor4x25(c2,c4, c6,c8);

    nlum = LAB_MixColor50(LAB_MixColor50(c5, c), c);
    #elif 0
    /**
     *    7          2
     *  7 4 7  ==  4 5 6
     *    7          8
    **/

    LAB_Color c1, c2, c3, c4, c5, c6, c7, c8, c9;

    c1 = BLOCKLIGHT(xx-ax-bx, yy-ay-by, zz-az-bz);
    c2 = BLOCKLIGHT(xx   -bx, yy   -by, zz   -bz);
    c3 = BLOCKLIGHT(xx+ax-bx, yy+ay-by, zz+az-bz);

    c4 = BLOCKLIGHT(xx-ax   , yy-ay   , zz-az   );
    c5 = BLOCKLIGHT(xx      , yy      , zz      );
    c6 = BLOCKLIGHT(xx+ax   , yy+ay   , zz+az   );

    c7 = BLOCKLIGHT(xx-ax+bx, yy-ay+by, zz-az+bz);
    c8 = BLOCKLIGHT(xx   +bx, yy   +by, zz   +bz);
    c9 = BLOCKLIGHT(xx+ax+bx, yy+ay+by, zz+az+bz);


    LAB_Color mult = 0xffd0d0d0;
    c1 = LAB_MulColor(c1, mult);
    c2 = LAB_MulColor(c2, mult);
    c3 = LAB_MulColor(c3, mult);
    c4 = LAB_MulColor(c4, mult);

    c5 = LAB_MulColor(c5, mult);

    c6 = LAB_MulColor(c6, mult);
    c7 = LAB_MulColor(c7, mult);
    c8 = LAB_MulColor(c8, mult);
    c9 = LAB_MulColor(c9, mult);


    LAB_Color c = LAB_MaxColor(LAB_MaxColor(LAB_MaxColor(c1,c3), LAB_MaxColor(c7,c9)),
                               LAB_MaxColor(LAB_MaxColor(c2,c4), LAB_MaxColor(c6,c8)));
    nlum = LAB_MaxColor(c, c5);
    #elif 0
    /**
     *  Light is transferred with a 5x5 kernel
     *
     * // NO:
     *                    1 3 3 1
     *  1 2 1   1 1       3 7 7 3
     *  2 4 2 * 1 1   =   3 7 7 3
     *  1 2 1             1 3 3 1
     *       /16                 /64
     *
     *                      1  4  6  4  1
     *  1 3 3 1             4 14 20 14  4
     *  3 7 7 3   1 1       6 20 28 20  6
     *  3 7 7 3 * 1 1   =   4 14 20 14  4
     *  1 3 3 1             1  4  6  4  1
     *         /256                      /1024
     *
     *
     *  0 1 2 1 0
     *  1 2 3 2 1
     *  2 3 4 3 2
     *  1 2 3 2 1
     *  0 1 2 1 0

     *  0 1 1 1 0
     *  1 2 2 2 1
     *  1 2 4 2 1
     *  1 2 2 2 1
     *  0 1 1 1 0
     *           /32
     */
    #define KERNEL_RADIUS 2

    // 1 wheighted
    LAB_Color a = LAB_MixColor4x25(BLOCKLIGHT_UV(-1,-2), BLOCKLIGHT_UV( 2,-1), BLOCKLIGHT_UV( 1, 2), BLOCKLIGHT_UV(-2, 1));
    LAB_Color b = LAB_MixColor4x25(BLOCKLIGHT_UV( 0,-2), BLOCKLIGHT_UV( 2, 0), BLOCKLIGHT_UV( 0, 2), BLOCKLIGHT_UV(-2, 0));
    LAB_Color c = LAB_MixColor4x25(BLOCKLIGHT_UV( 1,-2), BLOCKLIGHT_UV( 2, 1), BLOCKLIGHT_UV(-1, 2), BLOCKLIGHT_UV(-2,-1));

    // 2 wheighted
    LAB_Color d = LAB_MixColor4x25(BLOCKLIGHT_UV(-1,-1), BLOCKLIGHT_UV( 1,-1), BLOCKLIGHT_UV( 1, 1), BLOCKLIGHT_UV(-1, 1));
    LAB_Color e = LAB_MixColor4x25(BLOCKLIGHT_UV( 0,-1), BLOCKLIGHT_UV( 1, 0), BLOCKLIGHT_UV( 0, 1), BLOCKLIGHT_UV(-1, 0));

    LAB_Color f = LAB_MixColor4x25(a, b, c, BLOCKLIGHT_UV(0, 0));
    nlum = LAB_MixColor50(f, LAB_MixColor50(d, e));

    #elif 0
    /**
     *  Light is transferred by mixing in the direction: 2D Gaussian Kernel
     *  Light is transferred by max perpendicular to the direction
    **/
    #else
    /**
     *  Mix color with 2D 3x3 Gaussian Kernel or use the minimum of these multiplied by a factor
     *
     *  1 2 1
     *  2 4 2
     *  1 2 1
     *
    **/
    #define KERNEL_RADIUS 1

    // Gaussian
    LAB_Color g1 = LAB_MixColor4x25(BLOCKLIGHT_UV(-1,-1), BLOCKLIGHT_UV( 1,-1),
                                    BLOCKLIGHT_UV( 1, 1), BLOCKLIGHT_UV(-1, 1));

    LAB_Color g2 = LAB_MixColor4x25(BLOCKLIGHT_UV( 0,-1), BLOCKLIGHT_UV( 1, 0),
                                    BLOCKLIGHT_UV( 0, 1), BLOCKLIGHT_UV(-1, 0));

    LAB_Color g  = LAB_MixColor50(g2, LAB_MixColor50(g1, BLOCKLIGHT_UV(0, 0)));

    //g = LAB_SubColor(g, 0x00101010);

    // Maximum
    LAB_Color m1 = LAB_SubColor(LAB_MaxColor(LAB_MaxColor(BLOCKLIGHT_UV(-1,-1), BLOCKLIGHT_UV( 1,-1)),
                                             LAB_MaxColor(BLOCKLIGHT_UV( 1, 1), BLOCKLIGHT_UV(-1, 1))),
                                0x00303030);

    LAB_Color m2 = LAB_SubColor(LAB_MaxColor(LAB_MaxColor(BLOCKLIGHT_UV( 0,-1), BLOCKLIGHT_UV( 1, 0)),
                                LAB_MaxColor(BLOCKLIGHT_UV( 0, 1), BLOCKLIGHT_UV(-1, 0))),
                                0x00303030);

    LAB_Color m4 = LAB_SubColor(BLOCKLIGHT_UV(0, 0), 0x00303030);

    LAB_Color m  = LAB_MaxColor(m2, LAB_MaxColor(m1, m4));


    nlum = LAB_MaxColor(g, m);

    #endif
    lum = LAB_MaxColor(lum, nlum);//LAB_MulColor_Fast(nlum, dia));
    #endif

    return lum;
}









// neighborhood coordinates compressed to 3*6 bits
// each from -16 to 31
// each from 0-16 to 47-16 -> 6 bits each
#define LAB_NBH(x, y, z, face) (((x)+16)|((y)+16)<<6|((z)+16)<<12|(face)<<18)
#define LAB_NBH_X(xyzf)     (((xyzf)    &077)-16)
#define LAB_NBH_Y(xyzf)     (((xyzf)>> 6&077)-16)
#define LAB_NBH_Z(xyzf)     (((xyzf)>>12&077)-16)
#define LAB_NBH_FACE(xyzf)   ((xyzf)>>18    )

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
    /*LAB_CCPS_EACH_NEAR_POS(dirty_blocks, x, y, z,
    {
        int off;
        LAB_Chunk* cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &off);
        if(cnk)
        {
            for(int face = 0; face < 6; ++face)
            {
                LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, face, default_color, default_color_above);

                if(cnk->light[off].faces[face] != lum)
                {
                    cnk->light[off].faces[face] = lum;
                    if(x==0)  changed |=  1;
                    if(x==15) changed |=  2;
                    if(y==0)  changed |=  4;
                    if(y==15) changed |=  8;
                    if(z==0)  changed |= 16;
                    if(z==15) changed |= 32;
                    changed |= 64;

                    if(*queue_count != queue_cap)
                        queue[(*queue_count)++] = LAB_NBH(x,y,z,face);
                    else
                    {
                        changed |= 128;
                        //printf("QUEUE FULL\n");
                    }
                }
            }
        }
    });*/
    LAB_CCPS_EACH_POS(dirty_blocks, x, y, z,
    {
        int off;
        LAB_Chunk* cnk = LAB_GetNeighborhoodRef(chunks, x, y, z, &off);
        if(cnk)
        {
            for(int face = 0; face < 6; ++face)
            {
                LAB_Color lum = LAB_CalcLight(world, chunks, x, y, z, face, default_color, default_color_above);

                // check block itself
                if(cnk->light[off].faces[face] != lum)
                {
                    cnk->light[off].faces[face] = lum;
                    if(x==0)  changed |=  1;
                    if(x==15) changed |=  2;
                    if(y==0)  changed |=  4;
                    if(y==15) changed |=  8;
                    if(z==0)  changed |= 16;
                    if(z==15) changed |= 32;
                    changed |= 64;

                    if(*queue_count != queue_cap)
                        queue[(*queue_count)++] = LAB_NBH(x,y,z,face);
                    else
                    {
                        changed |= 128;
                        //printf("QUEUE FULL\n");
                    }
                }

                // check neighboring blocks in a given direction
                int ox = LAB_OX(face),    oy = LAB_OY(face),    oz = LAB_OZ(face);
                int ax = LAB_AX(face>>1), ay = LAB_AY(face>>1), az = LAB_AZ(face>>1);
                int bx = LAB_BX(face>>1), by = LAB_BY(face>>1), bz = LAB_BZ(face>>1);
                int aax = -ax, aay = -ay, aaz = -az;
                LAB_UNROLL(3)
                for(int a=-1; a<=1; ++a, (aax+=ax), (aay+=ay), (aaz+=az))
                {
                    int bbx = -bx, bby = -by, bbz = -bz;
                    LAB_UNROLL(3)
                    for(int b=-1; b<=1; ++b, (bbx+=bx), (bby+=by), (bbz+=bz))
                    {
                        int xx = x-ox+aax+bbx, yy = y-oy+aay+bby, zz = z-oz+aaz+bbz;


                        int off2;
                        LAB_Chunk* cnk2 = LAB_GetNeighborhoodRef(chunks, xx, yy, zz, &off2);
                        if(cnk2)
                        {
                            LAB_Color lum = LAB_CalcLight(world, chunks, xx, yy, zz, face, default_color, default_color_above);

                            // check block itself
                            if(cnk2->light[off2].faces[face] != lum)
                            {
                                cnk2->light[off2].faces[face] = lum;
                                if(xx==0)  changed |=  1;
                                if(xx==15) changed |=  2;
                                if(yy==0)  changed |=  4;
                                if(yy==15) changed |=  8;
                                if(zz==0)  changed |= 16;
                                if(zz==15) changed |= 32;
                                changed |= 64;

                                if(*queue_count != queue_cap)
                                    queue[(*queue_count)++] = LAB_NBH(xx,yy,zz,face);
                                else
                                {
                                    changed |= 128;
                                    //printf("QUEUE FULL\n");
                                }
                            }
                        }
                    }
                }
            }
        }
    });

    return changed;
}

LAB_HOT                                                // TODO: |--------------------| not used
int LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27], int cx, int cy, int cz)
{
    LAB_Chunk* ctr_cnk = chunks[1+3+9];
    if(!ctr_cnk) return 0;

    const LAB_Color dark = LAB_RGB(1, 1, 1); //LAB_RGB(16, 16, 16)

    LAB_Color default_color_above = cy <  -2 ? dark : LAB_RGB(255, 255, 255);
    //LAB_Color default_color = cy <  0 ? dark : LAB_RGB(255, 255, 255);
    LAB_Color default_color = dark;

    if(!ctr_cnk->light_generated)
    {
        LAB_PrepareLight(ctr_cnk, chunks[1+2*3+9], default_color_above);
        ctr_cnk->light_generated = 1;
        ctr_cnk->dirty_blocks = ~0; // check all blocks again
        //return ~0; // TODO: multiple updates to the view
        return 128;
    }

    #define LAB_LIGHT_QUEUE_SIZE (48*48*48)/*(1<<14)*/
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
        int xyzf = queue[queue_first];
        int qx = LAB_NBH_X(xyzf);
        int qy = LAB_NBH_Y(xyzf);
        int qz = LAB_NBH_Z(xyzf);
        int qface = LAB_NBH_FACE(xyzf);
        queue_first++;
        queue_first&=LAB_LIGHT_QUEUE_MASK;
        queue_count--;

        #if 1
        // optimize all 6 cases
        LAB_UNROLL(6)
        for(int face = 0; face < 6; ++face)
        if(face==qface)
        #else
        int face = qface;
        #endif
        {
            int x = qx-LAB_OX(face);
            int y = qy-LAB_OY(face);
            int z = qz-LAB_OZ(face);
            /*switch(face)
            {
                case 0: if(x==-16) { faces_changed |=  1; goto next_face; } else break;
                case 1: if(x== 31) { faces_changed |=  2; goto next_face; } else break;
                case 2: if(y==-16) { faces_changed |=  4; goto next_face; } else break;
                case 3: if(y== 31) { faces_changed |=  8; goto next_face; } else break;
                case 4: if(z==-16) { faces_changed |= 16; goto next_face; } else break;
                case 5: if(z== 31) { faces_changed |= 32; goto next_face; } else break;
            }*/
            /*int next_face = 0;
            if(x<=-15) { faces_changed |=  1; next_face=1; }
            if(x>= 30) { faces_changed |=  2; next_face=1; }
            if(y<=-15) { faces_changed |=  4; next_face=1; }
            if(y>= 30) { faces_changed |=  8; next_face=1; }
            if(z<=-15) { faces_changed |= 16; next_face=1; }
            if(z>= 30) { faces_changed |= 32; next_face=1; }
            if(next_face) goto next_face;*/

            int ax = LAB_AX(face>>1), ay = LAB_AY(face>>1), az = LAB_AZ(face>>1);
            int bx = LAB_BX(face>>1), by = LAB_BY(face>>1), bz = LAB_BZ(face>>1);
            int aax = -ax, aay = -ay, aaz = -az;
            LAB_UNROLL(3)
            for(int a=-1; a<=1; ++a, (aax+=ax), (aay+=ay), (aaz+=az))
            {
                int bbx = -bx, bby = -by, bbz = -bz;
                LAB_UNROLL(3)
                for(int b=-1; b<=1; ++b, (bbx+=bx), (bby+=by), (bbz+=bz))
                {
                    int xx = x+aax+bbx, yy = y+aay+bby, zz = z+aaz+bbz;


                    int next_face = 0;
                    if(xx==-17+KERNEL_RADIUS) { faces_changed |=  1; next_face=1; }
                    if(xx== 32-KERNEL_RADIUS) { faces_changed |=  2; next_face=1; }
                    if(yy==-17+KERNEL_RADIUS) { faces_changed |=  4; next_face=1; }
                    if(yy== 32-KERNEL_RADIUS) { faces_changed |=  8; next_face=1; }
                    if(zz==-17+KERNEL_RADIUS) { faces_changed |= 16; next_face=1; }
                    if(zz== 32-KERNEL_RADIUS) { faces_changed |= 32; next_face=1; }
                    if(next_face) continue;


                    int block_index;
                    LAB_Chunk* cnk;
                    cnk = LAB_GetNeighborhoodRef(chunks, xx, yy, zz, &block_index);
                    if(cnk)
                    {
                        LAB_Color lum = LAB_CalcLight(world, chunks, xx, yy, zz, face, default_color, default_color_above);

                        if(cnk->light[block_index].faces[face] != lum)
                        {
                            cnk->light[block_index].faces[face] = lum;

                            faces_changed |= 64;

                            cnk->dirty_blocks = LAB_CCPS_AddPos(cnk->dirty_blocks, xx&15, yy&15, zz&15);

                            if(queue_count != LAB_LIGHT_QUEUE_SIZE)
                            {
                                queue[(queue_first+(queue_count++))&LAB_LIGHT_QUEUE_MASK] = LAB_NBH(xx, yy, zz, face);
                            }
                            else
                            {
                                faces_changed |= 128;
                                printf("FULL QUEUE\n");
                            }
                        }
                    }
                }
            }

            //next_face:;
        }
    }

    return faces_changed;
}
#endif
