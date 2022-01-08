#include "LAB_debug.h"

#include <stdio.h>
#include <signal.h>
#include <stdarg.h>

#ifndef NDEBUG
    #ifdef __WINNT__
        // https://stackoverflow.com/questions/5693192/win32-backtrace-from-c-code
        #include <windows.h>
        #include <dbghelp.h>
        #include <imagehlp.h>

        void LAB_PrintStackTrace();
    #endif
#endif


#include <SDL2/SDL_thread.h>

static SDL_mutex* LAB_dbg_out_mtx;

void LAB_DbgInitOrAbort(void)
{
    LAB_dbg_out_mtx = SDL_CreateMutex();
    if(LAB_dbg_out_mtx == NULL)
    {
        fprintf(stderr, "Fatal Error: LAB_DbgInitOrAbort failed, aborting\n");
        abort();
    }
}

void LAB_DbgExit(void)
{
    SDL_DestroyMutex(LAB_dbg_out_mtx);
}

void LAB_DbgPrintf(const char* fmt, ...)
{
    if(SDL_LockMutex(LAB_dbg_out_mtx)) abort();
    va_list  ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fflush(stderr);
    va_end(ap);
    if(SDL_UnlockMutex(LAB_dbg_out_mtx)) abort();
}





#define LAB_HALT_ENTRIES 16
struct LAB_HaltEntry
{
    void(*handler)(void* user);
    void* user;
} LAB_halt_list[LAB_HALT_ENTRIES];

bool LAB_DbgAtHalt(void(*handler)(void* user), void* user)
{
    for(int i = 0; i < LAB_HALT_ENTRIES; ++i)
    {
        if(LAB_halt_list[i].handler == NULL)
        {
            LAB_halt_list[i].handler = handler;
            LAB_halt_list[i].user    = user;
            return true;
        }
    }
    return false;
}

void LAB_DbgRemoveHalt(void(*handler)(void* user), void* user)
{
    for(int i = 0; i < LAB_HALT_ENTRIES; ++i)
    {
        if(LAB_halt_list[i].handler == handler && 
           LAB_halt_list[i].user == user)
        {
            LAB_halt_list[i].handler = NULL;
            LAB_halt_list[i].user    = NULL;
            return;
        }
    }
    // TODO not found
}

void LAB_DbgDoHalt(void)
{
    for(int i = 0; i < LAB_HALT_ENTRIES; ++i)
    {
        if(LAB_halt_list[i].handler != NULL)
        {
            LAB_halt_list[i].handler(LAB_halt_list[i].user);
        }
    }
}




void LAB_AssumptionFailed(const char* type,
                          const char* expr,
                          const char* file,
                          int line,
                          const char* function,
                          int trap)
{
    LAB_DbgDoHalt();

    LAB_DbgPrintf("Checking %s failed at %s:%i%s%s:\n    %s\n",
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
        BOOL success;


        // capture backtrace
        HANDLE process = GetCurrentProcess();
        success = SymInitialize(process, NULL, TRUE);
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

            success = SymFromAddr(process, (DWORD64)(stack[i]), &displacement, psymbol);
            //BOOL success = SymGetSymFromAddr64(process, (DWORD64)(stack[i]), &displacement, psymbol);
            if(!success)
            {
                wchar_t buf[256];
                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
                fprintf(stderr, "    [%2i] %p : ??\n", i, (void*)stack[i]);
                fprintf(stderr, "        %ls\n",buf);
            }
            //else
            {
                fprintf(stderr, "    [%2i] %p,%p+%llx : %s\n", i, stack[i], (void*)psymbol->Address, displacement, psymbol->Name);
            }
        }
        SymCleanup(process);
    }
    end_win_stacktrace:;
}
#endif
#endif



#ifdef LAB_DBG_SET_BREAKPOINT
static volatile int LAB_DbgVar = 0;
void LAB_DbgBreak(void)
{
    // Set breakpoint here
    LAB_DbgPrintf("Breakpoint reached\n");
    getchar();
}
#endif