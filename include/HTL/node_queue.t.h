#include "HTL/pp.h"
#include <stdlib.h>
#include <stdbool.h>

// NAME, TYPE, PREV (lvalue), NEXT (lvalue)

typedef struct HTL_P(NAME)
{
    HTL_P(TYPE)* head, **tail;
    size_t dbg_size;
} HTL_P(NAME);


/********* LIFETIME *********/

bool HTL_MEMBER(Create)(HTL_P(NAME)* q);
void HTL_MEMBER(Destroy)(HTL_P(NAME)* q);


/********* ENTRY ACCESS *********/

bool HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q);
bool HTL_MEMBER(PushBack)(HTL_P(NAME)* q, HTL_P(TYPE)* elem);
HTL_P(TYPE)* HTL_MEMBER(PopFront)(HTL_P(NAME)* q);
void HTL_MEMBER(Splice)(HTL_P(NAME)* q, HTL_P(TYPE)* elem);

void HTL_MEMBER(DbgValidate)(HTL_P(NAME)* q);