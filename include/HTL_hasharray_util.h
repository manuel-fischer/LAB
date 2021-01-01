#pragma once
/** \file HTL_hasharray_util.h
 *
 *  Utility macros for hash arrays
 */


/** \def HTL_HASHARRAY_EACH(array_type, arr, e, ...)
 *
 *  Iterate through the hash array.
 *  Receive pointers to the elements.
 *
 *  \param array_type  hash array type,         text (no alias)
 *  \param arr         pointer to a hasharray
 *  \param e           entry pointer,           by name (lvalue)
 *  \param ...         iteration statement,     code block
 */
#define HTL_HASHARRAY_EACH(array_type, arr, e, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=&(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), (e))) { __VA_ARGS__ } \
    } \
} while(0)

/** \def HTL_HASHARRAY_REMOVE(array_type, arr, e, cond, ...)
 *
 *  Iterate through the hash array and remove entries.
 *  Receive pointers to the elements.
 *
 *  \param array_type  hash array type,         text (no alias)
 *  \param arr         pointer to a hasharray
 *  \param e           entry pointer,           by name (lvalue)
 *  \param cond        condition,               by name (re-evaluated)
 *  \param ...         cleanup statement,       code block
 *
 *  \attention
 *      Do not jump out of \a ... after invalidation.
 *      Use \a cond to skip elements instead!
 *      Otherwise the entry wouldn't be removed correctly.
 *
 *  \todo there should be a way to break out of the loop directly
 */
#define HTL_HASHARRAY_REMOVE(array_type, arr, e, cond, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=&(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), (e)) && (cond)) \
        { \
            __VA_ARGS__ \
            array_type##_RemoveEntry((arr), (e)); \
            --HTL_HASHARRAY_i; /* repeat */ \
        } \
    } \
} while(0)



/** \def HTL_HASHARRAY_EACH_DEREF(array_type, arr, e, ...)
 *
 *  Iterate through the hash array.
 *  Receive value copy of the elements.
 *
 *  \param array_type  hash array type,         text (no alias)
 *  \param arr         pointer to a hasharray
 *  \param e           entry value,             by name (lvalue)
 *  \param ...         iteration statement,     code block
 */
#define HTL_HASHARRAY_EACH_DEREF(array_type, arr, e, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), &(e))) { __VA_ARGS__ } \
    } \
} while(0)

/** \def HTL_HASHARRAY_REMOVE_DEREF(array_type, arr, e, cond, ...)
 *
 *  Iterate through the hash array and remove entries.
 *  Receive value copy of the elements.
 *
 *  \param array_type  hash array type,         text (no alias)
 *  \param arr         pointer to a hasharray
 *  \param e           entry value,             by name (lvalue)
 *  \param cond        condition,               by name (re-evaluated)
 *  \param ...         cleanup statement,       code block
 *
 *  \attention
 *      Do not jump out of \a ... after invalidation.
 *      Use \a cond to skip elements instead!
 *      Otherwise the entry wouldn't be removed correctly.
 *
 *  \todo there should be a way to break out of the loop directly
 */
#define HTL_HASHARRAY_REMOVE_DEREF(array_type, arr, e, cond, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), &(e)) && (cond)) \
        { \
            { __VA_ARGS__ } \
            array_type##_RemoveEntry((arr), &(arr)->table[HTL_HASHARRAY_i]); \
            --HTL_HASHARRAY_i; /* repeat */ \
        } \
    } \
} while(0)
