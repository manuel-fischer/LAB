#include "HTL/mem_config.h"
#include "HTL/mt_config.h"
#include "HTL/pp.h"
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL_thread.h>

typedef struct HTL_P(NAME)
{
    volatile HTL_P(TYPE)* queue;
    volatile size_t first, count, capacity;
    
    HTL_SEMAPHORE sem_cnt;
    HTL_SEMAPHORE sem_free;
    HTL_MUTEX     mtx_lock;
} HTL_P(NAME);


/********* LIFETIME *********/

bool HTL_MEMBER(Create)(HTL_P(NAME)* q, size_t capacity);
void HTL_MEMBER(Destroy)(HTL_P(NAME)* q);


/********* ENTRY ACCESS *********/

bool HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q);
bool HTL_MEMBER(IsFull)(HTL_P(NAME)* q);
void HTL_MEMBER(WaitPushBack)(HTL_P(NAME)* q, HTL_P(TYPE)* in);
void HTL_MEMBER(WaitPopFront)(HTL_P(NAME)* q, HTL_P(TYPE)* out);