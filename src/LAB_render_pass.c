#include "LAB_render_pass.h"

#include "LAB_gl.h"

// Assumption: for pass > SOLID: LAB_PrepareRenderPass called for pass-1
// ASSUMPTION: LAB_PrepareRenderPass always called 6 times with increasing pass from 0 to 5
// ASSUMPTION: terrain texture enabled
// Return if chunks are rendered backwards and sorted
bool LAB_PrepareRenderPass(LAB_RenderPass pass)
{
    #if LAB_DBG_SEETHROUGH_RENDER // DBG
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
            // GL_ALPHA_TEST: shader does not discard fragments
            LAB_GL_CHECK();
            glDisable(GL_BLEND);
            LAB_GL_CHECK();
            return 0;

        case LAB_RENDER_PASS_MASKED:
            // GL_ALPHA_TEST: shader discards fragments below 32./255.
            return 0;

        case LAB_RENDER_PASS_BLIT:
            LAB_GL_CHECK();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // DISABLE MIPMAPPING
            LAB_GL_CHECK();
            return 0;

        case LAB_RENDER_PASS_ALPHA:
            // GL_ALPHA_TEST: shader discards fragments below 1./255.
            LAB_GL_CHECK();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); // REENABLE MIPMAPPING
            LAB_GL_CHECK();
            glEnable(GL_BLEND);
            LAB_GL_CHECK();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            LAB_GL_CHECK();
            return 1; // SORT and render backwards

        //...
        default:
            LAB_UNREACHABLE();
    }
    #endif
}
