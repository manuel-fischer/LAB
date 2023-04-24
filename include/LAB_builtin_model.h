#pragma once

#include "LAB_model.h"
#include "LAB_aabb.h"
#include <stdbool.h>


typedef struct LAB_BoxTextures { LAB_Box2F a[6]; } LAB_BoxTextures;
typedef struct LAB_BoxColors { LAB_Color sides[6]; } LAB_BoxColors;
typedef struct LAB_BoxCull { uint64_t sides[6]; } LAB_BoxCull;

extern const LAB_BoxColors LAB_box_color_shade;
extern const LAB_BoxColors LAB_box_color_flat;

extern const float LAB_full_aabb[2][3];


LAB_INLINE LAB_VALUE_CONST
LAB_BoxColors LAB_BoxColors_Tint(LAB_BoxColors colors, LAB_Color c)
{
    LAB_BoxColors new;
    for(int i = 0; i < 6; ++i) new.sides[i] = LAB_MulColor(colors.sides[i], c);
    return new;
}

LAB_INLINE LAB_VALUE_CONST
LAB_BoxColors LAB_BoxColors_Flat(LAB_Color c)
{
    return (LAB_BoxColors) {{ c, c, c, c, c, c }};
}

LAB_INLINE LAB_VALUE_CONST
LAB_BoxColors LAB_BoxColors_Shaded(LAB_Color c)
{
    return LAB_BoxColors_Tint(LAB_box_color_shade, c);
}


LAB_INLINE
LAB_BoxTextures LAB_BoxTextures_All(LAB_Box2F b)
{
    return (LAB_BoxTextures) {{ b, b, b, b, b, b }};
}








bool LAB_Builtin_AddQuad(LAB_Model* m,
                         const float v[4][5],
                         LAB_Color c,
                         uint64_t cull, uint64_t light, uint64_t vis);

bool LAB_Builtin_ModelAddCube(LAB_Model* m,
                              const float aabb[2][3], const float tex[6][2][2],
                              LAB_BoxColors colors);


bool LAB_Builtin_ModelAddCubeAll(LAB_Model* m,
                                 const float aabb[2][3], const float tex[2][2],
                                 LAB_BoxColors colors);


bool LAB_Builtin_ModelAddCubeInverted(LAB_Model* m,
                                      const float aabb[2][3], const float tex[6][2][2],
                                      LAB_BoxColors colors);

bool LAB_Builtin_ModelAddCubeInvertedAll(LAB_Model* m,
                                         const float aabb[2][3], const float tex[2][2],
                                         LAB_BoxColors colors);

extern const float LAB_cross_aabb[2][3];

bool LAB_Builtin_ModelAddCross(LAB_Model* m,
                               const float aabb[2][3], const float tex[2][2],
                               LAB_Color c);