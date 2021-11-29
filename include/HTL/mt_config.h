#pragma once

#if !defined HTL_MUTEX || \
    !defined HTL_MUTEX_CREATE || \
    !defined HTL_MUTEX_DESTROY || \
    !defined HTL_MUTEX_LOCK || \
    !defined HTL_MUTEX_UNLOCK
#error  An interface to a mutex should be defined
#endif

#ifndef HTL_MUTEX_CREATE
#error  HTL_MUTEX_CREATE should be defined
#endif

#ifndef HTL_FREE
#error  HTL_FREE should be defined
#endif