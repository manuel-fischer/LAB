#pragma once

#include <stdbool.h>

#include "LAB_direction.h"
#include "LAB_vec.h"
#include "LAB_opt.h" // -> LAB_IN, LAB_OUT

typedef struct LAB_Vec3Algo_RayVsBox_Hit
{
    LAB_Vec3F collision_point;
    float collision_steps;
    LAB_DirIndex collision_face;

    bool has_hit;
} LAB_Vec3Algo_RayVsBox_Hit;

LAB_INLINE
LAB_Vec3Algo_RayVsBox_Hit LAB_Vec3Algo_RayVsBox(
    LAB_Vec3F ray_origin, LAB_Vec3F ray_dir,
    LAB_Box3F box)
{
    LAB_Vec3Algo_RayVsBox_Hit result;

    // TODO div by 0? does it result in +-infty:
    // t_1 = (rect1 - ray_origin) (/) ray_dir
    LAB_Vec3F t_1 = LAB_Vec3F_HdDiv(LAB_Vec3F_Sub(box.a, ray_origin), ray_dir);
    // t_2 = (rect2 - ray_origin) (/) ray_dir
    LAB_Vec3F t_2 = LAB_Vec3F_HdDiv(LAB_Vec3F_Sub(box.b, ray_origin), ray_dir);

    LAB_Vec3F t_min = LAB_Vec3F_HdMin(t_1, t_2);
    LAB_Vec3F t_max = LAB_Vec3F_HdMax(t_1, t_2);

    /*if(t_min[0] > t_max[1] || t_min[0] > t_max[2] ||
       t_min[1] > t_max[0] || t_min[1] > t_max[2] ||
       t_min[2] > t_max[0] || t_min[2] > t_max[1]) return false;*/

    int hit_min = LAB_Vec3F_MaxIndex(t_min);
    int hit_max = LAB_Vec3F_MinIndex(t_max);

    float t_hit_min = LAB_Vec3F_Get(t_min, hit_min);
    float t_hit_max = LAB_Vec3F_Get(t_max, hit_max);

    if(t_hit_max < 0)         return (result.has_hit=false, result);
    if(t_hit_max < t_hit_min) return (result.has_hit=false, result);


    // collision_point = ray_origin + t_hit_min*ray_dir;
    result.collision_point = LAB_Vec3F_Add(ray_origin, LAB_Vec3F_Mul(t_hit_min, ray_dir));
    result.collision_steps = t_hit_min;
    result.collision_face  = hit_min << 1 | (LAB_Vec3F_Get(ray_dir, hit_min) < 0);

    return (result.has_hit=true, result);
}


/**
 * matrix: model projection matrix
 */
LAB_INLINE
LAB_Vec3F LAB_ProjectPoint(double matrix[16], LAB_Vec3F pos)
{
    float proj_vec[3];
    LAB_UNROLL(3)
    for(int i = 0; i < 3; ++i)
    {
        proj_vec[i] = matrix[i+4*0]*pos.x+matrix[i+4*1]*pos.y+matrix[i+4*2]*pos.z
                    + matrix[i+4*3];
    }
    return (LAB_Vec3F) {
        .x = proj_vec[0] / proj_vec[2],
        .y = proj_vec[1] / proj_vec[2],
        .z = proj_vec[2],
    };
}