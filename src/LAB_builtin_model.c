#include "LAB_builtin_model.h"

#include "LAB_direction.h"

#define LAB_SUBARR_ADDR(arr, type, subsize, index) \
    (((type(*)[subsize])(arr))[index])

#define LAB_TRI_ADDR(arr, subsize, index) \
    LAB_SUBARR_ADDR(arr, LAB_Triangle, subsize, index)


LAB_STATIC
void LAB_Builtin_WriteVertex(LAB_Vertex* vert, const float v[5], LAB_Color c, uint64_t flags)
{
    *vert = (LAB_Vertex){ 
        .x = v[0], v[1], v[2], 
        .color = c, 
        .u = v[3], v[4],
        .flags = flags  
    };
}

#define LAB_TRIANGLE_SIZE 1
LAB_STATIC
void LAB_Builtin_WriteTriangle(LAB_Triangle tri[LAB_TRIANGLE_SIZE],
                               const float v0[5], const float v1[5], const float v2[5], 
                               LAB_Color c,
                               uint64_t cull, uint64_t light, uint64_t vis)
{
    LAB_Builtin_WriteVertex(&tri->v[0], v0, c, cull);
    LAB_Builtin_WriteVertex(&tri->v[1], v1, c, light);
    LAB_Builtin_WriteVertex(&tri->v[2], v2, c, vis);
}

#define LAB_QUAD_SIZE (2*LAB_TRIANGLE_SIZE)
void LAB_Builtin_WriteQuad(LAB_Triangle tri[LAB_QUAD_SIZE],
                           const float v[4][5],
                           LAB_Color c,
                           uint64_t cull, uint64_t light, uint64_t vis)
{
    LAB_Builtin_WriteTriangle(&tri[0], v[1], v[2], v[0], c, cull, light, vis);
    LAB_Builtin_WriteTriangle(&tri[1], v[2], v[1], v[3], c, cull, light, vis);
}

bool LAB_Builtin_AddQuad(LAB_Model* m,
                         const float v[4][5],
                         LAB_Color c,
                         uint64_t cull, uint64_t light, uint64_t vis)
{
    LAB_Triangle* tris = LAB_Model_Extend(m, LAB_QUAD_SIZE);
    if(tris == NULL) return false;
    LAB_Builtin_WriteQuad(tris, v, c, cull, light, vis);
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

#define LAB_CUBE_SIZE (6*LAB_QUAD_SIZE)
void LAB_Builtin_WriteCube(LAB_Triangle tri[LAB_CUBE_SIZE],
                           const float aabb[2][3], const float tex[6][2][2],
                           const LAB_Color cs[6], LAB_Color c)
{
    for(int face = 0; face < 6; ++face)
    {
        const uint8_t (*f)[3] = LAB_cube_vertices[face];
        const float (*t)[2] = tex[face];
        const float v[4][5] = {
            aabb[f[0][0]][0], aabb[f[0][1]][1], aabb[f[0][2]][2], t[0][0], t[0][1],
            aabb[f[1][0]][0], aabb[f[1][1]][1], aabb[f[1][2]][2], t[1][0], t[0][1],
            aabb[f[2][0]][0], aabb[f[2][1]][1], aabb[f[2][2]][2], t[0][0], t[1][1],
            aabb[f[3][0]][0], aabb[f[3][1]][1], aabb[f[3][2]][2], t[1][0], t[1][1],
        };
        int face_s = 1 << face;
        int vis = LAB_APPROX_EQ(aabb[face&1][face>>1], (float)(face&1)) ? face_s : LAB_DIR_ALL;
        LAB_Builtin_WriteQuad(LAB_TRI_ADDR(tri,LAB_QUAD_SIZE,face), v, LAB_MulColor(cs[face], c), vis, face_s, vis);
    }
}

const LAB_Color LAB_cube_color_shade[6] = {
    LAB_RGBX(EBEBEB), LAB_RGBX(EBEBEB), // west east
    LAB_RGBX(C0C0C0), LAB_RGBX(FFFFFF), // down up
    LAB_RGBX(D7D7D7), LAB_RGBX(D7D7D7), // north south
};
const LAB_Color LAB_cube_color_flat[6] = {
    LAB_RGBX(FFFFFF), LAB_RGBX(FFFFFF), // west east
    LAB_RGBX(FFFFFF), LAB_RGBX(FFFFFF), // down up
    LAB_RGBX(FFFFFF), LAB_RGBX(FFFFFF), // north south
};

const float LAB_full_aabb[2][3] = { { 0, 0, 0 }, { 1, 1, 1 } };



bool LAB_Builtin_ModelAddCube(LAB_Model* m,
                              const float aabb[2][3], const float tex[6][2][2],
                              const LAB_Color cs[6], LAB_Color c)
{
    LAB_Triangle* tris = LAB_Model_Extend(m, LAB_CUBE_SIZE);
    if(tris == NULL) return false;
    LAB_Builtin_WriteCube(tris, aabb, tex, cs, c);
    return true;
}

bool LAB_Builtin_ModelAddCubeAll(LAB_Model* m,
                                 const float aabb[2][3], const float tex[2][2],
                                 const LAB_Color cs[6], LAB_Color c)
{
    float tex6[6][2][2];
    for(int i = 0; i < 6; ++i) memcpy(tex6[i], tex, sizeof(float[2][2]));
    return LAB_Builtin_ModelAddCube(m, aabb, (const float(*)[2][2])tex6, cs, c);
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

#define LAB_CROSS_SIZE (4*LAB_QUAD_SIZE)
bool LAB_Builtin_WriteCross(LAB_Triangle tri[LAB_CROSS_SIZE],
                            const float aabb[2][3], const float tex[2][2],
                            LAB_Color c)
{
    for(int i = 0; i < 4; ++i)
    {
        const uint8_t (*f)[3] = LAB_cross_vertices[i];
        const float v[4][5] = {
            aabb[f[0][0]][0], aabb[f[0][1]][1], aabb[f[0][2]][2], tex[0][0], tex[0][1],
            aabb[f[1][0]][0], aabb[f[1][1]][1], aabb[f[1][2]][2], tex[1][0], tex[0][1],
            aabb[f[2][0]][0], aabb[f[2][1]][1], aabb[f[2][2]][2], tex[0][0], tex[1][1],
            aabb[f[3][0]][0], aabb[f[3][1]][1], aabb[f[3][2]][2], tex[1][0], tex[1][1],
        };
        int face_vis = LAB_cross_visibility[i];
        LAB_Builtin_WriteQuad(LAB_TRI_ADDR(tri,LAB_QUAD_SIZE,i), v, c, LAB_DIR_ALL, 0, face_vis);
    }
}


bool LAB_Builtin_ModelAddCross(LAB_Model* m,
                               const float aabb[2][3], const float tex[2][2],
                               LAB_Color c)
{
    LAB_Triangle* tris = LAB_Model_Extend(m, LAB_CROSS_SIZE);
    if(tris == NULL) return false;
    LAB_Builtin_WriteCross(tris, aabb, tex, c);
    return true;
}