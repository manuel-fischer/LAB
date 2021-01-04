/** \file HTL_queue.t.h
 *
 *  Fixed sized Queue
 *
 *  Template header (multiple inclusion)
 *
 *  \param NAME       name of the generated type, prefix of all functions
 *  \param TYPE       content type
 *  \param CAPACITY   maximum amount of elements
 */

#include "HTL_pp.h"
#include <stdlib.h>
#include <stdbool.h>

#ifndef DOXYGEN

typedef struct HTL_P(NAME)
{
    HTL_P(TYPE) queue[HTL_P(CAPACITY)];
    size_t first, count;
} HTL_P(NAME);


/********* LIFETIME *********/

bool HTL_MEMBER(Construct)(HTL_P(NAME)* q);
void HTL_MEMBER(Destruct)(HTL_P(NAME)* q);


/********* ENTRY ACCESS *********/

bool HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q);
bool HTL_MEMBER(IsFull)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(PushBack)(HTL_P(NAME)* q);
void HTL_MEMBER(PopFront)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(Front)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(Back)(HTL_P(NAME)* q);
HTL_P(TYPE)* HTL_MEMBER(Find)(HTL_P(NAME)* q, int (*comp1)(void* ctx, HTL_P(TYPE)* content), void* ctx);

#else /* DOXYGEN SECTION */

typedef struct Queue
{
    ContentType queue[CAPACITY];
    size_t first, count;
} Queue;


/********* LIFETIME *********/

/**
 *  Construct an empty queue at the referenced location.
 *
 *  \return 1 on success, 0 on failure
 *
 *  The function never fails, because no memory allocations
 *  are done, thus always 1 is returned
 *
 *  Alternatively the bytes could be set to 0
 *
 *  \pre  queue was not already constructed
 *
 *  \post the queue is valid to be used with the other functions
 *
 */
bool Queue_Construct(Queue* q);

/**
 *  Destruct a queue at the referenced location.
 *  Nothing happens, the function exists only for formal reasons.
 *
 *  \post All references to entries get invalidated, because
 *        the array gets freed, Construct could be called
 *        again
 */
void Queue_Destruct(Queue* q);


/********* ENTRY ACCESS *********/

/**
 *  \return 1 if queue is empty
 */
bool Queue_IsEmpty(Queue* q);

/**
 *  \return 1 if queue is full
 */
bool Queue_IsFull(Queue* q);

/**
 *  Creates room for an element at the back
 *
 *  \return the allocated entry on success (that is \c Back() ),
 *          \c NULL on failure.
 *          It fails, when the queue is full
 */
ContentType* Queue_PushBack(Queue* q);

/**
 *  Removes element from the front
 *
 *  \post References to the front get invalidated
 */
void Queue_PopFront(Queue* q);

/**
 *  \return reference to the front
 *
 *  \pre  The queue is not empty
 *  \post Return value points to a valid memory location
 */
ContentType* Queue_Front(Queue* q);

/**
 *  \return reference to the back
 *
 *  \pre  The queue is not empty
 *  \post Return value points to a valid memory location
 */
ContentType* Queue_Back(Queue* q);

/**
 *  Find an element in the queue
 *
 *  \return element that compares 0, otherwise \c NULL
 */
ContentType* Queue_Find(Queue* q, int (*comp1)(void* ctx, ContentType* content), void* ctx);

#endif
