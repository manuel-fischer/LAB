#include "LAB_error.h"
#include <stdio.h>
#include <stdarg.h>

#define LAB_ERROR_BUFFER_SIZE 256

static char LAB_error_buffer[LAB_ERROR_BUFFER_SIZE] = {};

const char* LAB_GetError(void)
{
    return LAB_error_buffer;
}

void LAB_SetError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf_s(LAB_error_buffer, LAB_ERROR_BUFFER_SIZE, fmt, args);
    va_end(args);
}
