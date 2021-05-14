#include "LAB_util.h"
#include "LAB_stdinc.h"

#include <time.h>

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

LAB_Nanos LAB_NanoSeconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    //timespec_get(&ts, TIME_UTC);
    return (LAB_Nanos)ts.tv_sec*1000000000ull + (LAB_Nanos)ts.tv_nsec;
}

size_t LAB_StrHash(const char* str)
{
    size_t hash = 0xabcdef;

    for(;*str; ++str)
        hash = ((hash << 5u) + hash) + (size_t)*str;

    return hash;
}
