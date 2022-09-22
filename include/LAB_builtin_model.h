#pragma once

#include "LAB_model.h"
#include "LAB_aabb.h"
#include <stdbool.h>



extern const LAB_Color LAB_cube_color_shade[6];
extern const LAB_Color LAB_cube_color_flat[6];
extern const float LAB_full_aabb[2][3];


bool LAB_Builtin_AddQuad(LAB_Model* m,
                         const float v[4][5],
                         LAB_Color c,
                         uint64_t cull, uint64_t light, uint64_t vis);

bool LAB_Builtin_ModelAddCube(LAB_Model* m,
                              const float aabb[2][3], const float tex[6][2][2],
                              const LAB_Color cs[6], LAB_Color c);

                              
bool LAB_Builtin_ModelAddCubeAll(LAB_Model* m,
                                 const float aabb[2][3], const float tex[2][2],
                                 const LAB_Color cs[6], LAB_Color c);
                              

bool LAB_Builtin_ModelAddCubeInverted(LAB_Model* m,
                                      const float aabb[2][3], const float tex[6][2][2],
                                      const LAB_Color cs[6], LAB_Color c);

bool LAB_Builtin_ModelAddCubeInvertedAll(LAB_Model* m,
                                         const float aabb[2][3], const float tex[2][2],
                                        const LAB_Color cs[6], LAB_Color c);

extern const float LAB_cross_aabb[2][3];

bool LAB_Builtin_ModelAddCross(LAB_Model* m,
                               const float aabb[2][3], const float tex[2][2],
                               LAB_Color c);