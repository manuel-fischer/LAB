#include "LAB_thread.h"


static SDL_threadID LAB_MainThreadID;

bool LAB_InitThread(void)
{
    LAB_MainThreadID = SDL_ThreadID();
    return !!LAB_MainThreadID;
}

bool LAB_IsMainThread(void)
{
    LAB_ASSERT(LAB_MainThreadID);
    return LAB_MainThreadID == SDL_ThreadID();
}
