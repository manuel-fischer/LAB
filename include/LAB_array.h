#pragma once

#include "LAB_bits.h"

#define LAB_ARRAY_TYPE_(type, data, size)  type
#define LAB_ARRAY_DATA_(type, data, size) (data)
#define LAB_ARRAY_SIZE_(type, data, size) (size)

#define LAB_ARRAY_TYPE(array) LAB_ARRAY_TYPE_ array
#define LAB_ARRAY_DATA(array) LAB_ARRAY_DATA_ array
#define LAB_ARRAY_SIZE(array) LAB_ARRAY_SIZE_ array

/*#define LAB_ARRAY(type) \
    struct { size_t size; type* data; } */

#define LAB_ARRAY_CREATE(array) ( \
    LAB_ARRAY_DATA(array) = NULL, \
    LAB_ARRAY_SIZE(array) = 0u, \
    (void)0 \
)

#define LAB_ARRAY_DESTROY(array) \
    LAB_Free(LAB_ARRAY_DATA(array))

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
#define LAB_ARRAY_APPEND(array, count, sub_array_result) do \
{ \
    size_t LAB_ARRAY_APPEND_count = (count); \
    size_t LAB_ARRAY_APPEND_old_capacity = LAB_Capacity(LAB_ARRAY_SIZE(array)); \
    size_t LAB_ARRAY_APPEND_new_size = LAB_ARRAY_SIZE(array) + LAB_ARRAY_APPEND_count; \
    size_t LAB_ARRAY_APPEND_new_capacity = LAB_Capacity(LAB_ARRAY_APPEND_new_size); \
    if(LAB_ARRAY_APPEND_new_capacity > LAB_ARRAY_APPEND_old_capacity) \
    { \
        (sub_array_result) = LAB_ReallocN(LAB_ARRAY_DATA(array), LAB_ARRAY_APPEND_new_capacity, sizeof(LAB_ARRAY_DATA(array)[0])); \
        if(sub_array_result) \
        { \
            LAB_ARRAY_DATA(array) = (sub_array_result); \
            (sub_array_result) += LAB_ARRAY_SIZE(array); \
            LAB_ARRAY_SIZE(array) = LAB_ARRAY_APPEND_new_size; \
        } \
    } \
    else \
    { \
        (sub_array_result) = LAB_ARRAY_DATA(array) + LAB_ARRAY_SIZE(array); \
        LAB_ARRAY_SIZE(array) = LAB_ARRAY_APPEND_new_size; \
    } \
} while(0)



