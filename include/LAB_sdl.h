#pragma once

// TODO: use LAB_SDL_, LAB_IMG_, etc namespaces to define debugged aliases
//  eg. IMG_Load -> LAB_IMG_Load

#ifndef NDEBUG
extern int LAB_sdl_debug_alloc_count;

#define LAB_SDL_DEBUG_ALLOC(obj) do \
{ \
    if(*(obj)) LAB_sdl_debug_alloc_count++; \
} while(0)
#define LAB_SDL_DEBUG_FREE(obj) do \
{ \
    if(*(obj)) LAB_sdl_debug_alloc_count--; \
} while(0)
#else
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
