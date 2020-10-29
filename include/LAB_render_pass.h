#pragma once

#include "LAB_attr.h"

// Render passes         ALPHA_TEST           BLEND            MIPMAPPING       CHUNK_ORDER    CHUNK_SORTED  DEPTH_MASK  DEPTH_TEST  DEPTH_OFFSET  IMPLEMENTED
// RENDER_PASS_SOLID         no                 no                yes             forward           no           yes        yes           no           yes
// RENDER_PASS_MASKED      >=32                 no                yes             forward           no           yes        yes           no           yes
// RENDER_PASS_BLIT        >=32                 no                 no             forward           no           yes        yes           no           yes
// -- QUERIES --
// RENDER_PASS_OVERLAY     >= 1       src*src.a+dst*(1-src.a)     yes             backward          no           yes        yes          yes            no
// RENDER_PASS_ALPHA       >= 1       src*src.a+dst*(1-src.a)     yes             backward         yes           yes        yes           no           yes
// RENDER_PASS_MULT2       >= 1       src*dst  +dst*src           yes            don't care         no            no        yes           no            no
// RENDER_PASS_ADD         >= 1       src*1    +dst*1             yes            don't care         no            no        yes           no            no



typedef int LAB_RenderPass;
enum
{
    LAB_RENDER_PASS_SOLID = 0,
    LAB_RENDER_PASS_MASKED,
    LAB_RENDER_PASS_BLIT,
    LAB_RENDER_PASS_ALPHA,
    // LAB_RENDER_PASS_MULT2,
    // LAB_RENDER_PASS_ADD,
    LAB_RENDER_PASS_COUNT
};

#include "LAB_gl.h"

// Assumption: for pass > SOLID: LAB_PrepareRenderPass called for pass-1
// ASSUMPTION: LAB_PrepareRenderPass always called 6 times with increasing pass from 0 to 5
// ASSUMPTION: terrain texture enabled
// Return if chunks are rendered backwards and sorted
LAB_INLINE bool LAB_PrepareRenderPass(LAB_RenderPass pass)
{
    #if 0 // DBG
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // REENABLE MIPMAPPING
            glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GEQUAL, 1/255.f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
        return 1; //pass == LAB_RENDER_PASS_ALPHA;
    #else
    switch(pass)
    {
        case LAB_RENDER_PASS_SOLID:
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_BLEND);
            return 0;

        case LAB_RENDER_PASS_MASKED:
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GEQUAL, 32/255.f);
            return 0;

        case LAB_RENDER_PASS_BLIT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // DISABLE MIPMAPPING
            return 0;

        case LAB_RENDER_PASS_ALPHA:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // REENABLE MIPMAPPING
            glAlphaFunc(GL_GEQUAL, 1/255.f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            return 1; // SORT and render backwards

        //...
        default:
            LAB_UNREACHABLE();
    }
    #endif
}
