#pragma once

#include <stdbool.h>

#include "LAB_direction.h"
#include "LAB_vec.h"
#include "LAB_mat.h"
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


LAB_INLINE
LAB_Vec4F LAB_Vec4F_FromPosition(LAB_Vec3F v)
{
    return (LAB_Vec4F) { v.x, v.y, v.z, 1 };
}

LAB_INLINE
LAB_Vec4F LAB_Vec4F_FromDirection(LAB_Vec3F v)
{
    return (LAB_Vec4F) { v.x, v.y, v.z, 0 };
}

/**
 * matrix: model projection matrix
 */
LAB_INLINE
LAB_Vec3F LAB_ProjectPoint(LAB_Mat4F matrix, LAB_Vec3F pos)
{
    LAB_Vec4F pos4 = LAB_Vec4F_FromPosition(pos);
    LAB_Vec4F v = LAB_Mat4F_RMul(matrix, pos4);

    return (LAB_Vec3F) { v.x/v.w, v.y/v.w, v.z/v.w };
}