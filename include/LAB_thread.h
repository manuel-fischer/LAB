#pragma once

#include <stdbool.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_mutex.h>
#include "LAB_debug.h"
#include "LAB_sdl.h"
#include <stdatomic.h>

bool LAB_InitThread(void);
bool LAB_IsMainThread(void);


#define LAB_SDL_LockMutex(mtx) LAB_SDL_CHECK(SDL_LockMutex(mtx))
#define LAB_SDL_UnlockMutex(mtx) LAB_SDL_CHECK(SDL_UnlockMutex(mtx))

#define LAB_SDL_CondBroadcast(cond) LAB_SDL_CHECK(SDL_CondBroadcast(cond))
#define LAB_SDL_CondSignal(cond) LAB_SDL_CHECK(SDL_CondSignal(cond))
#define LAB_SDL_CondWait(cond, mutex) LAB_SDL_CHECK(SDL_CondWait(cond, mutex))
#define LAB_SDL_CondWaitTimeout(cond, mutex, ms) LAB_SDL_CHECK(SDL_CondWaitTimeout(cond, mutex, ms))



typedef atomic_bool LAB_Access;
#define LAB_Access_TryLock(pflag) (!atomic_exchange_explicit(pflag, true, memory_order_acquire))
#define LAB_Access_Unlock(pflag) (atomic_store_explicit(pflag, false, memory_order_release))