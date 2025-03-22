#include "LAB_error.h"

#include "LAB_gl.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#define LAB_ERROR_BUFFER_SIZE 1024

// TODO: thread local
static char LAB_error_buffer[LAB_ERROR_BUFFER_SIZE] = {0};

static void LAB_LoadError()
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
            LAB_SetError("An OpenGL error has occured: %s", LAB_GL_GetError(glErr));
        }
        else if((sdlErr = SDL_GetError())[0] != '\0')
        {
            LAB_SetError("An SDL error has occured: %s", sdlErr);
            SDL_ClearError();
        }
    }
}

void LAB_SetSDLError(void)
{
    LAB_SetError("SDL error: %s", SDL_GetError());
    SDL_ClearError();
}


const char* LAB_GetError(void)
{
    LAB_LoadError();
    return LAB_error_buffer;
}

void LAB_ClearError(void)
{
    LAB_error_buffer[0] = '\0';
}

void LAB_SetError(const char* fmt, ...)
{
    size_t pos = strlen(LAB_error_buffer);
    if(pos != 0)
        pos += snprintf(LAB_error_buffer+pos, LAB_ERROR_BUFFER_SIZE-pos, "\nDuring this Error, another error occurred:\n");

    va_list args;
    va_start(args, fmt);
    vsnprintf(LAB_error_buffer+pos, LAB_ERROR_BUFFER_SIZE-pos, fmt, args);
    va_end(args);
}

void LAB_VAppendError(bool prepend_newline, const char* fmt, va_list args)
{
    if(LAB_error_buffer[0] == '\0')
    {
//        LAB_LoadError();
        if(LAB_error_buffer[0] == '\0')
            strcpy(LAB_error_buffer, "Unknown Error");
    }
    size_t slen = strlen(LAB_error_buffer);
    if(prepend_newline && slen < LAB_ERROR_BUFFER_SIZE-1)
    {
        LAB_error_buffer[slen++] = '\n';
        LAB_error_buffer[slen  ] = '\0';
    }

    if(slen < LAB_ERROR_BUFFER_SIZE-1)
    {
        vsnprintf(LAB_error_buffer+slen, LAB_ERROR_BUFFER_SIZE-slen, fmt, args);
    }
}

void LAB_AppendError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LAB_VAppendError(true, fmt, args);
    va_end(args);
}


void LAB_AddErrorContext(const char* filename, int line, const char* expression)
{
    if(expression == NULL)
        LAB_AppendError("    at %s:%d", filename, line);
    else
        LAB_AppendError("    at %s:%d:\n        %s", filename, line, expression);
}

void LAB_AddErrorContextFmt(const char* filename, int line, const char* fmt, ...)
{
    LAB_AppendError("    at %s:%d:\n        ", filename, line);

    va_list args;
    va_start(args, fmt);
    LAB_VAppendError(false, fmt, args);
    va_end(args);
}