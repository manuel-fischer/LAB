#pragma once

#include "LAB_bits.h"
#include "LAB_memory.h"

#define LAB_DEF_ARRAY(type, name_prefix) \
    type* name_prefix; \
    size_t name_prefix ## _count

#define LAB_REF_ARRAY(type, object, name_prefix) \
    (type, (object)->name_prefix, (object)->name_prefix ## _count)



#define LAB_DEF_ARRAY_UNNAMED(type) \
    type* data; \
    size_t count

#define LAB_REF_ARRAY_UNNAMED(type, object) \
    (type, (object)->data, (object)->count)


#define LAB_ARRAY_TYPE_(type, data, size)  type
#define LAB_ARRAY_DATA_(type, data, size) (data)
#define LAB_ARRAY_SIZE_(type, data, size) (size)

#define LAB_ARRAY_TYPE(array) LAB_ARRAY_TYPE_ array
#define LAB_ARRAY_DATA(array) LAB_ARRAY_DATA_ array
#define LAB_ARRAY_SIZE(array) LAB_ARRAY_SIZE_ array

/*#define LAB_ARRAY(type) \
    struct { size_t size; type* data; } */

#define LAB_ARRAY_CREATE_EMPTY(array) ( \
    LAB_ARRAY_DATA(array) = NULL, \
    !(LAB_ARRAY_SIZE(array) = 0u) /*!(..) -> return value without warning if ignored*/ \
)

#define LAB_ARRAY_CREATE(array, size) ( \
    LAB_ARRAY_SIZE(array) = (size), \
    LAB_ARRAY_DATA(array) = LAB_MallocN(LAB_ARRAY_SIZE(array), sizeof(LAB_ARRAY_TYPE(array))) \
)

#define LAB_ARRAY_DESTROY(array) \
    LAB_Free(LAB_ARRAY_DATA(array))


#define LAB_ARRAY_DESTROY_WITH(array, element_destruct) do { \
    for(size_t LAB_ARRAY_i = LAB_ARRAY_SIZE(array); LAB_ARRAY_i != 0; --LAB_ARRAY_i) { \
        element_destruct(&(LAB_ARRAY_DATA(array))[LAB_ARRAY_i - 1]); \
    } \
    LAB_Free(LAB_ARRAY_DATA(array)); \
} while(0)

#if 0
#define LAB_ARRAY_APPEND(array, count, sub_array_result) do \
{ \
    size_t LAB_ARRAY_APPEND_count = (count); \
    size_t LAB_ARRAY_APPEND_old_size = (array).size; \
    size_t LAB_ARRAY_APPEND_old_capacity = LAB_Capacity(LAB_ARRAY_APPEND_old_size); \
    size_t LAB_ARRAY_APPEND_new_capacity = (array).size == 0 ? 1 : LAB_ARRAY_APPEND_old_capacity; \
    size_t LAB_ARRAY_APPEND_new_size = (array).size + LAB_ARRAY_APPEND_count; \
    if((array).size > LAB_ARRAY_APPEND_new_size) \
    { \
        /* TODO overflow error */ \
        (sub_array_result) = NULL; \
        break; \
    } \
    int overflow = 0; \
    while(LAB_ARRAY_APPEND_new_size > LAB_ARRAY_APPEND_new_capacity) \
    { \
        size_t LAB_ARRAY_APPEND_prev_cap = LAB_ARRAY_APPEND_new_capacity; \
        LAB_ARRAY_APPEND_new_capacity <<= 1; \
        if(LAB_ARRAY_APPEND_prev_cap > (LAB_ARRAY_APPEND_new_capacity>>1)) \
        { \
            overflow = 1; \
            break; /*overflow*/ \
        } \
    } \
    if(overflow) \
    { \
        /* TODO overflow error */ \
        (sub_array_result) = NULL; \
        break; \
    } \
    if(LAB_ARRAY_APPEND_new_capacity > LAB_ARRAY_APPEND_old_capacity) \
    { \
        (sub_array_result) = LAB_ReallocN((array).data, LAB_ARRAY_APPEND_new_capacity, sizeof(*((array).data))); \
        if((sub_array_result)) \
        { \
            (array).size = LAB_ARRAY_APPEND_new_size; \
            (array).data = (sub_array_result); \
            (sub_array_result) += LAB_ARRAY_APPEND_old_size; \
        } \
    } \
    else \
    { \
        (sub_array_result) = (array).data + LAB_ARRAY_APPEND_old_size; \
    } \
} while(0)
#endif

// TODO currently no overflow checks
#define LAB_ARRAY_APPEND(array, count, p_sub_array_result) do \
{ \
    size_t LAB_ARRAY_APPEND_count = (count); \
    size_t LAB_ARRAY_APPEND_old_capacity = LAB_Capacity(LAB_ARRAY_SIZE(array)); \
    size_t LAB_ARRAY_APPEND_new_size = LAB_ARRAY_SIZE(array) + LAB_ARRAY_APPEND_count; \
    size_t LAB_ARRAY_APPEND_new_capacity = LAB_Capacity(LAB_ARRAY_APPEND_new_size); \
    if(LAB_ARRAY_APPEND_new_capacity > LAB_ARRAY_APPEND_old_capacity) \
    { \
        (*(p_sub_array_result)) = LAB_ReallocN(LAB_ARRAY_DATA(array), LAB_ARRAY_APPEND_new_capacity, sizeof(LAB_ARRAY_DATA(array)[0])); \
        if(*(p_sub_array_result)) \
        { \
            LAB_ARRAY_DATA(array) = (*(p_sub_array_result)); \
            (*(p_sub_array_result)) += LAB_ARRAY_SIZE(array); \
            LAB_ARRAY_SIZE(array) = LAB_ARRAY_APPEND_new_size; \
        } \
    } \
    else \
    { \
        (*(p_sub_array_result)) = LAB_ARRAY_DATA(array) + LAB_ARRAY_SIZE(array); \
        LAB_ARRAY_SIZE(array) = LAB_ARRAY_APPEND_new_size; \
    } \
} while(0)



#define LAB_ARRAY_RESIZE(p_success, array, new_size) do \
{ \
    size_t LAB_ARRAY_old_capacity = LAB_Capacity(LAB_ARRAY_SIZE(array)); \
    size_t LAB_ARRAY_new_size = (new_size); \
    size_t LAB_ARRAY_new_capacity = LAB_Capacity(LAB_ARRAY_new_size); \
    if(LAB_ARRAY_new_size == 0) \
    { \
        LAB_Free(LAB_ARRAY_DATA(array)); \
        LAB_ARRAY_DATA(array) = NULL; \
        LAB_ARRAY_SIZE(array) = 0; \
        *(p_success) = true; \
    } \
    else if(LAB_ARRAY_new_capacity != LAB_ARRAY_old_capacity) \
    { \
        LAB_ARRAY_TYPE(array)* LAB_ARRAY_new_data = LAB_ReallocN(LAB_ARRAY_DATA(array), LAB_ARRAY_new_capacity, sizeof(LAB_ARRAY_DATA(array)[0])); \
        if(LAB_ARRAY_new_data != NULL) \
        { \
            LAB_ARRAY_DATA(array) = LAB_ARRAY_new_data; \
            LAB_ARRAY_SIZE(array) = LAB_ARRAY_new_size; \
            *(p_success) = true; \
        } \
        else *(p_success) = false; \
    } \
    else \
    { \
        LAB_ARRAY_SIZE(array) = LAB_ARRAY_new_size; \
        *(p_success) = true; \
    } \
} while(0)

