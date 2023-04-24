#include "LAB_builtin_model.h"

#include "LAB_direction.h"

#define LAB_SUBARR_ADDR(arr, type, subsize, index) \
    (((type(*)[subsize])(arr))[index])

#define LAB_TRI_ADDR(arr, subsize, index) \
    LAB_SUBARR_ADDR(arr, LAB_Triangle, subsize, index)


void LAB_Builtin_WriteQuad(LAB_ModelQuad* quad,
                           const float v[4][5],
                           LAB_Color c,
                           uint64_t cull, uint64_t light, uint64_t vis)
{
    for(int i = 0; i < 4; ++i)
        quad->v[i] = (LAB_ModelQuadVertex)
        {
            .pos = LAB_Vec3F_FromArray(&v[i][0]),
            .tex = LAB_Vec2F_FromArray(&v[i][3]),
        };
    quad->color = c;
    quad->cull = cull;
    quad->light = light;
    quad->visibility = vis;
}

bool LAB_Builtin_AddQuad(LAB_Model* m,
                         const float v[4][5],
                         LAB_Color c,
                         uint64_t cull, uint64_t light, uint64_t vis)
{
    LAB_ModelQuad* quad = LAB_Model_Extend(m, 1);
    if(quad == NULL) return false;
    LAB_Builtin_WriteQuad(quad, v, c, cull, light, vis);
    return true;
}


static const uint8_t LAB_cube_vertices[6][4][3] = {
    /** WEST **/
    { { 0, 1, 0 },
      { 0, 1, 1 },
      { 0, 0, 0 },
      { 0, 0, 1 } },

    /** EAST **/
    { { 1, 1, 1 },
      { 1, 1, 0 },
      { 1, 0, 1 },
      { 1, 0, 0 } },

    /** DOWN **/
    { { 1, 0, 0 },
      { 0, 0, 0 },
      { 1, 0, 1 },
      { 0, 0, 1 } },

    /** UP **/
    { { 0, 1, 0 },
      { 1, 1, 0 },
      { 0, 1, 1 },
      { 1, 1, 1 } },

    /** NORTH **/
    { { 1, 1, 0 },
      { 0, 1, 0 },
      { 1, 0, 0 },
      { 0, 0, 0 } },

    /** SOUTH **/
    { { 0, 1, 1 },
      { 1, 1, 1 },
      { 0, 0, 1 },
      { 1, 0, 1 } },
};

void LAB_Builtin_WriteCube(LAB_ModelQuad* quads6,
                           const float aabb[2][3], const float tex[6][2][2],
                           LAB_BoxColors colors)
{
    for(int face = 0; face < 6; ++face)
    {
        const uint8_t (*f)[3] = LAB_cube_vertices[face];
        const float (*t)[2] = tex[face];
        const float v[4][5] = {
            { aabb[f[0][0]][0], aabb[f[0][1]][1], aabb[f[0][2]][2], t[0][0], t[0][1] },
            { aabb[f[1][0]][0], aabb[f[1][1]][1], aabb[f[1][2]][2], t[1][0], t[0][1] },
            { aabb[f[2][0]][0], aabb[f[2][1]][1], aabb[f[2][2]][2], t[0][0], t[1][1] },
            { aabb[f[3][0]][0], aabb[f[3][1]][1], aabb[f[3][2]][2], t[1][0], t[1][1] },
        };
        int face_s = 1 << face;
        int vis = LAB_APPROX_EQ(aabb[face&1][face>>1], (float)(face&1)) ? face_s : LAB_DIR_ALL;
        LAB_Builtin_WriteQuad(&quads6[face], v, colors.sides[face], vis, face_s, vis);
    }
}


void LAB_Builtin_WriteCubeInverted(LAB_ModelQuad* quads6,
                                   const float aabb[2][3], const float tex[6][2][2],
                                   LAB_BoxColors colors)
{
    for(int face = 0; face < 6; ++face)
    {
        const uint8_t (*f)[3] = LAB_cube_vertices[face];
        const float (*t)[2] = tex[face];
        const float v[4][5] = {
            { aabb[f[0][0]][0], aabb[f[0][1]][1], aabb[f[0][2]][2], t[0][0], t[0][1] },
            { aabb[f[2][0]][0], aabb[f[2][1]][1], aabb[f[2][2]][2], t[0][0], t[1][1] },
            { aabb[f[1][0]][0], aabb[f[1][1]][1], aabb[f[1][2]][2], t[1][0], t[0][1] },
            { aabb[f[3][0]][0], aabb[f[3][1]][1], aabb[f[3][2]][2], t[1][0], t[1][1] },
        };
        int face_s = 1 << face;
        int vis = LAB_APPROX_EQ(aabb[face&1][face>>1], (float)(face&1)) ? face_s : LAB_DIR_ALL;
        LAB_Builtin_WriteQuad(&quads6[face], v, colors.sides[face], vis, face_s, vis);
    }
}

