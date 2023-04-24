#pragma once

#include "LAB_attr.h"
#include "LAB_debug_options.h"
#include "LAB_stdinc.h"

// Render passes         ALPHA_TEST           BLEND            MIPMAPPING       CHUNK_ORDER    CHUNK_SORTED  DEPTH_MASK  DEPTH_TEST  DEPTH_OFFSET  IMPLEMENTED
// RENDER_PASS_SOLID         no                 no                yes             forward           no           yes        yes           no           yes
// RENDER_PASS_MASKED      >=32                 no                yes             forward           no           yes        yes           no           yes
// RENDER_PASS_BLIT        >=32                 no                 no             forward           no           yes        yes           no           yes
// -- QUERIES --
// RENDER_PASS_OVERLAY     >= 1       src*src.a+dst*(1-src.a)     yes             backward          no           yes        yes          yes            no
// RENDER_PASS_ALPHA       >= 1       src*src.a+dst*(1-src.a)     yes             backward         yes           yes        yes           no           yes
// RENDER_PASS_MULT2       >= 1       src*dst  +dst*src           yes            don't care         no            no        yes           no            no
// RENDER_PASS_ADD         >= 1       src*1    +dst*1             yes            don't care         no            no        yes           no            no



typedef unsigned LAB_RenderPass;
enum
{
    LAB_RENDER_PASS_SOLID = 0,
    LAB_RENDER_PASS_MASKED,
    LAB_RENDER_PASS_BLIT,
    LAB_RENDER_PASS_ALPHA,
    // LAB_RENDER_PASS_MULT2,
    // LAB_RENDER_PASS_ADD,
    LAB_RENDER_PASS_COUNT
};

bool LAB_PrepareRenderPass(LAB_RenderPass pass);
