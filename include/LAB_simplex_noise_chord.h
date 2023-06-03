#pragma once

#include "LAB_simplex_noise.h"
#include <math.h>
#include <tgmath.h>

// export LAB_SimplexNoiseChord*

// Functions usually output values between [-1, +1], but values up to [-2, +2] are possible

LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoiseChord2DS(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, size_t octaves)
{
    LAB_NoiseFloat v = 0;
    LAB_NoiseFloat freq = 1;
    LAB_NoiseFloat factor = 1;

    LAB_NoiseFloat xr = xin + (seed       & 0xffff);
    LAB_NoiseFloat yr = yin + (seed >> 16 & 0xffff);

    for(size_t o = 0; o <= octaves; ++o)
    {
        v += LAB_SimplexNoise2D(xr*freq, yr*freq)*factor;
        factor *= 0.5;
        freq *= 2.;
    }
    return v;
}


LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoiseChord3DS(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin, size_t octaves)
{
    LAB_NoiseFloat v = 0;
    LAB_NoiseFloat freq = 1;
    LAB_NoiseFloat factor = 1;

    LAB_NoiseFloat xr = xin + (seed       & 0xffff);
    LAB_NoiseFloat yr = yin + (seed >> 16 & 0xffff);
    LAB_NoiseFloat zr = zin + (seed >> 32 & 0xffff);

    for(size_t o = 0; o <= octaves; ++o)
    {
        v += LAB_SimplexNoise3D(xr*freq, yr*freq, zr*freq)*factor;
        factor *= 0.5;
        freq *= 2.;
    }
    return v;
}


LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoiseChord4DS(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin, LAB_NoiseFloat win, size_t octaves)
{
    LAB_NoiseFloat v = 0;
    LAB_NoiseFloat freq = 1;
    LAB_NoiseFloat factor = 1;

    LAB_NoiseFloat xr = xin + (seed       & 0xffff);
    LAB_NoiseFloat yr = yin + (seed >> 16 & 0xffff);
    LAB_NoiseFloat zr = zin + (seed >> 32 & 0xffff);
    LAB_NoiseFloat wr = zin + (seed >> 48 & 0xffff);

    for(size_t o = 0; o <= octaves; ++o)
    {
        v += LAB_SimplexNoise4D(xr*freq, yr*freq, zr*freq, wr*freq)*factor;
        factor *= 0.5;
        freq *= 2.;
    }
    return v;
}


LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoiseChordNormalize(LAB_NoiseFloat f)
{
    return (f / ((LAB_NoiseFloat)0.5+fabs(f))) * (LAB_NoiseFloat)1.25;
}


// normalized-functions
LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoiseChord2DSN(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, size_t octaves)
{
    return LAB_SimplexNoiseChordNormalize(LAB_SimplexNoiseChord2DS(seed, xin, yin, octaves));
}

LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoiseChord3DSN(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin, size_t octaves)
{
    return LAB_SimplexNoiseChordNormalize(LAB_SimplexNoiseChord3DS(seed, xin, yin, zin, octaves));
}

LAB_CONST LAB_INLINE
LAB_NoiseFloat LAB_SimplexNoiseChord4DSN(uint64_t seed, LAB_NoiseFloat xin, LAB_NoiseFloat yin, LAB_NoiseFloat zin, LAB_NoiseFloat win, size_t octaves)
{
    return LAB_SimplexNoiseChordNormalize(LAB_SimplexNoiseChord4DS(seed, xin, yin, zin, win, octaves));
}