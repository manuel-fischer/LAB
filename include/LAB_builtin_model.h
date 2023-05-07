#pragma once

#include "LAB_model.h"
#include <stdbool.h>


typedef struct LAB_BoxTextures { LAB_Box2F a[6]; } LAB_BoxTextures;
typedef struct LAB_BoxColors { LAB_Color sides[6]; } LAB_BoxColors;
typedef struct LAB_BoxCull { uint64_t sides[6]; } LAB_BoxCull;

extern const LAB_BoxColors LAB_box_color_shade;
extern const LAB_BoxColors LAB_box_color_flat;


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

LAB_INLINE
LAB_BoxTextures LAB_BoxTextures_Map(LAB_BoxTextures box, LAB_BoxTextures fraction)
{
    LAB_BoxTextures new;
    for(int i = 0; i < 6; ++i) new.a[i] = LAB_Box2F_Map(box.a[i], fraction.a[i]);
    return new;
}








bool LAB_Builtin_AddQuad(LAB_Model* m,
                         const float v[4][5],
                         LAB_Color c,
                         uint64_t cull, uint64_t light, uint64_t vis);

bool LAB_Builtin_ModelAddCube(LAB_Model* m,
                              LAB_Box3F aabb, LAB_BoxTextures tex,
                              LAB_BoxColors colors);


bool LAB_Builtin_ModelAddCubeAll(LAB_Model* m,
                                 LAB_Box3F aabb, LAB_Box2F tex,
                                 LAB_BoxColors colors);


bool LAB_Builtin_ModelAddCubeInverted(LAB_Model* m,
                                      LAB_Box3F aabb, LAB_BoxTextures tex,
                                      LAB_BoxColors colors);

bool LAB_Builtin_ModelAddCubeInvertedAll(LAB_Model* m,
                                         LAB_Box3F aabb, LAB_Box2F tex,
                                         LAB_BoxColors colors);


bool LAB_Builtin_ModelAddCross(LAB_Model* m,
                               LAB_Box3F aabb, LAB_Box2F tex,
                               LAB_Color c);