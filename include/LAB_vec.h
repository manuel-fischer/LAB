#pragma once

#include "LAB_math.h"
#include "LAB_util.h"
#include "LAB_direction.h"
#include "LAB_bits.h"

// These macros work with arrays, no pointers!

///// VEC TYPES /////

#define LAB_OP_ADD(a, b) ((a) + (b))
#define LAB_OP_SUB(a, b) ((a) - (b))
#define LAB_OP_MUL(a, b) ((a) * (b))
#define LAB_OP_DIV(a, b) ((a) / (b))


#define LAB_OP_SHL(a, b) ((a) << (b))
#define LAB_OP_SHR(a, b) ((a) >> (b))

#define LAB_OP_ANDAND(a, b) ((a) && (b))
#define LAB_OP_OROR(a, b) ((a) || (b))
#define LAB_OP_EQEQ(a, b) ((a) == (b))
#define LAB_OP_NEQ(a, b) ((a) != (b))

#define LAB_V_MIN_INDEX3(x, y, z) LAB_SELECT_MIN3(x,0, y,1, z,2)
#define LAB_V_MAX_INDEX3(x, y, z) LAB_SELECT_MAX3(x,0, y,1, z,2)


#define LAB_VEC3_DECL LAB_INLINE LAB_ALWAYS_INLINE

#define LAB_VEC3_DEFINE_TYPE(vtype, ctype) \
    typedef struct vtype \
    { \
        ctype x, y, z; \
    } vtype; \
    \
    LAB_VEC3_DEFINE_OP(vtype, ctype, Add, LAB_OP_ADD) \
    LAB_VEC3_DEFINE_OP(vtype, ctype, Sub, LAB_OP_SUB) \
    LAB_VEC3_DEFINE_REDUCE_COMBINE(vtype, ctype, ctype, Dot, LAB_OP_ADD, LAB_OP_MUL) \
    LAB_VEC3_DEFINE_SELF_ADJOINT(vtype, ctype, ctype, LengthSq, LAB_OP_ADD, LAB_OP_MUL) \
    LAB_VEC3_DEFINE_DISTANCE_SQ(vtype, ctype) \
    LAB_VEC3_DEFINE_SCALAR_OP(vtype, ctype, Mul, LAB_OP_MUL) \
    LAB_VEC3_DEFINE_RSCALAR_OP(vtype, ctype, RMul, LAB_OP_MUL) \
    LAB_VEC3_DEFINE_RSCALAR_OP(vtype, ctype, Div, LAB_OP_DIV) \
    LAB_VEC3_DEFINE_OP(vtype, ctype, HdMul, LAB_OP_MUL) \
    LAB_VEC3_DEFINE_OP(vtype, ctype, HdDiv, LAB_OP_DIV) \
    LAB_VEC3_DEFINE_OP(vtype, ctype, HdMin, LAB_MIN) \
    LAB_VEC3_DEFINE_OP(vtype, ctype, HdMax, LAB_MAX) \
    LAB_VEC3_DEFINE_REDUCE3(vtype, ctype, ctype, GetMin, LAB_MIN3) \
    LAB_VEC3_DEFINE_REDUCE3(vtype, ctype, ctype, GetMax, LAB_MAX3) \
    LAB_VEC3_DEFINE_REDUCE3(vtype, ctype, size_t, MinIndex, LAB_V_MIN_INDEX3) \
    LAB_VEC3_DEFINE_REDUCE3(vtype, ctype, size_t, MaxIndex, LAB_V_MAX_INDEX3) \
    LAB_VEC3_DEFINE_FROM_DIR_INDEX(vtype, ctype) \
    LAB_VEC3_DEFINE_ARRAY_CONVERSIONS(vtype, ctype) \
    LAB_VEC3_DEFINE_GET(vtype, ctype)

#define LAB_VEC3_DEFINE_OP(vtype, ctype, opname, opfn) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    vtype vtype##_##opname(vtype a, vtype b) { return (vtype){ .x = opfn(a.x, b.x), .y = opfn(a.y, b.y), .z = opfn(a.z, b.z) }; }

#define LAB_VEC3_DEFINE_SCALAR_OP(vtype, ctype, opname, opfn) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    vtype vtype##_##opname(ctype scalar, vtype b) { return (vtype){ .x = opfn(scalar, b.x), .y = opfn(scalar, b.y), .z = opfn(scalar, b.z) }; }

