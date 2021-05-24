#include "HTL_pp.h"
#include <stdlib.h>
#include <stdbool.h>

// priority queue
// TODO: implement as a fixed sized heap
// param NAME
// param TYPE
// param PRIO_TYPE
// param PRIO(TYPE e)
// param PRIO_HIGHER_THAN(PRIO_TYPE a, PRIO_TYPE b) return true if a is more important

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
HTL_P(TYPE)* HTL_MEMBER(Push)(HTL_P(NAME)* q, HTL_P(PRIO_TYPE) prio);
void HTL_MEMBER(PopFront)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(Front)(HTL_P(NAME)* q);