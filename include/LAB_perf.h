#pragma once

#include <time.h> // -> clock

#define LAB_PERF_BEGIN(name, repeat) \
    clock_t LAB_PERF_begin = clock(); \
    asm volatile (""); \
    for(int LAB_PERF_i = (repeat); LAB_PERF_i > 0; --LAB_PERF_i) \
    {

#define LAB_PERF_END(name) \
    } \
    asm volatile (""); \
    clock_t LAB_PERF_end = clock(); \
    fprintf(stderr, "LAB_PERF " name " %lf/%f (%f sec)\n", (double)(LAB_PERF_end - LAB_PERF_begin), (float)CLOCKS_PER_SEC, (float)(LAB_PERF_end - LAB_PERF_begin)/CLOCKS_PER_SEC);
