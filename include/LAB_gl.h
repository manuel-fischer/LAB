#pragma once

#ifndef GL_GLEXT_PROTOTYPES
#error  GL_GLEXT_PROTOTYPES should be defined for OpenGL 2
#endif

/*#ifndef GL3_PROTOTYPES
#error  GL3_PROTOTYPES should be defined for OpenGL 3
#endif

#include <GL/gl3.h>*/
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glext.h>

#include <stdio.h>

const char* LAB_GL_GetError(GLenum errorid);
const char* LAB_GL_GetCurrentError();

#define LAB_GL_CHECK() do \
{                                                                                                              \
    GLenum errorid = glGetError();                                                                             \
    if(errorid != 0)                                                                                           \
    {                                                                                                          \
        fprintf(stderr, "OpenGL Error occurred [" __FILE__ ",%i]:  %s\n", __LINE__, LAB_GL_GetError(errorid)); \
    }                                                                                                          \
} while(0)
