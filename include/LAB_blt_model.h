/**
 *  Builtin models
 */
#pragma once
#include "LAB_model.h"
#include "LAB_direction.h"

#define LAB_SHADE_X LAB_RGBX(EBEBEB) // west east
#define LAB_SHADE_Y LAB_RGBX(C0C0C0) // bottom
#define LAB_SHADE_Z LAB_RGBX(D7D7D7) // north south

//#define LAB_SHADE_C LAB_RGBX(D0D0D0) // cross
#define LAB_SHADE_C LAB_RGBX(FFFFFF) // cross

#define LAB_NO_SHADE LAB_RGBX(FFFFFF) // north south


#define LAB_MK_TRI(x0, y0, z0,  u0, v0, \
                   x1, y1, z1,  u1, v1, \
                   x2, y2, z2,  u2, v2, \
                   c, cull, light) \
    { .v = { { .x = x0, y0, z0, .color = c, .u = u0, .v = v0, .flags = cull  }, \
             { .x = x1, y1, z1, .color = c, .u = u1, .v = v1, .flags = light }, \
             { .x = x2, y2, z2, .color = c, .u = u2, .v = v2, .flags = 0     } } }

/// LAB_SetVertex(&t0[1],  x0, y0, z0,  r0, g0, b0, a0,  u0, v0,  cr, cg, cb, tx, ty);
/// LAB_SetVertex(&t0[2],  x1, y1, z1,  r1, g1, b1, a1,  u1, v1,  cr, cg, cb, tx, ty);
/// LAB_SetVertex(&t1[2],  x1, y1, z1,  r1, g1, b1, a1,  u1, v1,  cr, cg, cb, tx, ty);
/// LAB_SetVertex(&t0[0],  x2, y2, z2,  r2, g2, b2, a2,  u2, v2,  cr, cg, cb, tx, ty);
/// LAB_SetVertex(&t1[1],  x2, y2, z2,  r2, g2, b2, a2,  u2, v2,  cr, cg, cb, tx, ty);
/// LAB_SetVertex(&t1[0],  x3, y3, z3,  r3, g3, b3, a3,  u3, v3,  cr, cg, cb, tx, ty);

/*#define LAB_MK_QUAD(x0, y0, z0,  u0, v0, \
                    x1, y1, z1,  u1, v1, \
                    x2, y2, z2,  u2, v2, \
                    x3, y3, z3,  u3, v3, \
                    color, cull, light) \
    LAB_MK_TRI(x2, y2, z2,  u2, v2, \
               x0, y0, z0,  u0, v0, \
               x1, y1, z1,  u1, v1, \
               color, cull, light), \
    LAB_MK_TRI(x3, y3, z3,  u3, v3, \
               x2, y2, z2,  u2, v2, \
               x1, y1, z1,  u1, v1, \
               color, cull, light)*/

#define LAB_MK_QUAD(x0, y0, z0,  u0, v0, \
                    x1, y1, z1,  u1, v1, \
                    x2, y2, z2,  u2, v2, \
                    x3, y3, z3,  u3, v3, \
                    color, cull, light) \
    LAB_MK_TRI(x1, y1, z1,  u1, v1, \
               x2, y2, z2,  u2, v2, \
               x0, y0, z0,  u0, v0, \
               color, cull, light), \
    LAB_MK_TRI(x2, y2, z2,  u2, v2, \
               x1, y1, z1,  u1, v1, \
               x3, y3, z3,  u3, v3, \
               color, cull, light)


#define LAB_DEF_MODEL_CUBE_BASE(name,  wu, wv, wc,  eu, ev, ec, \
                                       du, dv, dc,  uu, uv, uc, \
                                       nu, nv, nc,  su, sv, sc, \
                                       shade_x,                 \
                                       shade_y,                 \
                                       shade_z, ...)            \
