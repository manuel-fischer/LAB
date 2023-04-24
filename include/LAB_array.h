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


typedef struct LAB_Array_Resize_PointerMem
{
    void* array;
    size_t pointer_size;
} LAB_Array_Resize_Locals;

typedef struct LAB_Array_Resize_Result
{
    void* new_data;
    bool success;
} LAB_Array_Resize_Result;


// by only allowing new_size != 0, the function only returns NULL exactly on an error
// padding should always be the same for the same allocation
LAB_INLINE LAB_ALWAYS_INLINE
void* LAB_Array_Resize_Realloc(void* data, size_t* old_size, size_t new_size, size_t element_size, size_t padding)
{
    LAB_ASSERT(new_size != 0);
    size_t old_capacity, new_capacity;

    if(LAB_IS_CONSTANT(padding) && padding == 0)
    {
        old_capacity = LAB_Capacity(*old_size);
        new_capacity = LAB_Capacity(new_size);
    }
    else
    {
        old_capacity = LAB_SELECT0(data, LAB_Capacity(*old_size+padding));
        new_capacity = LAB_Capacity(new_size+padding);
    }

    LAB_ASSERT(!data ? old_capacity == 0 : true);

    void* new_data = data;

    if(new_capacity != old_capacity)
        new_data = LAB_ReallocN(data, new_capacity, element_size);
    else
        LAB_ASSERT(new_data != NULL);

    if(new_data != NULL)
        *old_size = new_size;

    return new_data;
}

// Assigning the pointer, that avoids undefined assignment of eg int** through a void**
LAB_INLINE LAB_ALWAYS_INLINE
bool LAB_Array_Resize_Assign_Ptr(void*LAB_RESTRICT pdata, size_t pdata_size, void*LAB_RESTRICT pdata_new)
{
    bool success = memcmp(pdata_new, LAB_PTR_OFFSET(pdata_new, 1, pdata_size), pdata_size) != 0;

    if(success)
        memcpy(pdata, pdata_new, pdata_size);

    return success;
}


#define LAB_ARRAY_RESIZE_SOME(array, new_size) \
    LAB_Array_Resize_Assign_Ptr(&LAB_ARRAY_DATA(array), sizeof(LAB_ARRAY_DATA(array)), \
        ((LAB_ARRAY_TYPE(array)*[2]) { (LAB_ARRAY_TYPE(array)*)LAB_Array_Resize_Realloc( \
            LAB_ARRAY_DATA(array), &LAB_ARRAY_SIZE(array), new_size, sizeof(LAB_ARRAY_DATA(array)[0]), 0 \
        ), NULL }) \
    )

#define LAB_ARRAY_RESIZE2(array, new_size) \
    ((new_size) == 0) \
      ? (LAB_ARRAY_SIZE(array) = 0, LAB_ARRAY_DATA(array) = NULL, true) \
      : LAB_ARRAY_RESIZE2_SOME(array, new_size)

#define LAB_ARRAY_APPEND_SOME(array, add_size) \
    (LAB_ARRAY_RESIZE_SOME(array, LAB_ARRAY_SIZE(array) + (add_size)) ? LAB_ARRAY_DATA(array) + LAB_ARRAY_SIZE(array) - (add_size) : (LAB_ARRAY_TYPE(array)*)NULL)


// padded allocation like for additional sentinel values.
// The allocated buffer size is always a power of 2, thus the capacity is LAB_Capacity(size+padding).
// padding should always be the same for the same allocation, should not be mixed with other allocation
#define LAB_ARRAY_RESIZE_SOME_PAD(array, new_size, padding) \
    LAB_Array_Resize_Assign_Ptr(&LAB_ARRAY_DATA(array), sizeof(LAB_ARRAY_DATA(array)), \
        ((LAB_ARRAY_TYPE(array)*[2]) { (LAB_ARRAY_TYPE(array)*)LAB_Array_Resize_Realloc( \
            LAB_ARRAY_DATA(array), &LAB_ARRAY_SIZE(array), new_size, sizeof(LAB_ARRAY_DATA(array)[0]), padding \
        ), NULL }) \
    )

#define LAB_ARRAY_APPEND_SOME_PAD(array, add_size, padding) \
    (LAB_ARRAY_RESIZE_SOME_PAD(array, LAB_ARRAY_SIZE(array) + (add_size), padding) ? LAB_ARRAY_DATA(array) + LAB_ARRAY_SIZE(array) - (add_size) : (LAB_ARRAY_TYPE(array)*)NULL)

#define LAB_PaddedCapacity(size, padding) LAB_Capacity(size+padding)