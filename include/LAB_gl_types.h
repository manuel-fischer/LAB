#pragma once

#include "LAB_attr.h"
#include "LAB_opt.h"

typedef unsigned LAB_GL_Enum;
typedef unsigned LAB_GL_UInt;
typedef int LAB_GL_Int;

// typesafe id-types
#define LAB_GL_ID_TYPE_U(name) \
    typedef struct name { unsigned id; } name

#define LAB_GL_ID_TYPE_I(name) \
    typedef struct name { int id; } name


LAB_GL_ID_TYPE_U(LAB_GL_Program);
LAB_GL_ID_TYPE_U(LAB_GL_Shader);
LAB_GL_ID_TYPE_I(LAB_GL_Attr);
LAB_GL_ID_TYPE_I(LAB_GL_Uniform);
LAB_GL_ID_TYPE_U(LAB_GL_Array);
LAB_GL_ID_TYPE_U(LAB_GL_Buffer);
LAB_GL_ID_TYPE_U(LAB_GL_Texture);
LAB_GL_ID_TYPE_U(LAB_GL_Query);

LAB_VALUE_CONST LAB_ALWAYS_INLINE LAB_INLINE
bool LAB_GL_ID_OK_Unsigned(unsigned id) { return id != 0; }

LAB_VALUE_CONST LAB_ALWAYS_INLINE LAB_INLINE
bool LAB_GL_ID_OK_Signed(int id) { return id >= 0; }

#define LAB_GL_ID_OK(typed_id) (_Generic((typed_id).id, \
    unsigned: LAB_GL_ID_OK_Unsigned, \
    int:      LAB_GL_ID_OK_Signed)((typed_id).id))

#define LAB_GL_NULL_ID(type) (_Generic(((type){0}).id, \
    unsigned: (type){0}, \
    int:      (type){-1}))

