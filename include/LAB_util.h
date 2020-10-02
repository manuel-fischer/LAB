#pragma once

#include <stdint.h> // uint64_t
#include "LAB_opt.h"

    LAB_DEPRECATED("inefficient")
const int LAB_offset[6][3];


/**
 *  {-1, 0, 0},
 *  { 1, 0, 0},
 *  { 0,-1, 0},
 *  { 0, 1, 0},
 *  { 0, 0,-1},
 *  { 0, 0, 1},
 */
#define LAB_OFFSET_X(i) (int)(-!(i)+((i)==1))
#define LAB_OFFSET_Y(i) LAB_OFFSET_X((i)-2)
#define LAB_OFFSET_Z(i) LAB_OFFSET_X((i)-4)

#define LAB_OX(i) LAB_OFFSET_X(i)
#define LAB_OY(i) LAB_OFFSET_Y(i)
#define LAB_OZ(i) LAB_OFFSET_Z(i)


    LAB_DEPRECATED("inefficient")
const int LAB_offsetA[3][3];
/**
 *  { 0, 1, 0 },
 *  { 0, 0, 1 },
 *  { 1, 0, 0 },
 */
#define LAB_AX(i) (int)((i)==2)
#define LAB_AY(i) (int)((i)==0)
#define LAB_AZ(i) (int)((i)==1)


    LAB_DEPRECATED("inefficient")
const int LAB_offsetB[3][3];
/**
 *  { 0, 0, 1 },
 *  { 1, 0, 0 },
 *  { 0, 1, 0 },
 */
#define LAB_BX(i) (int)((i)==1)
#define LAB_BY(i) (int)((i)==2)
#define LAB_BZ(i) (int)((i)==0)



const char* LAB_Filename(const char* path);


#define LAB_MAX(a, b) ((a)>(b) ? (a) : (b))
#define LAB_MIN(a, b) ((a)<(b) ? (a) : (b))

#define LAB_MAX3(a, b, c) (  (a)>(b) ? ( (a)>(c) ? (a) : (c) ) : ( (b)>(c) ? (b) : (c) )  )

uint64_t LAB_NanoSeconds();
