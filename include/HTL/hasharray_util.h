#pragma once

#include "HTL/pp.h"

#define HTL_HASHARRAY_EACH(array_type, arr, e, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=&(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), (e))) { __VA_ARGS__ } \
    } \
} while(0)

#if 0
#define HTL_HASHARRAY_REMOVE(array_type, arr, e, cond, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=&(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), (e)) && (cond)) \
        { \
            { __VA_ARGS__ } \
            array_type##_RemoveEntry((arr), (e)); \
            --HTL_HASHARRAY_i; /* repeat */ \
        } \
    } \
} while(0)
#endif

#define HTL_HASHARRAY_REMOVE(array_type, arr, e, cond, ...) do { \
    size_t HTL_HASHARRAY_i = 0; \
    goto HTL_CAT_U(HTL_HASHARRAY_LOOP, __LINE__); \
    while(1) \
    { /*continue goes here*/ \
        array_type##_RemoveEntry((arr), (e)); \
    HTL_CAT_U(HTL_HASHARRAY_LOOP, __LINE__):; \
        if(HTL_HASHARRAY_i >= (arr)->capacity) \
            goto HTL_CAT_U(HTL_HASHARRAY_LOOPSTOP, __LINE__); \
        (e)=&(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), (e)) && (cond)) \
        { \
            { __VA_ARGS__ } \
            array_type##_RemoveEntry((arr), (e)); \
        } \
        else \
            ++HTL_HASHARRAY_i; \
        goto HTL_CAT_U(HTL_HASHARRAY_LOOP, __LINE__); \
    } /*break goes here*/ \
    array_type##_RemoveEntry((arr), (e)); \
    HTL_CAT_U(HTL_HASHARRAY_LOOPSTOP, __LINE__):; \
} while(0)



#define HTL_HASHARRAY_EACH_DEREF(array_type, arr, e, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), &(e))) { __VA_ARGS__ } \
    } \
} while(0)

#if 0
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
#endif

#define HTL_HASHARRAY_REMOVE_DEREF(array_type, arr, e, cond, ...) do { \
    size_t HTL_HASHARRAY_i = 0; \
    goto HTL_CAT_U(HTL_HASHARRAY_LOOP, __LINE__); \
    while(1) \
    { /*continue goes here*/ \
        array_type##_RemoveEntry((arr), &(arr)->table[HTL_HASHARRAY_i]); \
    HTL_CAT_U(HTL_HASHARRAY_LOOP, __LINE__):; \
        if(HTL_HASHARRAY_i >= (arr)->capacity) \
            goto HTL_CAT_U(HTL_HASHARRAY_LOOPSTOP, __LINE__); \
        (e)=(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), &(arr)->table[HTL_HASHARRAY_i]) && (cond)) \
        { \
            { __VA_ARGS__ } \
            array_type##_RemoveEntry((arr), &(arr)->table[HTL_HASHARRAY_i]); \
        } \
        else \
            ++HTL_HASHARRAY_i; \
        goto HTL_CAT_U(HTL_HASHARRAY_LOOP, __LINE__); \
    } /*break goes here*/ \
    array_type##_RemoveEntry((arr), &(arr)->table[HTL_HASHARRAY_i]); \
    HTL_CAT_U(HTL_HASHARRAY_LOOPSTOP, __LINE__):; \
} while(0)
