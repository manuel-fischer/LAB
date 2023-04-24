#define LAB_GL_NO_POISON
#include "LAB_gl.h"
#include "LAB_bits.h"
#include "LAB_debug.h"
#include "LAB_sdl.h"
#include "LAB_color_hdr.h"

#define LAB_GL_ERRORS_X(X) \
    X(GL_NO_ERROR, "No error") \
    X(GL_INVALID_ENUM, "An unacceptable value is specified for an enumerated argument") \
    X(GL_INVALID_VALUE, "A numeric argument is out of range") \
    X(GL_INVALID_OPERATION, "The specified operation is not allowed in the current state") \
    /*X(GL_INVALID_FRAMEBUFFER_OPERATION, "The framebuffer object is not complete")*/ \
    X(GL_OUT_OF_MEMORY, "There is not enough memory left to execute the command") \
    X(GL_STACK_UNDERFLOW, "An attempt has been made to perform an operation that would cause an internal stack to underflow") \
    X(GL_STACK_OVERFLOW, "An attempt has been made to perform an operation that would cause an internal stack to overflow")


#ifndef NDEBUG
int LAB_gl_debug_alloc_count = 0;
#endif

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

#ifndef NDEBUG
static const char* last_file;
static int last_line;
static bool had_debug_message;
static GLenum ignore_id;


LAB_STATIC void LAB_GL_HandleDebugMessage(GLenum source, GLenum type, GLuint id,
   GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if(ignore_id == id) return;

    const char* s_severity;
    const char* s_type;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR: s_type = "ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: s_type = "DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: s_type = "UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY: s_type = "PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: s_type = "PERFORMANCE"; break;
        case GL_DEBUG_TYPE_OTHER: s_type = "OTHER"; break;
        default: s_type = "UNKNOWN TYPE"; break;
    }


    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: s_severity = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: s_severity = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW: s_severity = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: s_severity = "NOTF"; break;
        default: s_severity = "UNKNOWN SEVERITY"; break;
    }

    fprintf(stderr, "GL[%s/%s/%i]: %s\n", s_severity, s_type, id, message);
    if(last_file != NULL)
    {
        fprintf(stderr, "Last Checkpoint: %s:%i\n", last_file, last_line);
    }

    had_debug_message = true;
}

void LAB_GL_CheckClear(void)
{
    last_file = NULL;
    had_debug_message = false;
}


void LAB_GL_IgnoreInfo(GLenum id)
{
    ignore_id = id;
}

void LAB_GL_ResetIgnoreInfo(void)
{
    ignore_id = 0;
}

void LAB_GL_SetupDebug(void)
{
    glDebugMessageCallback(LAB_GL_HandleDebugMessage, NULL);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
#endif


void LAB_GL_Check(const char* file, int line)
{
    #ifndef NDEBUG
    if(had_debug_message)
    {
        fprintf(stderr, "Next Checkpoint: %s:%i\n", file, line);
        had_debug_message = false;
    }

    last_file = file;
    last_line = line;
    #endif

    GLenum errorid = glGetError();
    if(errorid != 0)
    {
        fprintf(stderr, "OpenGL Error occurred [%s:%i]:  %s\n", file, line, LAB_GL_GetError(errorid));
       // LAB_ASSERT_FALSE("debug");
    }
}



// TODO: replace legacy matrix function calls
void LAB_GL_SetMatrix(GLenum mode, LAB_Mat4F matrix)
{
    glMatrixMode(mode);
    glLoadMatrixf(LAB_Mat4F_AsCArray(&matrix));

    LAB_GL_CHECK();
}


void LAB_GL_SetMatrix_Identity(GLenum mode)
{
    glMatrixMode(mode);
    glLoadIdentity();

    LAB_GL_CHECK();
}


void LAB_GL_UniformColor(LAB_GL_Uniform uniform, LAB_Color color)
{
    glUniform4f(uniform.id,
        (float)LAB_RED(color)/255.f,
        (float)LAB_GRN(color)/255.f,
        (float)LAB_BLU(color)/255.f,
        (float)LAB_ALP(color)/255.f);
}

void LAB_GL_UniformColorHDR(LAB_GL_Uniform uniform, LAB_ColorHDR color)
{
    glUniform4f(uniform.id,
        (float)LAB_HDR_RED_VAL(color),
        (float)LAB_HDR_GRN_VAL(color),
        (float)LAB_HDR_BLU_VAL(color),
        1.f);
}


int LAB_GL_GetInt(GLenum e)
{
    int i;
    glGetIntegerv(e, &i);
    return i;
}

void LAB_GL_ActivateTexture(LAB_GL_Texture* tex)
{
    if(tex->id == 0)
    {
        LAB_GL_ALLOC(glGenTextures, 1, &tex->id);
        glBindTexture(GL_TEXTURE_2D, tex->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, INFO_WIDTH, INFO_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, tex->id);
    }
}

void LAB_GL_UploadSurf(LAB_GL_Texture tex, SDL_Surface* surf)
{
    LAB_ASSUME_0(LAB_GL_GetUInt(GL_TEXTURE_BINDING_2D)==tex.id);

    int info_width  = LAB_CeilPow2(surf->w);
    int info_height = LAB_CeilPow2(surf->h);

    int free_surf = 0;
    if(surf->format->format != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_Surface* nImg;
        LAB_SDL_ALLOC(SDL_ConvertSurfaceFormat, &nImg, surf, SDL_PIXELFORMAT_RGBA32, 0);
        if(nImg == NULL) return;
        surf = nImg;
        free_surf = 1;
    }

    LAB_GL_CHECK();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info_width, info_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    LAB_GL_CHECK();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surf->w, surf->h, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    LAB_GL_CHECK();
    if(free_surf) LAB_SDL_FREE(SDL_FreeSurface, &surf);
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
