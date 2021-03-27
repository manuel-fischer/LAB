#pragma once

#include "LAB_opt.h"
typedef double LAB_NoiseFloat;

LAB_CONST LAB_NoiseFloat LAB_SimplexNoise2D(LAB_NoiseFloat xin, LAB_NoiseFloat yin);
LAB_CONST LAB_NoiseFloat LAB_SimplexNoise3D(LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin);
LAB_CONST LAB_NoiseFloat LAB_SimplexNoise4D(LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin, LAB_NoiseFloat win);
