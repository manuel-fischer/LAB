#pragma once

#ifdef NDEBUG
#  ifdef __GNUC__
#    define LAB_ASSUME(cond) do { if (!(cond)) LAB_UNREACHABLE(); } while(0)
#  else
#    define LAB_ASSUME(cond) (void)(0)
#  endif
#else
#  ifdef __GNUC__
#    define LAB_FUNCTION() __builtin_FUNCTION()
#  else
#    define LAB_FUNCTION() NULL
#  endif
#  define LAB_ASSUME(cond) do { \
    if(!(cond)) \
        LAB_AssumptionFailed(#cond, __FILE__, __LINE__, LAB_FUNCTION()); \
    } while(0)
#endif

void LAB_AssumptionFailed(const char* expr,
                          const char* file,
                          int line,
                          const char* function);
