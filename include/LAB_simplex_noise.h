#pragma once

#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_stdinc.h"

// export LAB_NoiseFloat
// export LAB_SimplexNoise*

typedef double LAB_NoiseFloat;

LAB_CONST LAB_NoiseFloat LAB_SimplexNoise2D(LAB_NoiseFloat xin, LAB_NoiseFloat yin);
LAB_CONST LAB_NoiseFloat LAB_SimplexNoise3D(LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin);
LAB_CONST LAB_NoiseFloat LAB_SimplexNoise4D(LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin, LAB_NoiseFloat win);

LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoise2DS(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin)
{
    return LAB_SimplexNoise2D(xin+(seed & 0xffff), yin+(seed>>16 & 0xffff));
}

LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoise3DS(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin)
{
    return LAB_SimplexNoise3D(xin+(seed & 0xffff), yin+(seed>>16 & 0xffff), zin+(seed>>32 & 0xffff));
}

LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoise4DS(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin, LAB_NoiseFloat win)
{
    return LAB_SimplexNoise4D(xin+(seed & 0xffff), yin+(seed>>16 & 0xffff), zin+(seed>>32 & 0xffff), win+(seed>>48 & 0xffff));
}
