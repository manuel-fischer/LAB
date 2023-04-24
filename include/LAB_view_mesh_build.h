#pragma once

#include "LAB_view_mesh.h"
#include "LAB_chunk_neighborhood.h"

typedef struct LAB_View_Mesh_BuildConfig
{
    //float exposure, saturation;
    bool flat_shade;
} LAB_View_Mesh_BuildConfig;


typedef struct LAB_View_Mesh_BuildArgs
{
    LAB_View_Mesh_BuildConfig cfg;
    LAB_View_Mesh* LAB_RESTRICT render_passes/*[LAB_RENDER_PASS_COUNT]*/;
    const LAB_BlockNbHood* LAB_RESTRICT blocknbh;
    const LAB_LightNbHood* LAB_RESTRICT lightnbh;
    LAB_DirSet visibility;
} LAB_View_Mesh_BuildArgs;


bool LAB_View_Mesh_BuildChunk(LAB_View_Mesh_BuildArgs ctx);