const LAB_BoxColors LAB_box_color_shade = {{
    LAB_RGBX(EBEBEB), LAB_RGBX(EBEBEB), // west east
    LAB_RGBX(C0C0C0), LAB_RGBX(FFFFFF), // down up
    LAB_RGBX(D7D7D7), LAB_RGBX(D7D7D7), // north south
}};
const LAB_BoxColors LAB_box_color_flat = {{
    LAB_RGBX(FFFFFF), LAB_RGBX(FFFFFF), // west east
    LAB_RGBX(FFFFFF), LAB_RGBX(FFFFFF), // down up
    LAB_RGBX(FFFFFF), LAB_RGBX(FFFFFF), // north south
}};

const float LAB_full_aabb[2][3] = { { 0, 0, 0 }, { 1, 1, 1 } };



bool LAB_Builtin_ModelAddCube(LAB_Model* m,
                              const float aabb[2][3], const float tex[6][2][2],
                              LAB_BoxColors colors)
{
    LAB_ModelQuad* quads = LAB_Model_Extend(m, 6);
    if(quads == NULL) return false;
    LAB_Builtin_WriteCube(quads, aabb, tex, colors);
    return true;
}

bool LAB_Builtin_ModelAddCubeAll(LAB_Model* m,
                                 const float aabb[2][3], const float tex[2][2],
                                 LAB_BoxColors colors)
{
    float tex6[6][2][2];
    for(int i = 0; i < 6; ++i) memcpy(tex6[i], tex, sizeof(float[2][2]));
    return LAB_Builtin_ModelAddCube(m, aabb, (const float(*)[2][2])tex6, colors);
}



bool LAB_Builtin_ModelAddCubeInverted(LAB_Model* m,
                                      const float aabb[2][3], const float tex[6][2][2],
                                      LAB_BoxColors colors)
{
    LAB_ModelQuad* tris = LAB_Model_Extend(m, 6);
    if(tris == NULL) return false;
    LAB_Builtin_WriteCubeInverted(tris, aabb, tex, colors);
    return true;
}

bool LAB_Builtin_ModelAddCubeInvertedAll(LAB_Model* m,
                                         const float aabb[2][3], const float tex[2][2],
                                         LAB_BoxColors colors)
{
    float tex6[6][2][2];
    for(int i = 0; i < 6; ++i) memcpy(tex6[i], tex, sizeof(float[2][2]));
    return LAB_Builtin_ModelAddCubeInverted(m, aabb, (const float(*)[2][2])tex6, colors);
}



const float LAB_cross_aabb[2][3] = { { 0.1, 0, 0.1 }, { 0.9, 1, 0.9 } };

static const uint8_t LAB_cross_visibility[4] = {
    LAB_DIR_N|LAB_DIR_W,
    LAB_DIR_S|LAB_DIR_E,
    LAB_DIR_N|LAB_DIR_E,
    LAB_DIR_S|LAB_DIR_W,
};
static const uint8_t LAB_cross_vertices[4][4][3] = {
    /** NORTH-WEST **/
    { { 1, 1, 0 },
      { 0, 1, 1 },
      { 1, 0, 0 },
      { 0, 0, 1 } },

    /** SOUTH-EAST **/
    { { 0, 1, 1 },
      { 1, 1, 0 },
      { 0, 0, 1 },
      { 1, 0, 0 } },

    /** NORTH-EAST **/
    { { 1, 1, 1 },
      { 0, 1, 0 },
      { 1, 0, 1 },
      { 0, 0, 0 } },

    /** SOUTH-WEST **/
    { { 0, 1, 0 },
      { 1, 1, 1 },
      { 0, 0, 0 },
      { 1, 0, 1 } },
};

void LAB_Builtin_WriteCross(LAB_ModelQuad quads[4],
                            const float aabb[2][3], const float tex[2][2],
                            LAB_Color c)
{
    for(int i = 0; i < 4; ++i)
    {
        const uint8_t (*f)[3] = LAB_cross_vertices[i];
        const float v[4][5] = {
            { aabb[f[0][0]][0], aabb[f[0][1]][1], aabb[f[0][2]][2], tex[0][0], tex[0][1] },
            { aabb[f[1][0]][0], aabb[f[1][1]][1], aabb[f[1][2]][2], tex[1][0], tex[0][1] },
            { aabb[f[2][0]][0], aabb[f[2][1]][1], aabb[f[2][2]][2], tex[0][0], tex[1][1] },
            { aabb[f[3][0]][0], aabb[f[3][1]][1], aabb[f[3][2]][2], tex[1][0], tex[1][1] },
        };
        int face_vis = LAB_cross_visibility[i];
        LAB_Builtin_WriteQuad(&quads[i], v, c, LAB_DIR_ALL, 0, face_vis);
    }
}


bool LAB_Builtin_ModelAddCross(LAB_Model* m,
                               const float aabb[2][3], const float tex[2][2],
                               LAB_Color c)
{
    LAB_ModelQuad* tris = LAB_Model_Extend(m, 4);
    if(tris == NULL) return false;
    LAB_Builtin_WriteCross(tris, aabb, tex, c);
    return true;
}