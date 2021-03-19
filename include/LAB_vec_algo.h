#pragma once

#include <stdbool.h>

#include "LAB_vec.h"
#include "LAB_opt.h" // -> LAB_IN, LAB_OUT

LAB_INLINE
bool LAB_Vec3Algo_RayVsRect(LAB_OUT float collision_point[LAB_RESTRICT 3],
                            LAB_OUT LAB_NONULL float* LAB_RESTRICT collision_steps,
                            LAB_OUT LAB_NONULL int*   LAB_RESTRICT collision_face,
                            LAB_IN  const float ray_origin[3], LAB_IN  const float ray_dir[3],
                            LAB_IN  const float rect1[3],      LAB_IN  const float rect2[3])
{
    float t_1[3], t_2[3];
    // t_1 = (rect1 - ray_origin) (/) ray_dir
    LAB_Vec3_Sub(t_1, rect1, ray_origin);
    LAB_Vec3_HadmdDiv(t_1, t_1, ray_dir); // TODO 0? does it result in +-infty

    // t_2 = (rect2 - ray_origin) (/) ray_dir
    LAB_Vec3_Sub(t_2, rect2, ray_origin);
    LAB_Vec3_HadmdDiv(t_2, t_2, ray_dir); // TODO 0? does it result in +-infty

    float t_min[3], t_max[3];
    LAB_Vec3_HadmdMin(t_min, t_1, t_2);
    LAB_Vec3_HadmdMax(t_max, t_1, t_2);

    /*if(t_min[0] > t_max[1] || t_min[0] > t_max[2] ||
       t_min[1] > t_max[0] || t_min[1] > t_max[2] ||
       t_min[2] > t_max[0] || t_min[2] > t_max[1]) return false;*/

    int hit_min/*, hit_max*/;
    hit_min = LAB_Vec3_GetMaxIndex(t_min);
    //hit_max = LAB_Vec3_GetMinIndex(t_max);

    float t_hit_min, t_hit_max;
    t_hit_min = LAB_Vec3_GetMax(t_min);
    t_hit_max = LAB_Vec3_GetMin(t_max);
    if(t_hit_max < 0) return false;
    if(t_hit_max < t_hit_min) return false;


    // collision_point = ray_origin + t_hit_min*ray_dir;
    LAB_Vec3_SclMul(collision_point, ray_dir, t_hit_min);
    LAB_Vec3_Add(collision_point, collision_point, ray_origin);

    *collision_steps = t_hit_min;
    *collision_face = hit_min << 1 | (ray_dir[hit_min] < 0);
    /*if(t_min[0] > t_min[1])
        if(t_min[0] > t_min[2])
            *collision_face = ray_dir[0] < 0 ? LAB_DIR_E : LAB_DIR_W;
        else
            *collision_face = ray_dir[2] < 0 ? LAB_DIR_S : LAB_DIR_N;
    else
        if(t_min[1] > t_min[2])
            *collision_face = ray_dir[1] < 0 ? LAB_DIR_U : LAB_DIR_D;
        else
            *collision_face = ray_dir[2] < 0 ? LAB_DIR_S : LAB_DIR_N;*/

    return true;
}
