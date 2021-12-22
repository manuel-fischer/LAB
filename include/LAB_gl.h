#pragma once

#ifdef __WINNT__
#include <GL/glew.h>
#define LAB_USES_GLEW
//#define GL3_PROTOTYPES
#else
#include <GLES3/gl3.h>
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
        fprintf(stderr, "OpenGL Error occurred [" __FILE__ ":%i]:  %s\n", __LINE__, LAB_GL_GetError(errorid)); \
    }                                                                                                          \
} while(0)

#ifndef NDEBUG
extern int LAB_gl_debug_alloc_count;

/*#define LAB_GL_DEBUG_ALLOC(count, array) do { \
    LAB_gl_debug_alloc_count += (count); \
} while(0)
#define LAB_GL_DEBUG_FREE(count, array) do { \
    LAB_gl_debug_alloc_count -= (count); \
} while(0)*/

#define LAB_GL_DEBUG_ALLOC_(count, array) do { \
    for(int LAB_GL_DEBUG_FREE_i = 0; LAB_GL_DEBUG_FREE_i < (count); ++LAB_GL_DEBUG_FREE_i) \
        if((array)[LAB_GL_DEBUG_FREE_i]) ++LAB_gl_debug_alloc_count; \
        else LAB_ASSUME(!"failed to allocate for " #array " in OpenGL"); \
} while(0)
#define LAB_GL_DEBUG_FREE_(count, array) do { \
    for(int LAB_GL_DEBUG_FREE_i = 0; LAB_GL_DEBUG_FREE_i < (count); ++LAB_GL_DEBUG_FREE_i) \
        if((array)[LAB_GL_DEBUG_FREE_i]) --LAB_gl_debug_alloc_count; \
} while(0)
#else
#define LAB_GL_DEBUG_ALLOC_(count, array) ((void)0)
#define LAB_GL_DEBUG_FREE_(count, array) ((void)0)
#endif
#define LAB_GL_ALLOC(function, count, array) do { \
    function(count, array); \
    LAB_GL_DEBUG_ALLOC_(count, array); \
} while(0)
#define LAB_GL_FREE(function, count, array) do { \
    LAB_GL_DEBUG_FREE_(count, array); \
    function(count, array); \
} while(0)

#ifdef NDEBUG
#define LAB_GL_REQUIRE(flag) ((void)0)
#define LAB_GL_REQUIRE_NOT(flag) ((void)0)
#else
#include "LAB_debug.h"
#define LAB_GL_REQUIRE(flag) do \
{ \
    GLboolean LAB_GL_REQUIRE_status; \
    glGetBooleanv(flag, &LAB_GL_REQUIRE_status); \
    LAB_ASSUME2("GL state check", LAB_GL_REQUIRE_status, #flag); \
} while(0)
#define LAB_GL_REQUIRE_NOT(flag) do \
{ \
    GLboolean LAB_GL_REQUIRE_status; \
    glGetBooleanv(flag, &LAB_GL_REQUIRE_status); \
    LAB_ASSUME2("GL state check", !LAB_GL_REQUIRE_status, "not " #flag); \
} while(0)
#endif

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



int LAB_GL_GetInt(GLenum e);
#define LAB_GL_GetUInt(e) (unsigned)LAB_GL_GetInt(e)



/**
 *  fix screen data fetched from opengl
 *
 *  flip image horizontally and set alpha channel to 255
 */
void LAB_GL_FixScreenImg(void* pixels, int w, int h);
