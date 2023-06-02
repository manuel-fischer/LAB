#pragma once

#include "LAB_vec.h"
#include "LAB_color.h"

typedef struct LAB_SkyVertex { LAB_Vec2F screen_pos; } LAB_SkyVertex;
typedef struct LAB_LineVertex { LAB_Vec3F pos; } LAB_LineVertex;

typedef struct LAB_GuiVertex { LAB_Vec2F pos_tex; } LAB_GuiVertex;
typedef struct LAB_FpsGraphVertex { LAB_Vec2F pos; LAB_Color color; } LAB_FpsGraphVertex;