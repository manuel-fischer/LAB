#pragma once

#include "LAB_memory.h"
#define HTL_CALLOC LAB_Calloc
#define HTL_MALLOC LAB_Malloc
#define HTL_FREE   LAB_Free


#include "LAB_debug.h"
#define HTL_ASSERT LAB_ASSERT


#include <SDL2/SDL.h>
#define HTL_MUTEX SDL_mutex*
#define HTL_MUTEX_CREATE(mtx) ((*(mtx) = SDL_CreateMutex()) != NULL)
#define HTL_MUTEX_DESTROY(mtx) SDL_DestroyMutex(*(mtx))
#define HTL_MUTEX_LOCK(mtx) SDL_LockMutex(*(mtx))
#define HTL_MUTEX_UNLOCK(mtx) SDL_UnlockMutex(*(mtx))

#define HTL_SEMAPHORE SDL_sem*
#define HTL_SEMAPHORE_CREATE(sem, count) ((*(sem) = SDL_CreateSemaphore(count)) != NULL)
#define HTL_SEMAPHORE_DESTROY(sem) SDL_DestroySemaphore(*(sem))
#define HTL_SEMAPHORE_WAIT(sem) SDL_SemWait(*(sem))
#define HTL_SEMAPHORE_POST(sem) SDL_SemPost(*(sem))