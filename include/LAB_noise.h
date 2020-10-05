#pragma once

#include "LAB_opt.h"
#include "LAB_stdinc.h"



LAB_DEPRECATED("SLOW!")
void LAB_SmoothNoise2D(LAB_OUT uint32_t smooth[16*16],
                       LAB_IN uint64_t noise[17*17]);

LAB_DEPRECATED("SLOW!")
void LAB_SmoothNoise3D(LAB_OUT uint32_t smooth[16*16*16],
                       LAB_IN uint64_t noise[17*17+17]);
