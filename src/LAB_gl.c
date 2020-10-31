#include "LAB_gl.h"
#include "LAB_bits.h"
#include "LAB_debug.h"

#define LAB_GL_ERRORS_X(X) \
    X(GL_NO_ERROR, "No error") \
    X(GL_INVALID_ENUM, "An unacceptable value is specified for an enumerated argument") \
    X(GL_INVALID_VALUE, "A numeric argument is out of range") \
    X(GL_INVALID_OPERATION, "The specified operation is not allowed in the current state") \
    /*X(GL_INVALID_FRAMEBUFFER_OPERATION, "The framebuffer object is not complete")*/ \
    X(GL_OUT_OF_MEMORY, "There is not enough memory left to execute the command") \
    X(GL_STACK_UNDERFLOW, "An attempt has been made to perform an operation that would cause an internal stack to underflow") \
    X(GL_STACK_OVERFLOW, "An attempt has been made to perform an operation that would cause an internal stack to overflow")


int LAB_gl_debug_alloc_count = 0;

const char* LAB_GL_GetError(GLenum errorid)
{
    switch(errorid)
    {
#define X(id, description) case id: return description;
        LAB_GL_ERRORS_X(X)
#undef X
        default: return "Unknown error";
    }
}

const char* LAB_GL_GetCurrentError(void)
{
    return LAB_GL_GetError(glGetError());
}

int LAB_GL_GetInt(GLenum e)
{
    int i;
    glGetIntegerv(e, &i);
    return i;
}

void LAB_GL_ActivateTexture(unsigned* gl_id)
{
    if(*gl_id == 0)
    {
        LAB_GL_ALLOC(glGenTextures, 1, gl_id);
        glBindTexture(GL_TEXTURE_2D, *gl_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, INFO_WIDTH, INFO_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, *gl_id);
    }
}

void LAB_GL_UploadSurf(unsigned gl_id, SDL_Surface* surf)
{
    LAB_ASSUME_0(LAB_GL_GetUInt(GL_TEXTURE_BINDING_2D)==gl_id);

    int info_width  = LAB_CeilPow2(surf->w);
    int info_height = LAB_CeilPow2(surf->h);

    int free_surf = 0;
    if(surf->format->format != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_Surface* nImg;
        nImg = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
        if(nImg == NULL) return;
        surf = nImg;
        free_surf = 1;
        //printf("Conv\n");
    }

    /*for(int x = 0; x < 16; ++x)
    {
        printf("%08x\n", ((uint32_t*)surf->pixels)[x]);
    }*/

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info_width, info_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surf->w, surf->h, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    if(free_surf) SDL_FreeSurface(surf);
}

void LAB_GL_DrawSurf(unsigned gl_id, int x, int y, int w, int h, int sw, int sh)
{
    LAB_ASSUME_0(LAB_GL_GetUInt(GL_TEXTURE_BINDING_2D)==gl_id);

    int rw, rh;
    rw = LAB_CeilPow2(w);
    rh = LAB_CeilPow2(h);


    // partly const
    static float info[5*3*2] = {
          0, (0), -1,   0,   0,
        (0),   0, -1, (1), (1),
          0,   0, -1,   0, (1),
        //
          0, (0), -1,   0,   0,
        (0), (0), -1, (1),   0,
        (0),   0, -1, (1), (1),
    };

    info[5*1] = info[5*4] = info[5*5]   = w;
    info[1] = info[5*3+1] = info[5*4+1] = h;

    info[5*1+3] = info[5*4+3] = info[5*5+3] = (float)w/rw;
    info[5*1+4] = info[5*2+4] = info[5*5+4] = (float)h/rh;


    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float f = 2.f/(float)sh;
    glScalef(f, f, 1);
    glTranslatef(-(float)sw/2+x, -(float)sh/2+y, 0);

    glVertexPointer(3, LAB_GL_TYPEOF(*info), sizeof *info * 5, info);
    glTexCoordPointer(2, LAB_GL_TYPEOF(*info), sizeof *info * 5, info+3);
    glDrawArrays(GL_TRIANGLES, 0, 3*2);

    LAB_GL_CHECK();

    //glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
}



void LAB_GL_FixScreenImg(void* pixels, int w, int h)
{
    uint8_t* pix = (uint8_t*)pixels;
    int c = 4*w*(h/2);
    for(int y0 = 0, y1 = 4*w*(h-1); y0 < c; y0+=4*w, y1-=4*w)
    {
        for(int x = 0; x < 4*w; x+=4)
        {
            for(int i = 0; i < 3; ++i)
            {
                char tmp = pix[y0+x+i];
                pix[y0+x+i] = pix[y1+x+i];
                pix[y1+x+i] = tmp;
            }
            pix[y0+x+3] = 0xffu;
            pix[y1+x+3] = 0xffu;
        }
    }
    if(h&1)
    {
        int y = c;
        for(int x = 0; x < 4*w; x+=4)
        {
            pix[y+x+3] = 0xffu;
        }
    }
}
