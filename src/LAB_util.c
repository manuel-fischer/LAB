#include "LAB_util.h"
#include "LAB_stdinc.h"

#include <time.h>

const int LAB_offset[6][3] = {
    {-1, 0, 0},
    { 1, 0, 0},
    { 0,-1, 0},
    { 0, 1, 0},
    { 0, 0,-1},
    { 0, 0, 1},
};


const int LAB_offsetA[3][3] = {
    { 0, 1, 0 },
    { 0, 0, 1 },
    { 1, 0, 0 },
};
const int LAB_offsetB[3][3] = {
    { 0, 0, 1 },
    { 1, 0, 0 },
    { 0, 1, 0 },
};


const char* LAB_Filename(const char* path)
{
    size_t i = strlen(path);
    while(i --> 0)
    {
        if(path[i] == '/' || path[i] == '\\')
            return path+i+1;
    }
    return path;
}


uint64_t LAB_NanoSeconds()
{
    struct timespec ts;
    //timespec_get(&ts, TIME_UTC);
    clock_gettime(CLOCK_REALTIME, &ts);
    return  (uint64_t)ts.tv_sec*1000000000ull + ts.tv_nsec;
}
