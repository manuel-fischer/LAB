#include "LAB_debug.h"

#include <stdio.h>

void LAB_AssumptionFailed(const char* expr,
                          const char* file,
                          int line,
                          const char* function)
{
    fprintf(stderr, "Assumption failed at %s|%i%s%s:\n    %s\n",
            file, line, function?" in ":"", function, expr);
}

