#pragma once

#define GL_GLEXT_PROTOTYPES

#ifdef LAB_USE_GLEW
#include <GL/glew.h>
#define LAB_USES_GLEW
//#define GL3_PROTOTYPES
#else
#include <GLES3/gl3.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glext.h>

#include <stdio.h>

#include <SDL2/SDL.h>

#include "LAB_vec.h"
#include "LAB_mat.h"
#include "LAB_obj.h"

#include "LAB_gl_types.h"
#include "LAB_color.h"
#include "LAB_color_hdr.h"

const char* LAB_GL_GetError(GLenum errorid);
const char* LAB_GL_GetCurrentError(void);

void LAB_GL_Check(const char* file, int line);

#ifndef NDEBUG
void LAB_GL_SetupDebug(void);
void LAB_GL_CheckClear(void);
void LAB_GL_IgnoreInfo(GLenum id);
void LAB_GL_ResetIgnoreInfo(void);
#else
#define LAB_GL_SetupDebug() ((void)0)
#define LAB_GL_CheckClear() ((void)0)
#define LAB_GL_IgnoreInfo(id) ((void)0)
#define LAB_GL_ResetIgnoreInfo() ((void)0)
#endif


#define LAB_GL_CHECK() LAB_GL_Check(__FILE__, __LINE__)
#define LAB_GL_DBG_CHECK() LAB_GL_CHECK()

#ifndef NDEBUG
extern int LAB_gl_debug_alloc_count;

/*#define LAB_GL_DEBUG_ALLOC(count, array) do { \
    LAB_gl_debug_alloc_count += (count); \
} while(0)
#define LAB_GL_DEBUG_FREE(count, array) do { \
    LAB_gl_debug_alloc_count -= (count); \
} while(0)*/

#define LAB_GL_DEBUG_ALLOC_(count, array) do { \
    for(int LAB_GL_DEBUG_FREE_i = 0; LAB_GL_DEBUG_FREE_i < (count); ++LAB_GL_DEBUG_FREE_i) \
        if((array)[LAB_GL_DEBUG_FREE_i]) ++LAB_gl_debug_alloc_count; \
        else LAB_ASSUME(!"failed to allocate for " #array " in OpenGL"); \
} while(0)
#define LAB_GL_DEBUG_FREE_(count, array) do { \
    for(int LAB_GL_DEBUG_FREE_i = 0; LAB_GL_DEBUG_FREE_i < (count); ++LAB_GL_DEBUG_FREE_i) \
        if((array)[LAB_GL_DEBUG_FREE_i]) --LAB_gl_debug_alloc_count; \
} while(0)
#else
#define LAB_GL_DEBUG_ALLOC_(count, array) ((void)0)
#define LAB_GL_DEBUG_FREE_(count, array) ((void)0)
#endif
#define LAB_GL_ALLOC(function, count, array) do { \
    function(count, array); \
    LAB_GL_DEBUG_ALLOC_(count, array); \
} while(0)
#define LAB_GL_FREE(function, count, array) do { \
    LAB_GL_DEBUG_FREE_(count, array); \
    function(count, array); \
} while(0)



#ifndef NDEBUG
#define LAB_GL_DEBUG_OBJ_ALLOC() (++LAB_gl_debug_alloc_count, LAB_MakeTrue())
#define LAB_GL_DEBUG_OBJ_FREE() (--LAB_gl_debug_alloc_count, LAB_MakeTrue())
#else
#define LAB_GL_DEBUG_OBJ_ALLOC() (LAB_MakeTrue())
#define LAB_GL_DEBUG_OBJ_FREE() (LAB_MakeTrue())
#endif

#define LAB_GL_OBJ_ALLOC(function, id_element) ( \
    function(1, &(id_element)->id), \
    (id_element)->id != 0 && (LAB_GL_DEBUG_OBJ_ALLOC(), true) \
)
#define LAB_GL_OBJ_ALLOC_TARGET(function, target, id_element) ( \
    function(target, 1, &(id_element)->id), \
    (id_element)->id != 0 && (LAB_GL_DEBUG_OBJ_ALLOC(), true) \
)
#define LAB_GL_OBJ_FREE(function, id_element) ( \
    (void)((id_element)->id != 0 && (LAB_GL_DEBUG_OBJ_FREE())), \
    function(1, &(id_element)->id), \
    (void)0 \
)

#define LAB_GL_OBJ_ALLOC1(expr) ( \
    (expr) != 0 && (LAB_GL_DEBUG_OBJ_ALLOC(), true) \
)
#define LAB_GL_OBJ_FREE1(function, expr) ( \
    (void)((expr) != 0 && (function(expr), LAB_GL_DEBUG_OBJ_FREE())) \
)

#define LAB_GL_OBJ_ID_ALLOC(expr) ( \
    LAB_GL_ID_OK(expr) && (LAB_GL_DEBUG_OBJ_ALLOC(), true) \
)
#define LAB_GL_OBJ_ID_FREE(function, expr) ( \
    LAB_GL_ID_OK(expr) && (function(expr), LAB_GL_DEBUG_OBJ_FREE()) \
)

