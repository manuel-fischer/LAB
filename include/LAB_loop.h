#pragma once

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
