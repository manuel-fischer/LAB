#include "LAB_world_light.h"
#include "LAB_debug.h"
#include "LAB_chunk_pseudo.h"
#include "LAB_chunk_neighborhood.h"
#include "LAB_blocks.h"
// TODO: use pseudo chunks instead of NULL chunks -> no branching required
//       - build chunk neighborhood with these pseudo chunks
// TODO: reduce range of searching for possibly changed block positions
//       - use crammed chunk pos set, stored in the chunks

#include <stdio.h> // DBG

#define LAB_LIGHT_PRECISION 32

// minimum light level for completely unlit block with that dia
//#define LAB_DARK_LIGHT(dia) ((((dia)>>4) & 0x0f0f0fu) | 0xff000000u)
//#define LAB_DARK_LIGHT(dia) (0)
//#define LAB_MIN_LIGHT(dia) ((((dia)>>6) & 0x030303u) | 0xff000000u)
//#define LAB_MIN_LIGHT(dia) (0)
//#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((lum)>>3 & 0x1f1f1f)-((lum)>>4 & 0x0f0f0f))
//#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((lum)>>3 & 0x1f1f1f))
//#define LAB_LIGHT_FALL_OFF(lum) ((lum) - ((((lum)&0xf8f8f8)+0x070707) >> 3 & 0xf8f8f8))

//#define LAB_LIGHT_FALL_OFF(lum) ((lum) - (((lum)+(~((lum)>>5) & 0x070707)) >> 3 & 0x1f1f1f) - ((lum)>>4 & 0x0f0f0f))
#define LAB_LIGHT_FALL_OFF(lum) LAB_MulColorHDR_RoundUp(lum, LAB_LIGHT_ALPHA)

#define LAB_LIGHT_ALPHA LAB_HDR_RGB_F(0.8125,0.8125,0.8125)
////#define LAB_LIGHT_ALPHA LAB_HDR_RGB_F(0.95,0.95,0.95)

#define LAB_LIGHT_BETA LAB_HDR_RGB_F(0.1875,0.1875,0.1875)
//#define LAB_LIGHT_BETA LAB_HDR_RGB_F(0.5,0.5,0.5)


//#define LAB_IS_SKYLIGHT(is_down, cf) ((is_down) && ((cf)&LAB_COL_MASK) == LAB_COL_MASK)
//#define LAB_IS_SKYLIGHT(is_down, cf) ((is_down))
#define LAB_IS_SKYLIGHT(is_down, cf) ((is_down) && ((cf) == LAB_HDR_UNIT_WHITE))
//#define LAB_IS_SKYLIGHT(is_down, cf) 0

// TODO remove
// light conversion
#define LAB_LCV(color) LAB_Color_To_ColorHDR(color)

//#define LAB_LNORM(c) ((c) | LAB_ALP_MASK)
#define LAB_LNORM(c) (c)
//#define LAB_LNORM(c) LAB_MaxColorHDR(c, LAB_HDR_RGB_F(0.01, 0.01, 0.01))

