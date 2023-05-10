#pragma once

#include "LAB_vec2.h"
#include "SDL2/SDL_rect.h"

LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST
SDL_Point LAB_Vec2I2SDL(LAB_Vec2I a)
{
    SDL_Point p;
    LAB_Vec2I_Unpack(&p.x, &p.y, a);
    return p;
}

LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST
SDL_FPoint LAB_Vec2F2SDL(LAB_Vec2F a)
{
    SDL_FPoint p;
    LAB_Vec2F_Unpack(&p.x, &p.y, a);
    return p;
}


LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST
SDL_Rect LAB_Box2I2SDL(LAB_Box2I a)
{
    SDL_Rect r;
    LAB_Box2I_Unpack_Sized(&r.x, &r.y, &r.w, &r.h, a);
    return r;
}

LAB_INLINE LAB_ALWAYS_INLINE LAB_VALUE_CONST
SDL_FRect LAB_Box2F2SDL(LAB_Box2F a)
{
    SDL_FRect r;
    LAB_Box2F_Unpack_Sized(&r.x, &r.y, &r.w, &r.h, a);
    return r;
}

