#pragma once

const int LAB_offset[6][3];

const char* LAB_Filename(const char* path);


#define LAB_MAX(a, b) ((a)>(b) ? (a) : (b))
#define LAB_MIN(a, b) ((a)<(b) ? (a) : (b))

#define LAB_MAX3(a, b, c) (  (a)>(b) ? ( (a)>(c) ? (a) : (c) ) : ( (b)>(c) ? (b) : (c) )  )
