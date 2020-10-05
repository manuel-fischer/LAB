#include "LAB_debug.h"

#include <stdio.h>
#include <signal.h>

void LAB_AssumptionFailed(const char* expr,
                          const char* file,
                          int line,
                          const char* function)
{
    fprintf(stderr, "Assumption failed at %s|%i%s%s:\n    %s\n",
            file, line, function?" in ":"", function, expr);
    //raise(SIGILL); // alt: SIGINT SIGBREAK SIGTRAP
    #ifdef __GNUC__
    __builtin_trap();
    #endif
    while(1);
}

