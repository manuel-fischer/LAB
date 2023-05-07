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
                           LAB_Box3F aabb, LAB_BoxTextures tex,
                           LAB_BoxColors colors)
{
    const LAB_Vec3F *a = LAB_Box3F_AsCArray(&aabb);
    const float(*aa)[3] = LAB_Box3F_AsCMDArray(&aabb);
    for(int face = 0; face < 6; ++face)
    {
        const uint8_t (*f)[3] = LAB_cube_vertices[face];
        LAB_Box2F t = tex.a[face];
        const float v[4][5] = {
            { a[f[0][0]].x, a[f[0][1]].y, a[f[0][2]].z, t.a.x, t.a.y },
            { a[f[1][0]].x, a[f[1][1]].y, a[f[1][2]].z, t.b.x, t.a.y },
            { a[f[2][0]].x, a[f[2][1]].y, a[f[2][2]].z, t.a.x, t.b.y },
            { a[f[3][0]].x, a[f[3][1]].y, a[f[3][2]].z, t.b.x, t.b.y },
        };
        int face_s = 1 << face;
        int vis = LAB_APPROX_EQ(aa[face&1][face>>1], (float)(face&1)) ? face_s : LAB_DIR_ALL;
        LAB_Builtin_WriteQuad(&quads6[face], v, colors.sides[face], vis, face_s, vis);
    }
}


void LAB_Builtin_WriteCubeInverted(LAB_ModelQuad* quads6,
                                   LAB_Box3F aabb, LAB_BoxTextures tex,
                                   LAB_BoxColors colors)
{
    const LAB_Vec3F *a = LAB_Box3F_AsCArray(&aabb);
    const float(*aa)[3] = LAB_Box3F_AsCMDArray(&aabb);
    for(int face = 0; face < 6; ++face)
    {
        const uint8_t (*f)[3] = LAB_cube_vertices[face];
        LAB_Box2F t = tex.a[face];
        const float v[4][5] = {
            { a[f[0][0]].x, a[f[0][1]].y, a[f[0][2]].z, t.a.x, t.a.y },
            { a[f[2][0]].x, a[f[2][1]].y, a[f[2][2]].z, t.a.x, t.b.y },
            { a[f[1][0]].x, a[f[1][1]].y, a[f[1][2]].z, t.b.x, t.a.y },
            { a[f[3][0]].x, a[f[3][1]].y, a[f[3][2]].z, t.b.x, t.b.y },
        };
        int face_s = 1 << face;
        int vis = LAB_APPROX_EQ(aa[face&1][face>>1], (float)(face&1)) ? face_s : LAB_DIR_ALL;
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



bool LAB_Builtin_ModelAddCube(LAB_Model* m,
                              LAB_Box3F aabb, LAB_BoxTextures tex,
                              LAB_BoxColors colors)
{
    LAB_ModelQuad* quads = LAB_Model_Extend(m, 6);
    if(quads == NULL) return false;
    LAB_Builtin_WriteCube(quads, aabb, tex, colors);
    return true;
}

bool LAB_Builtin_ModelAddCubeAll(LAB_Model* m,
                                 LAB_Box3F aabb, LAB_Box2F tex,
                                 LAB_BoxColors colors)
{
    return LAB_Builtin_ModelAddCube(m, aabb, LAB_BoxTextures_All(tex), colors);
}



bool LAB_Builtin_ModelAddCubeInverted(LAB_Model* m,
                                      LAB_Box3F aabb, LAB_BoxTextures tex,
                                      LAB_BoxColors colors)
{
    LAB_ModelQuad* tris = LAB_Model_Extend(m, 6);
    if(tris == NULL) return false;
    LAB_Builtin_WriteCubeInverted(tris, aabb, tex, colors);
    return true;
}

bool LAB_Builtin_ModelAddCubeInvertedAll(LAB_Model* m,
                                         LAB_Box3F aabb, LAB_Box2F tex,
                                         LAB_BoxColors colors)
{
    return LAB_Builtin_ModelAddCubeInverted(m, aabb, LAB_BoxTextures_All(tex), colors);
}



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
                            LAB_Box3F aabb, LAB_Box2F tex,
                            LAB_Color c)
{
    const LAB_Vec3F *a = LAB_Box3F_AsCArray(&aabb);
    for(int i = 0; i < 4; ++i)
    {
        const uint8_t (*f)[3] = LAB_cross_vertices[i];
        const float v[4][5] = {
            { a[f[0][0]].x, a[f[0][1]].y, a[f[0][2]].z, tex.a.x, tex.a.y },
            { a[f[1][0]].x, a[f[1][1]].y, a[f[1][2]].z, tex.b.x, tex.a.y },
            { a[f[2][0]].x, a[f[2][1]].y, a[f[2][2]].z, tex.a.x, tex.b.y },
            { a[f[3][0]].x, a[f[3][1]].y, a[f[3][2]].z, tex.b.x, tex.b.y },
        };
        int face_vis = LAB_cross_visibility[i];
        LAB_Builtin_WriteQuad(&quads[i], v, c, LAB_DIR_ALL, 0, face_vis);
    }
}


bool LAB_Builtin_ModelAddCross(LAB_Model* m,
                               LAB_Box3F aabb, LAB_Box2F tex,
                               LAB_Color c)
{
    LAB_ModelQuad* tris = LAB_Model_Extend(m, 4);
    if(tris == NULL) return false;
    LAB_Builtin_WriteCross(tris, aabb, tex, c);
    return true;
}