LAB_Triangle name##_data[] = {                             \
    /** WEST **/                                           \
    LAB_MK_QUAD(0, 1, 0,  (wu)+0, (wv)+0,                  \
                0, 1, 1,  (wu)+1, (wv)+0,                  \
                0, 0, 0,  (wu)+0, (wv)+1,                  \
                0, 0, 1,  (wu)+1, (wv)+1,                  \
                LAB_MUL_COLOR(wc, shade_x),                \
                LAB_DIR_WEST, LAB_DIR_WEST),               \
    /** EAST **/                                           \
    LAB_MK_QUAD(1, 1, 1,  (eu)+0, (ev)+0,                  \
                1, 1, 0,  (eu)+1, (ev)+0,                  \
                1, 0, 1,  (eu)+0, (ev)+1,                  \
                1, 0, 0,  (eu)+1, (ev)+1,                  \
                LAB_MUL_COLOR(ec, shade_x),                \
                LAB_DIR_EAST, LAB_DIR_EAST),               \
    /** DOWN **/                                           \
    LAB_MK_QUAD(1, 0, 0,  (du)+0, (dv)+0,                  \
                0, 0, 0,  (du)+1, (dv)+0,                  \
                1, 0, 1,  (du)+0, (dv)+1,                  \
                0, 0, 1,  (du)+1, (dv)+1,                  \
                LAB_MUL_COLOR(dc, shade_y),                \
                LAB_DIR_DOWN, LAB_DIR_DOWN),               \
    /** UP **/                                             \
    LAB_MK_QUAD(0, 1, 0,  (uu)+0, (uv)+0,                  \
                1, 1, 0,  (uu)+1, (uv)+0,                  \
                0, 1, 1,  (uu)+0, (uv)+1,                  \
                1, 1, 1,  (uu)+1, (uv)+1,                  \
                (uc),                                      \
                LAB_DIR_UP, LAB_DIR_UP),                   \
    /** NORTH **/                                          \
    LAB_MK_QUAD(1, 1, 0,  (nu)+0, (nv)+0,                  \
                0, 1, 0,  (nu)+1, (nv)+0,                  \
                1, 0, 0,  (nu)+0, (nv)+1,                  \
                0, 0, 0,  (nu)+1, (nv)+1,                  \
                LAB_MUL_COLOR(nc, shade_z),                \
                LAB_DIR_NORTH, LAB_DIR_NORTH),             \
    /** SOUTH **/                                          \
    LAB_MK_QUAD(0, 1, 1,  (su)+0, (sv)+0,                  \
                1, 1, 1,  (su)+1, (sv)+0,                  \
                0, 0, 1,  (su)+0, (sv)+1,                  \
                1, 0, 1,  (su)+1, (sv)+1,                  \
                LAB_MUL_COLOR(sc, shade_z),                \
                LAB_DIR_SOUTH, LAB_DIR_SOUTH)              \
};                                                         \
LAB_Model name = {                                         \
    .size = sizeof(name##_data) / sizeof(LAB_Triangle),    \
    .data = name##_data,                                   \
    __VA_ARGS__                                            \
}



#define LAB_DEF_MODEL_CUBE(name,  wu, wv, wc,  eu, ev, ec, \
                                  du, dv, dc,  uu, uv, uc, \
                                  nu, nv, nc,  su, sv, sc, \
                                  ...)                     \
   LAB_DEF_MODEL_CUBE_BASE(name,  wu, wv, wc,  eu, ev, ec, \
                                  du, dv, dc,  uu, uv, uc, \
                                  nu, nv, nc,  su, sv, sc, \
                                  LAB_SHADE_X,             \
                                  LAB_SHADE_Y,             \
                                  LAB_SHADE_Z,             \
                                  __VA_ARGS__)


#define LAB_DEF_MODEL_CUBE_ALL(name,  tu, tv, tc, ...) \
    LAB_DEF_MODEL_CUBE(name,  tu, tv, tc,  tu, tv, tc, \
                              tu, tv, tc,  tu, tv, tc, \
                              tu, tv, tc,  tu, tv, tc, \
                              __VA_ARGS__)


#define LAB_DEF_MODEL_CUBE_SIDES(name,  hu, hv, hc, \
                                        du, dv, dc,  uu, uv, uc, \
                                        ...) \
    LAB_DEF_MODEL_CUBE(name,  hu, hv, hc,  hu, hv, hc, \
                              du, dv, dc,  uu, uv, uc, \
                              hu, hv, hc,  hu, hv, hc, \
                              __VA_ARGS__)


//---------