// return blocks that changed
LAB_STATIC LAB_HOT
LAB_CCPS LAB_TickLight_ProcessQuadrant(
    LAB_Chunk_Blocks* blocks_ctr, LAB_LightNbHood_Mut* light_chunks,
    int quadrant, bool init, 
    int faces_changed, LAB_CCPS update_blocks)
{

    LAB_CCPS relit_blocks = 0;

    LAB_Chunk_Light* light_ctr = light_chunks->bufs[LAB_NB_CENTER];

    LAB_ASSERT(blocks_ctr);

    #define LAB_DEBUG_INFO_HERE(fmt, ...) \
        "c=%8x, q=%i, dia=%8x, lum=%8x\n    "\
        "primary={%8x, %8x, %8x}, secondary={%8x, %8x, %8x}" fmt, \
        \
        c, quadrant, LAB_LCV(b->dia), LAB_LCV(b->lum),\
        primary[0], primary[1], primary[2], secondary[0], secondary[1], secondary[2] __VA_ARGS__
            

    #define LAB_TickLight_ProcessBlock(getLightAt) do \
    { \
        int block_index = LAB_XADD3(x, y<<4, z<<8); \
        \
        LAB_Block* b = LAB_BlockP(blocks_ctr->blocks[block_index]); \
        LAB_ColorHDR c = 0; \
        LAB_ColorHDR primary[3]; \
        primary[0] = getLightAt(light_chunks, x-dd[0], y, z, quadrant); \
        primary[1] = getLightAt(light_chunks, x, y-dd[1], z, quadrant); \
        primary[2] = getLightAt(light_chunks, x, y, z-dd[2], quadrant); \
        LAB_ColorHDR secondary[3]; \
        secondary[0] = getLightAt(light_chunks, x, y-dd[1], z-dd[2], quadrant); \
        secondary[1] = getLightAt(light_chunks, x-dd[0], y, z-dd[2], quadrant); \
        secondary[2] = getLightAt(light_chunks, x-dd[0], y-dd[1], z, quadrant); \
        LAB_ColorHDR secondary_mixed[3]; /* each sums of lsj*/ \
        secondary_mixed[0] = LAB_MixColorHDR50(secondary[1], secondary[2]); \
        secondary_mixed[1] = LAB_MixColorHDR50(secondary[0], secondary[2]); \
        secondary_mixed[2] = LAB_MixColorHDR50(secondary[0], secondary[1]); \
        LAB_UNROLL(3) \
        for(int i = 0; i < 3; ++i) \
        { \
            bool is_down = i==1 && !(quadrant&2); \
            LAB_ColorHDR prm = primary[i]; \
            LAB_ColorHDR sec = secondary[i]; \
            LAB_ColorHDR cf = prm; \
            if(!LAB_IS_SKYLIGHT(is_down, cf)) \
                cf = LAB_LIGHT_FALL_OFF(cf); \
            c = LAB_MaxColorHDR(c, cf); \
            LAB_ASSERT_FMT(LAB_HDR_EXP_VALUE(c) < 10, \
                LAB_DEBUG_INFO_HERE("\n    i=%i, cf=%8x",, i, cf)); \
            if(0) LAB_DBG_PRINTF("prm=%8x, sec=%8x, cf=%8x\n",prm, sec, cf); \
        } \
        \
        /*c = LAB_MulColorHDR_RoundUp(c, LAB_RGBE_HDR_N(86, 86, 86, 0x80));*/ \
        c = LAB_MaxColorHDR(LAB_MulColorHDR(c, LAB_LCV(b->dia)), LAB_LCV(b->lum)); \
        c = LAB_LNORM(c); \
        c = LAB_SELECT0(LAB_HDR_EXP(c) >= 128-LAB_LIGHT_PRECISION, c); \
        \
        bool relit = c_init || light_ctr->light[block_index].quadrants[quadrant] != c; \
        LAB_ASSERT_FMT(LAB_HDR_EXP_VALUE(c) < 10, LAB_DEBUG_INFO_HERE(,)); \
        \
        /*if(relit)*/ \
        { \
            light_ctr->light[block_index].quadrants[quadrant] = c; \
            relit_blocks  |= LAB_CCPS_Pos(x, y, z)*relit; \
            update_blocks |= LAB_CCPS_Pos(x, y, z)*relit; \
        } \
    } while(0)



    int dd[3];
    int dx, dy, dz; // delta index = sign
    int x0, y0, z0; // start index
    int xm, ym, zm; // scan kernel
    
    if(quadrant & 1) { dd[0] = dx =  1; x0 =  0; xm = 3; } 
    else             { dd[0] = dx = -1; x0 = 15; xm = 6; }
    if(quadrant & 2) { dd[1] = dy =  1; y0 =  0; ym = 3; } 
    else             { dd[1] = dy = -1; y0 = 15; ym = 6; }
    if(quadrant & 4) { dd[2] = dz =  1; z0 =  0; zm = 3; } 
    else             { dd[2] = dz = -1; z0 = 15; zm = 6; }



    int axis_changed = (!!(faces_changed & ( (1|2)  ^  1<<!!(quadrant&1))))
                     | (!!(faces_changed & ( (4|8)  ^  4<<!!(quadrant&2)))) << 1
                     | (!!(faces_changed & ((16|32) ^ 16<<!!(quadrant&4)))) << 2;


    LAB_PULL_CONST(int, c_init, 2, init)
    {

        int zi, yi, xi;
        int x, y, z;
        zi = yi = xi = 0; // TODO: hide warning

        // Hide uninitialized warning message
        zi=yi=xi = x=y=z = 0;

        // if-condition: check if the previous or current column has changes
        #define x_loop(start, all_if) \
            for(xi = (start), x = x0 + (start)*dx; xi < 16; ++xi, x += dx) \
            if(c_init || (all_if) || update_blocks <<  1 & (xm << x) & 0x1fffe)
            
        #define y_loop(start, all_if) \
            for(yi = (start), y = y0 + (start)*dy; yi < 16; ++yi, y += dy) \
            if(c_init || (all_if) || update_blocks >> 15 & (ym << y) & 0x1fffe)
            
        #define z_loop(start, all_if) \
            for(zi = (start), z = z0 + (start)*dz; zi < 16; ++zi, z += dz) \
            if(c_init || (all_if) || update_blocks >> 31 & (zm << z) & 0x1fffe)


        {
            int cur_loop = 0;



            #define LOOP_YIELD(i) do \
            { \
                cur_loop = (i); \
                goto process_block; \
                case (i):; \
            } while(0)

            if(axis_changed || LAB_CCPS_HasPos(update_blocks, x0, y0, z0))
            {
                x = x0, y = y0; z = z0;
        process_block:;
                #define LAB_GETLIGHTAT(light_chunks, x, y, z, quadrant) \
                    (LAB_LightNbHood_RefLightNode(light_chunks, x, y, z)->quadrants[quadrant])

                LAB_TickLight_ProcessBlock(LAB_GETLIGHTAT);
                #undef LAB_GETLIGHTAT
            }

            switch(cur_loop)
            {
                case 0:
                

                // x-axis:
                z = z0; y = y0;
                x_loop(1, axis_changed & (4|2))
                    LOOP_YIELD(1);

                // y-axis:
                z = z0; x = x0;
                y_loop(1, axis_changed & (4|1))
                    LOOP_YIELD(2);

                // z-axis:
                y = y0; x = x0;
                z_loop(1, axis_changed & (2|1))
                    LOOP_YIELD(3);

                        
                // x-plane:
                x = x0;
                z_loop(1, axis_changed & (1))
                y_loop(1, axis_changed & (1))
                    LOOP_YIELD(4);
  
                // y-plane
                y = y0;
                z_loop(1, axis_changed & (2))
                x_loop(1, axis_changed & (2))
                    LOOP_YIELD(5);

                // z-plane
                z = z0;
                y_loop(1, axis_changed & (4))
                x_loop(1, axis_changed & (4))
                    LOOP_YIELD(6);
            }
        }



        z_loop(1, false)
        y_loop(1, false)
        x_loop(1, false)
        {
            #define LAB_GETLIGHTAT(light_chunks, x, y, z, quadrant) ( \
                LAB_ASSERT_FMT(0 <= (x) && (x) < 16, "x=%d", x), \
                LAB_ASSERT_FMT(0 <= (y) && (y) < 16, "y=%d", y), \
                LAB_ASSERT_FMT(0 <= (z) && (z) < 16, "z=%d", z), \
                (light_ctr->light[LAB_XADD3((x), (y)<<4, (z)<<8)].quadrants[quadrant])\
            )

            LAB_TickLight_ProcessBlock(LAB_GETLIGHTAT);
            #undef LAB_GETLIGHTAT
        }
    }
    return relit_blocks;

    #undef LAB_DEBUG_INFO_HERE
}


