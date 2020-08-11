/**
 * Fixed sized Queue          TODO
 *
 * PARAMS:
 *    NAME       name of the generated type, prefix of all functions
 *    TYPE       content type
 *    CAPACITY   maximum amount of elements
 */

#include "HTL_pp.h"
#include <stdlib.h>

typedef struct HTL_P(NAME)
{
    HTL_P(TYPE) queue[HTL_P(CAPACITY)];
    size_t first, count;
} HTL_P(NAME);


/********* LIFETIME *********/

/**
 *  Construct an empty queue at the referenced location
 *  Return 1 on success, 0 on failure
 *  The function never fails, because no memory allocations
 *  are done
 *
 *  PRE:  queue was not already constructed
 *
 *  POST: the queue is valid to be used with the other functions
 *
 *  Alternatively the bytes could be set to 0
 */
int  HTL_MEMBER(Construct)(HTL_P(NAME)* q);

/**
 *  Destruct a queue at the referenced location
 *  Nothing happens, the function exists only for formal reasons
 *
 *  POST: All references to entries get invalidated, because
 *        the array gets freed, Construct could be called
 *        again
 */
void HTL_MEMBER(Destruct)(HTL_P(NAME)* q);


/********* ENTRY ACCESS *********/

/**
 *  Returns 1 if queue is empty
 */
int  HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q);

/**
 *  Creates room for an element at the back
 *
 *  Returns the allocated entry on success,
 *  NULL on failure
 *  It fails, when the queue is full
 *
 *  The element can be inserted by writing to Back()
 */
HTL_P(TYPE)* HTL_MEMBER(PushBack)(HTL_P(NAME)* q);

/**
 *  removes Element from the front
 *
 *  POST: References to the front get invalidated
 */
void HTL_MEMBER(PopFront)(HTL_P(NAME)* q);

/**
 *  Get Reference to the front
 *
 *  PRE:  The queue is not empty
 *  POST: Return value points to a valid memory location
 */
HTL_P(TYPE)* HTL_MEMBER(Front)(HTL_P(NAME)* q);

/**
 *  Get Reference to the back
 *
 *  PRE:  The queue is not empty
 *  POST: Return value points to a valid memory location
 */
HTL_P(TYPE)* HTL_MEMBER(Back)(HTL_P(NAME)* q);
