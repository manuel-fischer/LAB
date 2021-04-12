#pragma once

#include "LAB_bits.h"

#define LAB_ARRAY(type) \
    struct { size_t size; type* data; }

#define LAB_ARRAY_CREATE(array) \
    memset(&(array), 0, sizeof(array))

#define LAB_ARRAY_DESTROY(array) \
    LAB_Free((array).data)

#define LAB_ARRAY_APPEND(array, count, sub_array_result) do \
{ \
    size_t LAB_ARRAY_APPEND_count = (count); \
    size_t LAB_ARRAY_APPEND_capacity = (array).size == 0 ? 0 : LAB_Capacity((array).size); \
    size_t LAB_ARRAY_APPEND_new_size = (array).size + LAB_ARRAY_APPEND_count; \
    if((array).size > LAB_ARRAY_APPEND_new_size) \
    { \
        /* TODO overflow error */ \
        (sub_array_result) = NULL; \
        break; \
    } \
    int overflow = 0; \
    while(LAB_ARRAY_APPEND_new_size > LAB_ARRAY_APPEND_capacity) \
    { \
        size_t LAB_ARRAY_APPEND_prev_cap = LAB_ARRAY_APPEND_capacity; \
        LAB_ARRAY_APPEND_capacity <<= 1; \
        if(LAB_ARRAY_APPEND_prev_cap > (LAB_ARRAY_APPEND_capacity>>1)) \
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
    (sub_array_result) = LAB_ReallocN((array).data, LAB_ARRAY_APPEND_capacity, sizeof(*((array).data))); \
    if((sub_array_result)) \
    { \
        (array).size = LAB_ARRAY_APPEND_new_size; \
        (array).data = (sub_array_result); \
    } \
} while(0)
