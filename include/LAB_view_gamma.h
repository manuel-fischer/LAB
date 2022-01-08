#pragma once

#include "LAB_stdinc.h"
#include "LAB_color.h"

typedef struct LAB_View_GammaMap
{
    uint8_t map[3][256];
} LAB_View_GammaMap;

extern const LAB_View_GammaMap LAB_gamma_dark;
extern const LAB_View_GammaMap LAB_gamma_light;


LAB_INLINE
LAB_POINTER_CONST
LAB_Color LAB_View_GammaMap_MapColor(const LAB_View_GammaMap* map, LAB_Color color)
{
    return LAB_RGBA(map->map[0][LAB_RED(color)],
                    map->map[1][LAB_GRN(color)],
                    map->map[2][LAB_BLU(color)],
                    LAB_ALP(color));
}