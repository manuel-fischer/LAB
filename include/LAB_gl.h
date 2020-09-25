#pragma once

/*#ifndef GL3_PROTOTYPES
#error  GL3_PROTOTYPES should be defined for OpenGL 3
#endif

#include <GL/gl3.h>*/
#ifndef NO_GLEW
#include <GL/glew.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glext.h>

#include <stdio.h>

#include <SDL2/SDL.h>

const char* LAB_GL_GetError(GLenum errorid);
const char* LAB_GL_GetCurrentError(void);

#define LAB_GL_CHECK() do \
{                                                                                                              \
    GLenum errorid = glGetError();                                                                             \
    if(errorid != 0)                                                                                           \
    {                                                                                                          \
        fprintf(stderr, "OpenGL Error occurred [" __FILE__ ",%i]:  %s\n", __LINE__, LAB_GL_GetError(errorid)); \
    }                                                                                                          \
} while(0)

int LAB_gl_debug_alloc_count;

#define LAB_GL_DEBUG_ALLOC(count) do { \
    LAB_gl_debug_alloc_count += (count); \
} while(0)
#define LAB_GL_DEBUG_FREE(count) do { \
    LAB_gl_debug_alloc_count -= (count); \
} while(0)


/**
 *  Returns the OpenGL type index
 */
#define LAB_GL_TYPEOF(expr)               \
    _Generic(expr,                        \
             GLbyte:   GL_BYTE,           \
             GLubyte:  GL_UNSIGNED_BYTE,  \
             GLshort:  GL_SHORT,          \
             GLushort: GL_UNSIGNED_SHORT, \
             GLint:    GL_INT,            \
             GLuint:   GL_UNSIGNED_INT,   \
             GLfloat:  GL_FLOAT,          \
             GLdouble: GL_DOUBLE)
             //GLfixed:  GL_FIXED,
             //GLhalf:   GL_HALF_FLOAT)


void LAB_GL_ActivateTexture(unsigned* gl_id);

void LAB_GL_UploadSurf(unsigned gl_id, SDL_Surface* surf);
void LAB_GL_DrawSurf(unsigned gl_id, int x, int y, int w, int h, int sw, int sh);