LAB_HOT
LAB_CCPS LAB_TickLight(LAB_World* world, LAB_Chunk*const chunks[27],
                       int neighbors_changed, LAB_CCPS blocks_changed)
{
    LAB_Chunk* chunk = chunks[LAB_NB_CENTER];
    LAB_ASSERT(chunk);

    for(int i = 0; i < 27; ++i)
    {
        if(i != LAB_NB_CENTER)
            LAB_ASSERT(!chunks[i] || chunks[i]->light_generated);
    }


    LAB_CCPS relit_blocks = 0;


    uint8_t face2quads[6] =
    {
        (1<<1)|(1<<3)|(1<<5)|(1<<7),
        (1<<0)|(1<<2)|(1<<4)|(1<<6),

        (1<<2)|(1<<3)|(1<<6)|(1<<7),
        (1<<0)|(1<<1)|(1<<4)|(1<<5),

        (1<<4)|(1<<5)|(1<<6)|(1<<7),
        (1<<0)|(1<<1)|(1<<2)|(1<<3),
    };

    /*uint64_t face2ccps[6] =
    {
        0xffffffffffff0001,
        0xffffffffffff8000,
        0xffffffff0001ffff,
        0xffffffff8000ffff,
        0xffff0001ffffffff,
        0xffff8000ffffffff,
    };*/

    //LAB_CCPS boundary_blocks = 0;
    int faces_changed = 0;

    int relit_quads = 0;
    int face;
    LAB_DIR_EACH(63, face,
    {
        int i = LAB_OX(face)+1 + (LAB_OY(face)+1)*3 + (LAB_OZ(face)+1)*9;
        if(neighbors_changed & 1<<i)
        {
            faces_changed |= 1<<face;
            relit_quads |= face2quads[face];
            //boundary_blocks |= face2ccps[face];
        }
    });

    if(!chunk->light_generated || blocks_changed)
        relit_quads = 255;


    if(chunk->buf_blocks == 0) // Optimization: all neighboring chunks are sky
    {
        LAB_Chunk_Light* l = LAB_Chunk_Light_Read_ByY(chunk->pos.y);

        bool opt_light = true;

        for(int f = 0; f < 6; ++f)
        {
            int i = LAB_OX(f)+1 + (LAB_OY(f)+1)*3 + (LAB_OZ(f)+1)*9;
            if(chunks[i] == NULL || chunks[i]->buf_light == l) continue;

            opt_light = false;
            break;
        }

        if(opt_light)
        {
            LAB_Chunk_SetLightBuf(chunk, l);
            chunk->light_generated = true;
            return 0; // 0 ok here
        }
    }


    LAB_Chunk_Blocks* ctr_blocks = LAB_Chunk_Blocks_Read(chunk);
    if(!ctr_blocks) return 0; // TODO: correct error handling

    LAB_LightNbHood_Mut light_chunks;
    if(!LAB_LightNbHood_GetWrite(chunks, &light_chunks)) return 0; // TODO: correct error handling

    for(int i = 0; i < 8; ++i, relit_quads>>=1)
    {
        if(relit_quads&1)
            relit_blocks |= LAB_TickLight_ProcessQuadrant(
                ctr_blocks, &light_chunks, i, 
                !chunk->light_generated, faces_changed, blocks_changed);
    }

    LAB_Chunk_Light_Optimize(chunk);
    
    chunk->light_generated = true;
    return relit_blocks;
}