#pragma once
#include <GL/gl.h>
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
