#include "LAB_timelib.h"
#include <time.h>

LAB_Nanos LAB_NanoSeconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    //timespec_get(&ts, TIME_UTC);
    return (LAB_Nanos)ts.tv_sec*1000000000ull + (LAB_Nanos)ts.tv_nsec;
}