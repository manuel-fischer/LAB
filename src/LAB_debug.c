#include "LAB_debug.h"

#include <stdio.h>
#include <signal.h>

#ifndef NDEBUG
    #ifdef __WINNT__
        // https://stackoverflow.com/questions/5693192/win32-backtrace-from-c-code
        #include <windows.h>
        #include <dbghelp.h>
        #include <imagehlp.h>

        void LAB_PrintStackTrace();
    #endif
#endif

void LAB_AssumptionFailed(const char* type,
                          const char* expr,
                          const char* file,
                          int line,
                          const char* function,
                          int trap)
{
    fprintf(stderr, "Checking %s failed at %s:%i%s%s:\n    %s\n",
            type, file, line, function?" in ":"", function, expr);
    //raise(SIGILL); // alt: SIGINT SIGBREAK SIGTRAP
    #if !defined NDEBUG && defined __WINNT__ && !defined __GNUC__
    LAB_PrintStackTrace();
    #endif

    if(trap)
    {
        #ifdef __GNUC__
        __builtin_trap();
        #else
        while(1);
        #endif
    }
}

#ifndef NDEBUG
#ifdef __WINNT__
void LAB_PrintStackTrace()
{
    // TODO: PDB-File required?

    // TODO: Multithreading
    {
        // capture backtrace
        HANDLE process = GetCurrentProcess();
        BOOL success = SymInitialize(process, NULL, TRUE);
        if(!success)
        {
            fprintf(stderr, "Cannot print stacktrace\n");
            goto end_win_stacktrace;
        }
        DWORD sym_options = SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME;
        SymSetOptions(sym_options);

        static void* stack[128];
        unsigned short frames = CaptureStackBackTrace(0, 128, stack, NULL);

        //printf("%lli\n", (long long)stack[0]-(long long)&LAB_PrintStackTrace);


        fprintf(stderr, "Backtrace [%i]:\n", frames);
        for(unsigned i = 0; i < frames; ++i)
        {
            #define SYMBOL_LEN 512
            _Alignas(SYMBOL_INFO) char symbol_buf[sizeof(SYMBOL_INFO) + SYMBOL_LEN*sizeof(TCHAR)];
            memset(symbol_buf, 0, sizeof symbol_buf);
            PSYMBOL_INFO psymbol = (PSYMBOL_INFO)symbol_buf;
            psymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            psymbol->MaxNameLen = SYMBOL_LEN-1;

            /*_Alignas(IMAGEHLP_SYMBOL64) char symbol_buf[sizeof(IMAGEHLP_SYMBOL64) + SYMBOL_LEN*sizeof(TCHAR)];
            memset(symbol_buf, 0, sizeof symbol_buf);
            PIMAGEHLP_SYMBOL64 psymbol = (PIMAGEHLP_SYMBOL64)symbol_buf;
            psymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            psymbol->MaxNameLength = SYMBOL_LEN;*/

            DWORD64 displacement = 0;

            BOOL success = SymFromAddr(process, (DWORD64)(stack[i]), &displacement, psymbol);
            //BOOL success = SymGetSymFromAddr64(process, (DWORD64)(stack[i]), &displacement, psymbol);
            if(!success)
            {
                wchar_t buf[256];
                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
                fprintf(stderr, "    [%2i] %p : ??\n", i, stack[i]);
                fprintf(stderr, "        %ls\n",buf);
            }
            //else
            {
                fprintf(stderr, "    [%2i] %p,%p+%x : %s\n", i, stack[i], psymbol->Address, displacement, psymbol->Name);
            }
        }
        SymCleanup(process);
    }
    end_win_stacktrace:;
}
#endif
#endif
