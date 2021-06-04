#pragma once

#include "LAB_error.h"

// ... -- on success
#define LAB_OBJ(init, defer, ...) do \
{ \
    if(init) \
    { \
        { __VA_ARGS__ } \
        { defer; } \
    } \
    else \
    { \
        LAB_AddErrorContext(__FILE__, __LINE__, #init); \
    } \
} while(0)

// Error source: SDL_GetError
#define LAB_OBJ_SDL(init, defer, ...) do \
{ \
    if(init) \
    { \
        { __VA_ARGS__ } \
        { defer; } \
    } \
    else \
    { \
        LAB_SetError("SDL Error: %s", SDL_GetError()); \
        LAB_AddErrorContext(__FILE__, __LINE__, #init); \
    } \
} while(0)
