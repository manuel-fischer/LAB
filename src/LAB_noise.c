#include "LAB_noise.h"


#if 0

#define DEF_NOISE_FUNC(me, rec)                                                          \
LAB_HOT LAB_INLINE                                                                       \
void LAB_SmoothNoiseRec##me(LAB_OUT uint32_t smooth[16*16],                       \
                                   LAB_IN uint64_t noise[17*17],                         \
                                   int x0, int y0, int x1, int y1,                       \
                                   uint32_t n00, uint32_t n01,                           \
                                   uint32_t n10, uint32_t n11)                           \
{                                                                                        \
    if(x0+1 == x1)                                                                       \
    {                                                                                    \
        smooth[x0|y0<<4] = ((uint64_t)n00+(uint64_t)n01+(uint64_t)n10+(uint64_t)n11) / 4;\
    }                                                                                    \
    else                                                                                 \
    {                                                                                    \
        int xc, yc;                                                                      \
        uint64_t r0c, rc0, rcc, rc1, r1c;                                                \
        uint32_t n0c, nc0, ncc, nc1, n1c;                                                \
                                                                                         \
        xc = (x0+x1) >> 1;                                                               \
        yc = (y0+y1) >> 1;                                                               \
                                                                                         \
        r0c = noise[xc+17*y0] & 0xffffffff;                                              \
        rc0 = noise[x0+17*yc] & 0xffffffff;                                              \
        rcc = noise[xc+17*yc] & 0xffffffff;                                              \
        rc1 = noise[x1+17*yc] & 0xffffffff;                                              \
        r1c = noise[xc+17*y1] & 0xffffffff;                                              \
                                                                                         \
                                                                                         \
        n0c = ((uint64_t)n00*(0x100000000ll-r0c)+(uint64_t)n01*r0c)>>32;                 \
        nc0 = ((uint64_t)n00*(0x100000000ll-rc0)+(uint64_t)n10*rc0)>>32;                 \
                                                                                         \
        nc1 = ((uint64_t)n01*(0x100000000ll-rc1)+(uint64_t)n11*rc1)>>32;                 \
        n1c = ((uint64_t)n10*(0x100000000ll-r1c)+(uint64_t)n11*r1c)>>32;                 \
                                                                                         \
        /*ncc = ((((uint64_t)nc0*(0x100000000ll-rcc)+(uint64_t)nc1*rcc)>>32)             \
            + (((uint64_t)n0c*(0x100000000ll-rcc)+(uint64_t)n1c*rcc)>>32)                \
        ) >> 1;*/                                                                        \
                                                                                         \
        ncc = ((((uint64_t)n00*(0x100000000ll-rcc)+(uint64_t)n11*rcc)>>32)               \
            + (((uint64_t)n01*(0x100000000ll-rcc)+(uint64_t)n10*rcc)>>32)                \
        ) >> 1;                                                                          \
                                                                                         \
                                                                                         \
        LAB_SmoothNoiseRec##rec(smooth, noise, x0, y0, xc, yc, n00, n0c, nc0, ncc);      \
        LAB_SmoothNoiseRec##rec(smooth, noise, xc, y0, x1, yc, n0c, n01, ncc, nc1);      \
        LAB_SmoothNoiseRec##rec(smooth, noise, x0, yc, xc, y1, nc0, ncc, n10, n1c);      \
        LAB_SmoothNoiseRec##rec(smooth, noise, xc, yc, x1, y1, ncc, nc1, n1c, n11);      \
    }                                                                                    \
}

void LAB_SmoothNoiseRec0(LAB_OUT uint32_t smooth[16*16],
                         LAB_IN uint64_t noise[17*17],
                         int x0, int y0, int x1, int y1,
                         uint32_t n00, uint32_t n01,
                         uint32_t n10, uint32_t n11)
{
    LAB_UNREACHABLE();
}

DEF_NOISE_FUNC(1, 0)
DEF_NOISE_FUNC(2, 1)
DEF_NOISE_FUNC(4, 2)
DEF_NOISE_FUNC(8, 4)
DEF_NOISE_FUNC(16, 8)

LAB_HOT
void LAB_SmoothNoise2D(LAB_OUT uint32_t smooth[16*16],
                     LAB_IN uint64_t noise[17*17])
{
    uint32_t n00, n01, n10, n11;
    n00 = noise[0];
    n01 = noise[16];
    n10 = noise[16*17];
    n11 = noise[16*17+16];
    LAB_SmoothNoiseRec16(smooth, noise, 0, 0, 16, 16, n00, n01, n10, n11);
}
#else



