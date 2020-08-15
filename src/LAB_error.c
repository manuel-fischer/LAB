#include "LAB_error.h"
#include <stdio.h>
#include <stdarg.h>
#include <LAB_gl.h>
#include <SDL2/SDL.h>

#define LAB_ERROR_BUFFER_SIZE 256

static char LAB_error_buffer[LAB_ERROR_BUFFER_SIZE] = {0};

const char* LAB_GetError(void)
{
    if(LAB_error_buffer[0] == '\0')
    {
        // check libraries if an error occurred
        // TODO: if an SDL Error is catched early, there is currently no code to clear the error again,
        //       so the error is printed twice
        GLenum glErr;
        const char* sdlErr;
        if((glErr = glGetError()) != GL_NO_ERROR)
        {
            LAB_SetError("An OpenGL error has occured at unknown location: %s", LAB_GL_GetError(glErr));
        }
        else if((sdlErr = SDL_GetError())[0] != '\0')
        {
            LAB_SetError("An SDL error has occured at unknown location: %s", sdlErr);
            SDL_ClearError();
        }
    }
    return LAB_error_buffer;
}

void LAB_ClearError(void)
{
    LAB_error_buffer[0] = '\0';
}

void LAB_SetError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf_s(LAB_error_buffer, LAB_ERROR_BUFFER_SIZE, fmt, args);
    va_end(args);
}