#define LAB_DEF_MODEL_CUBE_SHADELESS(name,  wu, wv, wc,  eu, ev, ec, \
                                            du, dv, dc,  uu, uv, uc, \
                                            nu, nv, nc,  su, sv, sc, \
                                            ...)                     \
   LAB_DEF_MODEL_CUBE_BASE(name,  wu, wv, wc,  eu, ev, ec, \
                                  du, dv, dc,  uu, uv, uc, \
                                  nu, nv, nc,  su, sv, sc, \
                                  LAB_NO_SHADE,            \
                                  LAB_NO_SHADE,            \
                                  LAB_NO_SHADE,            \
                                  __VA_ARGS__)


#define LAB_DEF_MODEL_CUBE_SHADELESS_ALL(name,  tu, tv, tc, ...) \
    LAB_DEF_MODEL_CUBE_SHADELESS(name,  tu, tv, tc,  tu, tv, tc, \
                              tu, tv, tc,  tu, tv, tc, \
                              tu, tv, tc,  tu, tv, tc, \
                              __VA_ARGS__)


#define LAB_DEF_MODEL_CUBE_SHADELESS_SIDES(name,  hu, hv, hc, \
                                                  du, dv, dc,  uu, uv, uc, \
                                                  ...) \
    LAB_DEF_MODEL_CUBE_SHADELESS(name,  hu, hv, hc,  hu, hv, hc, \
                              du, dv, dc,  uu, uv, uc, \
                              hu, hv, hc,  hu, hv, hc, \
                              __VA_ARGS__)










#define LAB_DEF_MODEL_CROSS_BASE(name,  wu, wv, wc,  eu, ev, ec, \
                                        nu, nv, nc,  su, sv, sc, \
                                        ...) \
LAB_Triangle name##_data[] = {                             \
    /** WEST **/                                           \
    LAB_MK_QUAD(0.9, 1, 0.1,  (wu)+0, (wv)+0,              \
                0.1, 1, 0.9,  (wu)+1, (wv)+0,              \
                0.9, 0, 0.1,  (wu)+0, (wv)+1,              \
                0.1, 0, 0.9,  (wu)+1, (wv)+1,              \
                LAB_MUL_COLOR(wc, LAB_SHADE_C),            \
                LAB_DIR_ALL, 0),                           \
    /** EAST **/                                           \
    LAB_MK_QUAD(0.1, 1, 0.9,  (eu)+0, (ev)+0,              \
                0.9, 1, 0.1,  (eu)+1, (ev)+0,              \
                0.1, 0, 0.9,  (eu)+0, (ev)+1,              \
                0.9, 0, 0.1,  (eu)+1, (ev)+1,              \
                LAB_MUL_COLOR(ec, LAB_SHADE_C),            \
                LAB_DIR_ALL, 0),                           \
    /** NORTH **/                                          \
    LAB_MK_QUAD(0.9, 1, 0.9,  (nu)+0, (nv)+0,              \
                0.1, 1, 0.1,  (nu)+1, (nv)+0,              \
                0.9, 0, 0.9,  (nu)+0, (nv)+1,              \
                0.1, 0, 0.1,  (nu)+1, (nv)+1,              \
                LAB_MUL_COLOR(nc, LAB_SHADE_C),            \
                LAB_DIR_ALL, 0),                           \
    /** SOUTH **/                                          \
    LAB_MK_QUAD(0.1, 1, 0.1,  (su)+0, (sv)+0,              \
                0.9, 1, 0.9,  (su)+1, (sv)+0,              \
                0.1, 0, 0.1,  (su)+0, (sv)+1,              \
                0.9, 0, 0.9,  (su)+1, (sv)+1,              \
                LAB_MUL_COLOR(sc, LAB_SHADE_C),            \
                LAB_DIR_ALL, 0)                            \
};                                                         \
LAB_Model name = {                                         \
    .size = sizeof(name##_data) / sizeof(LAB_Triangle),    \
    .data = name##_data,                                   \
    __VA_ARGS__                                            \
}

#define LAB_DEF_MODEL_CROSS(name,  tu, tv, tc, ...) \
    LAB_DEF_MODEL_CROSS_BASE(name,  tu, tv, tc,  tu, tv, tc, \
                                    tu, tv, tc,  tu, tv, tc, \
                                    __VA_ARGS__)
