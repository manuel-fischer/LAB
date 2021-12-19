#pragma once

#include "LAB_error.h"

// ... -- on success
#define LAB_OBJ(init, defer, ...) do \
{ \
    if((init)) \
    { \
        { __VA_ARGS__ } \
        { defer; } \
    } \
    else \
    { \
        LAB_AddErrorContext(__FILE__, __LINE__, #init); \
    } \
} while(0)


#define LAB_OBJ_FOR(index_var, count, init, defer, ...) do \
{ \
    size_t index_var; \
    for(index_var = 0; index_var < (count); ++index_var) \
        if(!(init)) break; \
    if(index_var == (count)) { __VA_ARGS__ } \
    else \
    { \
        LAB_AddErrorContext(__FILE__, __LINE__, #init); \
    } \
    while(index_var > 0) \
    { \
        index_var--; \
        { defer; } \
    } \
} while(0)


#define LAB_OBJ_FOR2(index_var, count, init, defer1, defer, ...) do \
{ \
    size_t index_var; \
    for(index_var = 0; index_var < (count); ++index_var) \
        if(!(init)) break; \
    if(index_var == (count)) { __VA_ARGS__ } \
    else \
    { \
        LAB_AddErrorContext(__FILE__, __LINE__, #init); \
    } \
    { defer1; } \
    while(index_var > 0) \
    { \
        index_var--; \
        { defer; } \
    } \
} while(0)


// Error source: SDL_GetError
#define LAB_OBJ_SDL(init, defer, ...) do \
{ \
    if((init)) \
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