LAB_HOT
void LAB_SmoothNoise2D(LAB_OUT uint32_t smooth[16*16],
                     LAB_IN uint64_t noise[17*17])
{
    int depth = 0;
    int x0 = 0, y0 = 0, x1 = 16, y1 = 16;
    typedef struct Elem
    {
        uint32_t n00, n01, n10, n11;

        uint32_t nc0, nc1, n1c, pad; // same level
    } Elem;
    Elem stack[5];

    stack[0].n00 = noise[0];
    stack[0].n01 = noise[16];
    stack[0].n10 = noise[16*17];
    stack[0].n11 = noise[16*17+16];

    int recurr = 1;
    do
    {
        Elem* t = &stack[depth];

        if(x0+1 == x1) // depth == 4
        {
            smooth[x0|y0<<4] = ((uint64_t)t->n00+(uint64_t)t->n01+(uint64_t)t->n10+(uint64_t)t->n11) / 4;

            #if 1
            goto sidewards;
            #else
            smooth[(x0+1)|y0<<4] = smooth[x0|(y0+1)<<4] = smooth[(x0+1)|(y0+1)<<4] = smooth[x0|y0<<4];
            //goto downwards;
            x0++; x1++; y0++; y1++;
            goto upwards;
            #endif
        }
        else
        {
            if(recurr)
                goto downwards;
            else
                goto sidewards;
        }


        downwards:
        {
            depth++;



            int xc, yc;
            uint64_t r0c, rc0, rcc, rc1, r1c;
            uint32_t n0c, nc0, ncc, nc1, n1c;

            xc = (x0+x1) >> 1;
            yc = (y0+y1) >> 1;

            r0c = noise[xc+17*y0] & 0xffffffff;
            rc0 = noise[x0+17*yc] & 0xffffffff;
            rcc = noise[xc+17*yc] & 0xffffffff;
            rc1 = noise[x1+17*yc] & 0xffffffff;
            r1c = noise[xc+17*y1] & 0xffffffff;

            n0c = ((uint64_t)t->n00*(0x100000000ll-r0c)+(uint64_t)t->n01*r0c)>>32;
            nc0 = ((uint64_t)t->n00*(0x100000000ll-rc0)+(uint64_t)t->n10*rc0)>>32;

            nc1 = ((uint64_t)t->n01*(0x100000000ll-rc1)+(uint64_t)t->n11*rc1)>>32;
            n1c = ((uint64_t)t->n10*(0x100000000ll-r1c)+(uint64_t)t->n11*r1c)>>32;

            ncc = ((((uint64_t)t->n00*(0x100000000ll-rcc)+(uint64_t)t->n11*rcc)>>32)
                + (((uint64_t)t->n01*(0x100000000ll-rcc)+(uint64_t)t->n10*rcc)>>32)
            ) >> 1;



            Elem* d = &stack[depth];
            d->n00 = t->n00;
            d->n01 = n0c;
            d->n10 = nc0;
            d->n11 = ncc;

            d->nc0 = nc0;
            d->nc1 = nc1;
            d->n1c = n1c;

            x1 = xc;
            y1 = yc;
        } continue;



        sidewards:
        {
            int w = 16 >> depth;
            int xi = x0 & w;
            int yi = y0 & w;

            if(xi && yi) goto upwards;

            Elem* p = &stack[depth-1];
            if(!xi && !yi)
            {
                // move right
                x0 |= w; // +=
                x1 += w;

                t->n00 = t->n01;
                t->n01 = p->n01;
                t->n10 = t->n11;
                t->n11 = t->nc1;
            }
            if(xi)
            {
                // move left down
                x0 &=~w; // -=
                x1 -= w;
                y0 |= w; // +=
                y1 += w;

                t->n00 = t->nc0;
                t->n01 = t->n10;
                t->n10 = p->n10;
                t->n11 = t->n1c;
            }
            if(yi)
            {
                // move right
                x0 |= w; // -=
                x1 += w;

                t->n00 = t->n01;
                t->n01 = t->nc1;
                t->n10 = t->n1c;
                t->n11 = p->n11;
            }
            recurr = 1;
        } continue;

        upwards:
        {
            // upwards
            --depth;
            recurr = 0;
            x0 &= x0-1;
            y0 &= y0-1;
            if(depth == 0) break;
        } continue;
    } while(/*depth > 0 -- see 2 lines above*/1);
}
#endif










