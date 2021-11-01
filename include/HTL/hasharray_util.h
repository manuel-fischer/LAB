#pragma once

#define HTL_HASHARRAY_EACH(array_type, arr, e, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=&(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), (e))) { __VA_ARGS__ } \
    } \
} while(0)

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



#define HTL_HASHARRAY_EACH_DEREF(array_type, arr, e, ...) do { \
    for(size_t HTL_HASHARRAY_i = 0; HTL_HASHARRAY_i < (arr)->capacity; ++HTL_HASHARRAY_i) \
    { \
        (e)=(arr)->table[HTL_HASHARRAY_i]; \
        if(array_type##_IsEntry((arr), &(e))) { __VA_ARGS__ } \
    } \
} while(0)

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
