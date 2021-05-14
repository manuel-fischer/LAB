#include "HTL_pp.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct HTL_P(NAME)
{
    HTL_P(TYPE)* queue;
    size_t first, count, capacity;
} HTL_P(NAME);


/********* LIFETIME *********/

bool HTL_MEMBER(Create)(HTL_P(NAME)* q, size_t capacity);
void HTL_MEMBER(Destroy)(HTL_P(NAME)* q);


/********* ENTRY ACCESS *********/

bool HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q);
bool HTL_MEMBER(IsFull)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(PushBack)(HTL_P(NAME)* q);
void HTL_MEMBER(PopFront)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(Front)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(Back)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(Find)(HTL_P(NAME)* q, int (*comp1)(void* ctx, HTL_P(TYPE)* content), void* ctx);