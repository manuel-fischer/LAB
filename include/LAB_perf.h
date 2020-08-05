#pragma once

#include <SDL2/SDL_timer.h> // -> SDL_GetPerformanceCounter, SDL_GetPerformanceFrequency

#define LAB_PERF_BEGIN(name, repeat) \
    static uint64_t LAB_PERF_alltime = 0; \
    static uint64_t LAB_PERF_count = 0; \
    uint64_t LAB_PERF_begin = SDL_GetPerformanceCounter(); \
    asm volatile (""); \
    for(int LAB_PERF_i = (repeat); LAB_PERF_i > 0; --LAB_PERF_i) \
    {

#define LAB_PERF_END(name) \
    } \
    asm volatile (""); \
    uint64_t LAB_PERF_end = SDL_GetPerformanceCounter(); \
    uint64_t LAB_PERF_diff = LAB_PERF_end - LAB_PERF_begin; \
    LAB_PERF_alltime += LAB_PERF_diff; \
    LAB_PERF_count++; \
    uint64_t freq = SDL_GetPerformanceFrequency(); \
    fprintf(stderr, "LAB_PERF " name " %i (%f sec), avg %f (%f sec)\n", (int)LAB_PERF_diff, (float)LAB_PERF_diff/freq, (float)LAB_PERF_alltime/(float)LAB_PERF_count, (float)LAB_PERF_alltime/(float)LAB_PERF_count/freq);