#define LAB_VEC3_DEFINE_RSCALAR_OP(vtype, ctype, opname, opfn) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    vtype vtype##_##opname(vtype a, ctype scalar) { return (vtype){ .x = opfn(a.x, scalar), .y = opfn(a.y, scalar), .z = opfn(a.z, scalar) }; }

#define LAB_VEC3_DEFINE_REDUCE_COMBINE(vtype, ctype, rtype, opname, reducefn, combinefn) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    rtype vtype##_##opname(vtype a, vtype b) { return reducefn(reducefn(combinefn(a.x, b.x), combinefn(a.y, b.y)), combinefn(a.z, b.z)); }

#define LAB_VEC3_DEFINE_SELF_ADJOINT(vtype, ctype, rtype, opname, reducefn, combinefn) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    rtype vtype##_##opname(vtype a) { return reducefn(reducefn(combinefn(a.x, a.x), combinefn(a.y, a.y)), combinefn(a.z, a.z)); }

#define LAB_VEC3_DEFINE_REDUCE(vtype, ctype, rtype, opname, reducefn) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    rtype vtype##_##opname(vtype a) { return reducefn(reducefn(a.x, a.y), a.z); }

#define LAB_VEC3_DEFINE_REDUCE3(vtype, ctype, rtype, opname, reduce3fn) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    rtype vtype##_##opname(vtype a) { return reduce3fn(a.x, a.y, a.z); }

#define LAB_VEC3_DEFINE_DISTANCE_SQ(vtype, ctype) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    ctype vtype##_DistanceSq(vtype a, vtype b) \
    { \
        ctype dx = a.x-b.x, dy = a.y-b.y, dz = a.z-b.z; \
        return dx*dx + dy*dy + dz*dz; \
    }




#define LAB_VEC3_DEFINE_FROM_DIR_INDEX(vtype, ctype) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    vtype vtype##_##FromDirIndex(LAB_DirIndex i) { return (vtype) { LAB_OX(i), LAB_OY(i), LAB_OZ(i) }; }


#define LAB_VEC3_DEFINE_ARRAY_CONVERSIONS(vtype, ctype) \
    LAB_VEC3_DECL \
    vtype vtype##_FromArray(const ctype* array) { return (vtype) { array[0], array[1], array[2] }; } \
    LAB_VEC3_DECL \
    ctype* vtype##_AsArray(vtype* a) { return &a->x; } \
    LAB_VEC3_DECL \
    const ctype* vtype##_AsCArray(const vtype* a) { return &a->x; } \
    LAB_VEC3_DECL \
    ctype* vtype##_ToArray(ctype* array, vtype a) { array[0] = a.x; array[1] = a.y; array[2] = a.z; return array; } \
    LAB_VEC3_DECL \
    void vtype##_Unpack(ctype* x, ctype* y, ctype* z, vtype a) { *x = a.x; *y = a.y; *z = a.z;}


#define LAB_VEC3_DEFINE_GET(vtype, ctype) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    ctype vtype##_##Get(vtype a, size_t i) { \
        return vtype##_AsArray(&a)[i]; \
    } \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    ctype* vtype##_##Ref(vtype* a, size_t i) { \
        return &vtype##_AsArray(a)[i]; \
    }


#define LAB_VEC3_DEFINE_INVERT(vtype, ctype) /*TODO*/


#define LAB_VEC3_DEFINE_TYPE_FLOAT(vtype, ctype) \
    LAB_VEC3_DEFINE_TYPE(vtype, ctype) \
    LAB_VEC3_DEFINE_INVERT(vtype, ctype)



#define LAB_VEC3_DEFINE_TYPE_INT(vtype, ctype) \
    LAB_VEC3_DEFINE_TYPE(vtype, ctype) \
    \
    LAB_VEC3_DEFINE_RSCALAR_OP(vtype, ctype, Sar, LAB_Sar) \
    LAB_VEC3_DEFINE_RSCALAR_OP(vtype, ctype, Shr, LAB_OP_SHR) \
    LAB_VEC3_DEFINE_RSCALAR_OP(vtype, ctype, Shl, LAB_OP_SHL) \
    \
    LAB_VEC3_DEFINE_REDUCE_COMBINE(LAB_Vec3I, int, bool, Equals, LAB_OP_ANDAND, LAB_OP_EQEQ)



LAB_VEC3_DEFINE_TYPE_INT(LAB_Vec3I, int)

