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


uint64_t LAB_NanoSeconds()
{
    struct timespec ts;
#ifdef __WINNT__
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    timespec_get(&ts, TIME_UTC);
#endif
    return  (uint64_t)ts.tv_sec*1000000000ull + ts.tv_nsec;
}

size_t LAB_StrHash(const char* str)
{
    size_t hash = 0xabcdef;

    for(;*str; ++str)
        hash = ((hash << 5) + hash) + (int)*str;

    return hash;
}
