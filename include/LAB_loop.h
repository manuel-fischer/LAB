#pragma once

// export LAB_LOOP_*
// export LAB_FOREACH
// export LAB_FOREACH_INDEX
// export LAB_CFOREACH
// export LAB_CFOREACH_INDEX

// max exclusive
#define LAB_LOOP_FORWARD(itr_type, itr, min, max, ...) do \
{ \
    for(itr_type itr = (min); itr < (max); ++itr) \
    { __VA_ARGS__ } \
} \
while(0)

#define LAB_LOOP_BACKWARD(itr_type, itr, min, max, ...) do \
{ \
    if((min) < (max)) \
    { \
        itr_type itr = (max); \
        do \
        { \
            --itr; \
            { __VA_ARGS__ } \
        } \
        while(itr != (min)); \
    } \
} \
while(0)



#define LAB_LOOP_BACKWARD_IF(cond) \
    int LAB_LOOP_BACKWARD_IF_bw = (cond); \
    LAB_LOOP_BACKWARD_IF_2

#define LAB_LOOP_BACKWARD_IF_2(itr_type, itr, min, max, ...) do \
{ \
    if((min) < (max)) \
    { \
        int LAB_LOOP_BACKWARD_IF_step = LAB_LOOP_BACKWARD_IF_bw ? -1 : 1; \
        itr_type itr = LAB_LOOP_BACKWARD_IF_bw ? (max)-1 : (min); \
        itr_type LAB_LOOP_BACKWARD_IF_end = (LAB_LOOP_BACKWARD_IF_bw ? (min) : (max)-1); \
        while(1) \
        { \
            { __VA_ARGS__ } \
            if(itr == LAB_LOOP_BACKWARD_IF_end) \
                break; \
            itr += LAB_LOOP_BACKWARD_IF_step; \
        } \
    } \
} \
while(0)




#define LAB_FOREACH(element_type, iterator, prefix) \
    for(element_type* LAB_FOREACH_BEGIN = (prefix), \
                    * LAB_FOREACH_END = LAB_FOREACH_BEGIN + (prefix ## _count), \
                    * iterator = LAB_FOREACH_BEGIN; \
        iterator != LAB_FOREACH_END; ++iterator)

#define LAB_FOREACH_INDEX(element_type, index_name, iterator, prefix) \
    LAB_FOREACH(element_type, iterator, prefix) \
    for(size_t LAB_FOREACH_TMP=1, index_name = iterator-LAB_FOREACH_BEGIN;LAB_FOREACH_TMP;LAB_FOREACH_TMP=0)

#define LAB_CFOREACH(element_type, iterator, prefix) \
    LAB_FOREACH(element_type const, iterator, prefix)

#define LAB_CFOREACH_INDEX(element_type, index_name, iterator, prefix) \
    LAB_FOREACH_INDEX(element_type const, index_name, iterator, prefix)


#define LAB_FOREACH_ARRAY(iterator, array) \
    for(LAB_ARRAY_TYPE(array)* LAB_FOREACH_BEGIN = LAB_ARRAY_DATA(array), \
                             * LAB_FOREACH_END = LAB_FOREACH_BEGIN + LAB_ARRAY_SIZE(array), \
                             * iterator = LAB_FOREACH_BEGIN; \
        iterator != LAB_FOREACH_END; ++iterator)