LAB_VEC3_DEFINE_TYPE_FLOAT(LAB_Vec3F, float)
LAB_VEC3_DEFINE_TYPE_FLOAT(LAB_Vec3D, double)

#define LAB_VEC3_DEFINE_TYPE_CONVERSION(name, vfrom, cfrom, vto, cto) \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    vto name(vfrom a) { return (vto) { .x = (cto)a.x, .y = (cto)a.y, .z = (cto)a.z }; }

LAB_VEC3_DEFINE_TYPE_CONVERSION(LAB_Vec3I2F, LAB_Vec3I, int, LAB_Vec3F, float)
LAB_VEC3_DEFINE_TYPE_CONVERSION(LAB_Vec3I2D, LAB_Vec3I, int, LAB_Vec3D, double)

LAB_VEC3_DEFINE_TYPE_CONVERSION(LAB_Vec3F2D, LAB_Vec3F, float, LAB_Vec3D, double)
LAB_VEC3_DEFINE_TYPE_CONVERSION(LAB_Vec3D2F_Cast, LAB_Vec3D, double, LAB_Vec3F, float)

// rounding conversions
LAB_VEC3_DECL LAB_VALUE_CONST
LAB_Vec3I LAB_Vec3F2I_FastFloor(LAB_Vec3F a)
{
    return (LAB_Vec3I)
    {
        LAB_FastFloorF2I(a.x),
        LAB_FastFloorF2I(a.y),
        LAB_FastFloorF2I(a.z),
    };
}

LAB_VEC3_DECL LAB_VALUE_CONST
LAB_Vec3I LAB_Vec3D2I_FastFloor(LAB_Vec3D a)
{
    return (LAB_Vec3I)
    {
        LAB_FastFloorD2I(a.x),
        LAB_FastFloorD2I(a.y),
        LAB_FastFloorD2I(a.z),
    };
}





LAB_VEC3_DECL LAB_VALUE_CONST
LAB_Vec3I LAB_Vec3I_Abs(LAB_Vec3I a)
{
    return (LAB_Vec3I) { abs(a.x), abs(a.y), abs(a.z), };
}


LAB_VEC3_DECL LAB_VALUE_CONST
LAB_Vec3F LAB_Vec3F_Abs(LAB_Vec3F a)
{
    return (LAB_Vec3F) { fabsf(a.x), fabsf(a.y), fabsf(a.z), };
}

LAB_VEC3_DECL LAB_VALUE_CONST
LAB_Vec3D LAB_Vec3D_Abs(LAB_Vec3D a)
{
    return (LAB_Vec3D) { fabs(a.x), fabs(a.y), fabs(a.z), };
}


///// BOX TYPES /////


#define LAB_BOX3_DEFINE_TYPE(btype, vtype, ctype) \
    typedef struct btype \
    { \
        vtype a, b; \
    } btype; \
    \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    btype btype##_Add(btype box, vtype offset) \
    { \
        return (btype) { vtype##_Add(box.a, offset), vtype##_Add(box.b, offset) }; \
    } \
    \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    bool btype##_Contains_Inc(btype box, vtype v) \
    { \
        return box.a.x <= v.x && v.x <= box.b.x \
            && box.a.y <= v.y && v.y <= box.b.y \
            && box.a.z <= v.z && v.z <= box.b.z; \
    } \
    \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    bool btype##_IsEmpty(btype box) \
    { \
        return box.a.x >= box.b.x \
            || box.a.y >= box.b.y \
            || box.a.z >= box.b.z; \
    } \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    btype btype##_Intersection(btype a, btype b) \
    { \
        return (btype) { vtype##_HdMax(a.a, b.a), vtype##_HdMin(a.b, b.b) }; \
    } \
    \
    LAB_VEC3_DECL LAB_VALUE_CONST \
    bool btype##_Intersects(btype a, btype b) \
    { \
        return !btype##_IsEmpty(btype##_Intersection(a, b)); \
    }


LAB_BOX3_DEFINE_TYPE(LAB_Box3I, LAB_Vec3I, int)
LAB_BOX3_DEFINE_TYPE(LAB_Box3F, LAB_Vec3F, int)
LAB_BOX3_DEFINE_TYPE(LAB_Box3D, LAB_Vec3D, int)

#define LAB_VEC3_FROM(vtype, ptr) ((vtype) { (ptr)->x, (ptr)->y, (ptr)->z })