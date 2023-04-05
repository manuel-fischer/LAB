#include "LAB_config.h"

#ifdef __WINNT__
//#include <windows.h> // < longer build times
#include <sysinfoapi.h>
#else
#include <unistd.h>
#endif

size_t LAB_CoreCount(void)
{
    #ifdef __WINNT__
    SYSTEM_INFO sys_inf;
    GetSystemInfo(&sys_inf);
    return sys_inf.dwNumberOfProcessors;
    #else
    return sysconf(_SC_NPROCESSORS_ONLN);
    #endif
}