#if 1
LAB_HOT
void LAB_SmoothNoise3D(LAB_OUT uint32_t smooth[16*16*16],
                       LAB_IN uint64_t noise[17*17+17])
{
    int depth = 0;
    int x0 = 0, y0 = 0, z0 = 0, x1 = 32, y1 = 32, z1 = 32;
    typedef struct Elem
    {
        uint32_t n[27];
        uint32_t pad[5];
    } Elem;
    Elem stack[6];

    //memset(smooth, 0, sizeof *smooth * 16*16*16);

    stack[0].n[0]  = noise[0];
    stack[0].n[1]  = noise[16];
    stack[0].n[3]  = noise[16*17];
    stack[0].n[4]  = noise[16*17+16];

    stack[0].n[9]  = noise[16*17*17+0];
    stack[0].n[10] = noise[16*17*17+16];
    stack[0].n[12] = noise[16*17*17+16*17];
    stack[0].n[13] = noise[16*17*17+16*17+16];

    int recurr = 1;
    do
    {

        //printf("%2i, %2i, %2i  --  %2i, %2i, %2i\n", x0, y0, z0, x1, y1, z1);
        //Elem* t = &stack[depth];

        if(x0+1 == x1) // depth == 4
        {
            Elem* t = &stack[depth];

            smooth[x0|y0<<4|z0<<8] = ((uint64_t)t->n[ 0]+(uint64_t)t->n[ 2]+(uint64_t)t->n[ 6]+(uint64_t)t->n[ 8]
                                   +  (uint64_t)t->n[18]+(uint64_t)t->n[20]+(uint64_t)t->n[24]+(uint64_t)t->n[26]) / 8u;
            //smooth[x0+16*y0+256*z0] ^= 0x123456789abcdef;
            goto sidewards;
        }
        else
        {
            if(recurr)
                goto downwards;
            else
                goto sidewards;
        }


        downwards:
        {
            depth++;

            x1 = (x0+x1)/2;
            y1 = (y0+y1)/2;
            z1 = (z0+z1)/2;

            goto calculate;
        } continue;



        sidewards:
        {

            int w = 32 >> depth;
            int xi = x0 & w;
            int yi = y0 & w;
            int zi = z0 & w;
            int ii = (!!xi)|2*(!!yi)|4*(!!zi);
            if(ii == 7) goto upwards;

            ++ii;

            x0 = (x0&~w) | w*!!(ii&1);
            y0 = (y0&~w) | w*!!(ii&2);
            z0 = (z0&~w) | w*!!(ii&4);
            x1 = x0 + w;
            y1 = y0 + w;
            z1 = z0 + w;

            //recurr = 1;

            goto calculate;
        } continue;

        calculate:
        {
            int w = 32 >> depth;
            //int xs = x0 & w;
            //int ys = y0 & w;
            //int zs = z0 & w;

            int xi = !!(x0 & w);
            int yi = !!(y0 & w);
            int zi = !!(z0 & w);
            int ni = xi+3*yi+9*zi;

            Elem* d = &stack[depth];
            Elem* p = &stack[depth-1];

            uint32_t* pn = p->n+ni;
            for(int z = 0; z < 3; ++z)
            for(int y = 0; y < 3; ++y)
            for(int x = 0; x < 3; ++x)
            {
                uint64_t n;
                //unsigned indx = xs+ww*x+17*(ys+ww*y)+17*17*(zs+ww*z);
                unsigned indx =        x0 + (w*x)/2
                              +    17*(y0 + (w*y)/2)
                              + 17*17*(z0 + (w*z)/2);
                uint64_t r = noise[indx]&0xffffffffull;
                //r += 0x80000000ull;
                //r>>=1;
                #define XXX(x,y,z) (uint64_t)(pn[     (x)/2  \
                                                 + 3*((y)/2) \
                                                 + 9*((z)/2) ])
                #define LRP(a,b,m) (( \
                                       (uint64_t)(a)*(0x100000000ull-(uint64_t)(m)) \
                                     + (uint64_t)(b)*                (uint64_t)(m) \
                                   ) >> 32)
                #if 0
                uint32_t n0 = LRP(XXX(x,y,z),XXX(x+1,y+1,z+1), r);
                /*uint32_t n1 = LRP(XXX(x+1,y,z),XXX(x,y+1,z+1), r);
                uint32_t n2 = LRP(XXX(x,y+1,z),XXX(x+1,y,z+1), r);
                uint32_t n3 = LRP(XXX(x,y,z+1),XXX(x+1,y+1,z), r);*/

                //n = (XXX(x,y,z)+XXX(x+1,y+1,z+1))/2;
                n = (uint64_t)n0;
                #elif 0
                n = ((uint64_t)XXX(x,   y,   z) + XXX(x,   y,   z+1)
                             + XXX(x+1, y,   z) + XXX(x+1, y,   z+1)
                             + XXX(x,   y+1, z) + XXX(x,   y+1, z+1)
                             + XXX(x+1, y+1, z) + XXX(x+1, y+1, z+1)) / 8;
                #else
                int cc = (x&1)+(y&1)+(z&1);
                n=0;
                if(x&1)
                    n += LRP(XXX(  x,   y,   z),XXX(1+x,   y,   z), r)
                       + LRP(XXX(  x,   y, 1+z),XXX(1+x,   y, 1+z), r)
                       + LRP(XXX(  x, 1+y,   z),XXX(1+x, 1+y,   z), r)
                       + LRP(XXX(  x, 1+y, 1+z),XXX(1+x, 1+y, 1+z), r);
                if(y&1)
                    n += LRP(XXX(  x,   y,   z),XXX(  x, 1+y,   z), r)
                       + LRP(XXX(1+x,   y,   z),XXX(1+x, 1+y,   z), r)
                       + LRP(XXX(  x,   y, 1+z),XXX(  x, 1+y, 1+z), r)
                       + LRP(XXX(1+x,   y, 1+z),XXX(1+x, 1+y, 1+z), r);
                if(z&1)
                    n += LRP(XXX(  x,   y,   z),XXX(  x,   y, 1+z), r)
                       + LRP(XXX(  x, 1+y,   z),XXX(  x, 1+y, 1+z), r)
                       + LRP(XXX(1+x,   y,   z),XXX(1+x,   y, 1+z), r)
                       + LRP(XXX(1+x, 1+y,   z),XXX(1+x, 1+y, 1+z), r);
                if(cc == 0) n = XXX(x, y, z);
                else switch(cc)
                {
                    case 1: n /= 4u*1u; break;
                    case 2: n /= 4u*2u; break;
                    case 3: n /= 4u*3u; break;
                    default: LAB_UNREACHABLE();
                }
                #endif
                d->n[x+3*y+9*z] = n;
            }
            recurr = 1;
        } continue;

        upwards:
        {
            // upwards
            --depth;
            recurr = 0;
            x0 &= x0-1;
            y0 &= y0-1;
            z0 &= z0-1;
            if(depth == 1) break;
        } continue;
    } while(/*depth > 0 -- see 2 lines above*/1);
}
#elif 0
LAB_HOT
void LAB_SmoothNoise3D(LAB_OUT uint32_t smooth[16*16*16],
                       LAB_IN uint64_t noise[17*17+17])
{
    for(int z = 0; z < 16; ++z)
    for(int y = 0; y < 16; ++y)
    for(int x = 0; x < 16; ++x)
    {
        int xx, yy, zz;
        xx = x*16/15;
        yy = y*16/15;
        zz = z*16/15;
        smooth[x+16*y+256*z] = noise[xx+17*yy+17*17*zz];
    }

}
#else
LAB_HOT
void LAB_SmoothNoise3D(LAB_OUT uint32_t smooth[16*16*16],
                       LAB_IN uint64_t noise[17*17+17])
{
    for(uint64_t z = 0; z < 16; ++z)
    for(uint64_t y = 0; y < 16; ++y)
    for(uint64_t x = 0; x < 16; ++x)
    {
        uint64_t n00, n01, n10, n11;

        #define c(x) ((uint64_t)(x)&0xffffffffull)

        n00 = ((16u-x)*c(noise[0]             ) + x*c(noise[16]               ))/16u;
        n01 = ((16u-x)*c(noise[16*17]         ) + x*c(noise[16*17+16]         ))/16u;
        n10 = ((16u-x)*c(noise[16*17*17+0]    ) + x*c(noise[16*17*17+16]      ))/16u;
        n11 = ((16u-x)*c(noise[16*17*17+16*17]) + x*c(noise[16*17*17+16*17+16]))/16u;

        uint64_t n0, n1;
        n0 = ((16u-y)*c(n00) +  y*c(n01))/16u;
        n1 = ((16u-y)*c(n10) +  y*c(n11))/16u;

        smooth[x+16*y+256*z] = ((16u-z)*c(n0) + z*c(n1))/16u;
    }

}
#endif
