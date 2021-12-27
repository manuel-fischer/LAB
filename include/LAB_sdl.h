#pragma once

// TODO: use LAB_SDL_, LAB_IMG_, etc namespaces to define debugged aliases
//  eg. IMG_Load -> LAB_IMG_Load

#ifndef NDEBUG
extern int LAB_sdl_debug_alloc_count;

#include "LAB_debug.h"

#define LAB_SDL_DEBUG_ALLOC_1() do \
{ \
    LAB_sdl_debug_alloc_count++; \
} while(0)
#define LAB_SDL_DEBUG_FREE_1() do \
{ \
    LAB_sdl_debug_alloc_count--; \
    LAB_ASSUME(LAB_sdl_debug_alloc_count>=0); \
} while(0)

#define LAB_SDL_DEBUG_ALLOC(obj) do \
{ \
    /*LAB_ASSUME_OR_WARN(*(obj));*/ \
    if(*(obj)) LAB_sdl_debug_alloc_count++; \
} while(0)
#define LAB_SDL_DEBUG_FREE(obj) do \
{ \
    if(*(obj)) LAB_sdl_debug_alloc_count--; \
    LAB_ASSUME(LAB_sdl_debug_alloc_count>=0); \
} while(0)
#else
#define LAB_SDL_DEBUG_ALLOC_1() ((void)0)
#define LAB_SDL_DEBUG_FREE_1() ((void)0)
#define LAB_SDL_DEBUG_ALLOC(obj) ((void)0)
#define LAB_SDL_DEBUG_FREE(obj) ((void)0)
#endif


#define LAB_SDL_ALLOC(func, obj, ...) do \
{ \
    *(obj) = (func)(__VA_ARGS__); \
    LAB_SDL_DEBUG_ALLOC(obj); \
} while(0)

#define LAB_SDL_FREE(func, obj) do \
{ \
    LAB_SDL_DEBUG_FREE(obj); \
    (func)(*(obj)); \
} while(0)



#define LAB_SDL_CHECK(expr) do \
{ \
    bool LAB_SDL_CHECK_cond = (expr) >= 0; \
    LAB_ASSUME2("SDL Check", LAB_SDL_CHECK_cond, #expr); \
} \
while(0)
