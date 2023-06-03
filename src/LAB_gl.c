#include "LAB_gl.h"
#include "LAB_bits.h"
#include "LAB_sdl.h"

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
        //LAB_ASSERT_FALSE("debug");
        __asm__ ("nop");
    }
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


bool LAB_GL_Texture_Create(LAB_GL_Texture* tex)
{
    if(!LAB_GL_OBJ_ALLOC_TARGET(glCreateTextures, GL_TEXTURE_2D, tex)) return false;

    LAB_GL_CHECK();
    glTextureParameteri(tex->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    LAB_GL_CHECK();
    glTextureParameteri(tex->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    LAB_GL_CHECK();

    return true;
}


bool LAB_GL_Texture_CreateSized(LAB_GL_Texture* tex, LAB_Vec2Z size)
{
    if(!LAB_GL_Texture_Create(tex)) return false;

    LAB_GL_Texture_ResizeCeilPow2(*tex, size);

    return true;
}

bool LAB_GL_Texture_CreateFromSurface(LAB_GL_Texture* tex, SDL_Surface* surf)
{
    if(!LAB_GL_Texture_CreateSized(tex, (LAB_Vec2Z) {surf->w, surf->h})) return false;

    if(!LAB_GL_Texture_Upload(*tex, surf))
    {
        LAB_GL_OBJ_FREE(glDeleteTextures, tex);
        return false;
    }

    return true;
}

void LAB_GL_Texture_ResizeCeilPow2(LAB_GL_Texture tex, LAB_Vec2Z size)
{
    LAB_ASSERT_LONG(glIsTexture(tex.id));

    size_t width  = LAB_CeilPow2(size.x);
    size_t height = LAB_CeilPow2(size.y);
    LAB_GL_CHECK();
    glTextureStorage2D(tex.id, /*levels*/1, GL_RGBA8, width, height);
    LAB_GL_CHECK();

    //int w=-1, h=-1;
    //glGetTextureLevelParameteriv(tex.id, 0, GL_TEXTURE_WIDTH, &w);
    //glGetTextureLevelParameteriv(tex.id, 0, GL_TEXTURE_HEIGHT, &h);
    //LAB_ASSERT_FMT(w == width && h == height, "w=%i, h=%i, width=%i, height=%i", w, h, width, height);
}


LAB_STATIC
void LAB_GL_Texture_UploadRGBA32(LAB_GL_Texture tex, SDL_Surface* surf)
{
    LAB_ASSERT_LONG(glIsTexture(tex.id));

    //int w=-1, h=-1;
    //glGetTextureLevelParameteriv(tex.id, 0, GL_TEXTURE_WIDTH, &w);
    //glGetTextureLevelParameteriv(tex.id, 0, GL_TEXTURE_HEIGHT, &h);
    //LAB_DBG_PRINTF("upload %ix%i surface @%p to %ix%i texture @%p\n", surf->w, surf->h, (void*)surf, w, h, (void*)tex.id);

    LAB_GL_CHECK();
    glTextureSubImage2D(tex.id, 0, 0, 0, surf->w, surf->h, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    LAB_GL_CHECK();
}

bool LAB_GL_Texture_Upload(LAB_GL_Texture tex, SDL_Surface* surf)
{
    if(surf->format->format != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_Surface* rgba_surf;
        LAB_SDL_ALLOC(SDL_ConvertSurfaceFormat, &rgba_surf, surf, SDL_PIXELFORMAT_RGBA32, 0);
        if(rgba_surf == NULL) return false;
        LAB_GL_Texture_UploadRGBA32(tex, rgba_surf);
        LAB_SDL_FREE(SDL_FreeSurface, &rgba_surf);
    }
    else
    {
        LAB_GL_Texture_UploadRGBA32(tex, surf);
    }
    return true;
}

bool LAB_GL_Texture_ResizeUpload(LAB_GL_Texture* tex, LAB_Vec2Z* tex_size, SDL_Surface* surf)
{
    LAB_Vec2Z surf_size = { surf->w, surf->h };
    LAB_Vec2Z surf_size2 = { LAB_CeilPow2(surf->w), LAB_CeilPow2(surf->h) };
    if(!LAB_Vec2Z_Equals(*tex_size, surf_size2))
    {
        if(tex->id == 0 || !LAB_Vec2Z_Equals(*tex_size, (LAB_Vec2Z){0, 0}))
        {
            LAB_GL_OBJ_FREE(glDeleteTextures, tex);
            LAB_GL_Texture_Create(tex);
        }

        *tex_size = surf_size2;

        LAB_GL_Texture_ResizeCeilPow2(*tex, surf_size);
    }
    return LAB_GL_Texture_Upload(*tex, surf);
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