#ifdef NDEBUG
#define LAB_GL_REQUIRE(flag) ((void)0)
#define LAB_GL_REQUIRE_NOT(flag) ((void)0)
#else
#include "LAB_debug.h"
#define LAB_GL_REQUIRE(flag) do \
{ \
    GLboolean LAB_GL_REQUIRE_status; \
    glGetBooleanv(flag, &LAB_GL_REQUIRE_status); \
    LAB_ASSUME2("GL state check", LAB_GL_REQUIRE_status, #flag); \
} while(0)
#define LAB_GL_REQUIRE_NOT(flag) do \
{ \
    GLboolean LAB_GL_REQUIRE_status; \
    glGetBooleanv(flag, &LAB_GL_REQUIRE_status); \
    LAB_ASSUME2("GL state check", !LAB_GL_REQUIRE_status, "not " #flag); \
} while(0)
#endif


#define LAB_GL_TYPEOF_MAPPING(X)  \
    X(GLbyte,   GL_BYTE),           \
    X(GLubyte,  GL_UNSIGNED_BYTE),  \
    X(GLshort,  GL_SHORT),          \
    X(GLushort, GL_UNSIGNED_SHORT), \
    X(GLint,    GL_INT),            \
    X(GLuint,   GL_UNSIGNED_INT),   \
    X(GLfloat,  GL_FLOAT),          \
    X(GLdouble, GL_DOUBLE)
    //GLfixed:  GL_FIXED,
    //GLhalf:   GL_HALF_FLOAT)

#define LAB_GL_TYPEOF_DUMMY_VEC ((LAB_Vec3I){0})
#define LAB_GL_TYPEOF_X_CASE(t, tid) t:tid
#define LAB_GL_TYPEOF_X_AS_VEC(t, tid) t:LAB_GL_TYPEOF_DUMMY_VEC

#define LAB_GL_TYPEOF_IMPL_AS_VEC(expr) \
    _Generic(expr, \
        LAB_GL_TYPEOF_MAPPING(LAB_GL_TYPEOF_X_AS_VEC), \
        default: expr \
    )

/**
 *  Returns the OpenGL type index
 */
#define LAB_GL_SCALAR_TYPE(expr) \
    _Generic(expr, \
        LAB_GL_TYPEOF_MAPPING(LAB_GL_TYPEOF_X_CASE) \
    )

#define LAB_GL_ELEMENT_TYPE(expr) \
    _Generic(expr, \
        LAB_Color: GL_UNSIGNED_BYTE, \
        default: _Generic(LAB_GL_TYPEOF_IMPL_AS_VEC(expr).x, \
            LAB_GL_TYPEOF_MAPPING(LAB_GL_TYPEOF_X_CASE) \
        ) \
    )

#define LAB_GL_MEMBER_ELEMENT_TYPE(tp, member) LAB_GL_ELEMENT_TYPE(((tp*)NULL)->member)

void LAB_GL_ActivateTexture(LAB_GL_Texture* tex);

void LAB_GL_UploadSurf(LAB_GL_Texture tex, SDL_Surface* surf);

void LAB_GL_SetMatrix(GLenum mode, LAB_Mat4F matrix);
void LAB_GL_SetMatrix_Identity(GLenum mode);

void LAB_GL_UniformColor(LAB_GL_Uniform uniform, LAB_Color color);
void LAB_GL_UniformColorHDR(LAB_GL_Uniform uniform, LAB_ColorHDR color);

#define LAB_GL_SetMatrix_0(mode) LAB_GL_SetMatrix_Identity(mode)
#define LAB_GL_SetMatrix_1(mode, a) LAB_GL_SetMatrix(mode, a)
#define LAB_GL_SetMatrix_2(mode, a, b) LAB_GL_SetMatrix(mode, LAB_Mat4F_Chain(a, b))
#define LAB_GL_SetMatrix_3(mode, a, b, c) LAB_GL_SetMatrix(mode, LAB_REDUCE_3(LAB_Mat4F_Chain, a, b, c))
#define LAB_GL_SetMatrix_4(mode, a, b, c, d) LAB_GL_SetMatrix(mode, LAB_REDUCE_4(LAB_Mat4F_Chain, a, b, c, d))
#define LAB_GL_SetMatrix_5(mode, a, b, c, d, e) LAB_GL_SetMatrix(mode, LAB_REDUCE_5(LAB_Mat4F_Chain, a, b, c, d, e))

#define LAB_GL_SetCurrentMatrix_0 LAB_GL_SetMatrix_0
#define LAB_GL_SetCurrentMatrix_1 LAB_GL_SetMatrix_1
#define LAB_GL_SetCurrentMatrix_2 LAB_GL_SetMatrix_2
#define LAB_GL_SetCurrentMatrix_3 LAB_GL_SetMatrix_3
#define LAB_GL_SetCurrentMatrix_4 LAB_GL_SetMatrix_4
#define LAB_GL_SetCurrentMatrix_5 LAB_GL_SetMatrix_5

int LAB_GL_GetInt(GLenum e);
#define LAB_GL_GetUInt(e) (unsigned)LAB_GL_GetInt(e)



/**
 *  fix screen data fetched from opengl
 *
 *  flip image horizontally and set alpha channel to 255
 */
void LAB_GL_FixScreenImg(void* pixels, int w, int h);


#ifndef LAB_GL_NO_POISON

// legacy matrix operations
#pragma GCC poison glPushMatrix
#pragma GCC poison glPopMatrix
#pragma GCC poison glMatrixMode
#pragma GCC poison glScalef
#pragma GCC poison glScaled
#pragma GCC poison glTranslatef
#pragma GCC poison glTranslated
#pragma GCC poison glRotatef
#pragma GCC poison glRotated

#pragma GCC poison glMultMatrix
#pragma GCC poison glFrustum

#endif