#pragma once

#include "LAB_debug.h"

// disjunctive addition, UB if at any position both bits are set
//#define LAB_XADD(a, b) LAB_CORRECT_IF((a&b) == 0, (a)+(b))
//#define LAB_XADD(a, b) LAB_CORRECT_IF(((a)|(b)) == (a)+(b), (a)+(b))

//#define LAB_XADD(a, b) LAB_CORRECT_IF(!((a)&(b)),(a)+(b))
#define LAB_XADD(a, b) ((a)+(b))
#define LAB_XADD3(a, b, c) LAB_XADD(LAB_XADD(a, b), c)
#define LAB_XADD4(a, b, c, d) LAB_XADD(LAB_XADD(a, b), LAB_XADD(c, d))

// constant versions
#define LAB_CXADD(a, b) ((a)+(b))
#define LAB_CXADD3(a, b, c) LAB_CXADD(LAB_CXADD(a, b), c)
#define LAB_CXADD4(a, b, c, d) LAB_CXADD(LAB_CXADD(a, b), LAB_CXADD(c, d))

// conjunctive subtraction, UB if at any position a bit in a=0 and b=1
#define LAB_XSUB(a, b) LAB_CORRECT_IF(((a)&~(b)) == (a)-(b), (a)-(b))
#define LAB_XSUB3(a, b, c) LAB_XSUB(LAB_XSUB(a, b), c)
#define LAB_XSUB4(a, b, c, d) LAB_XSUB(LAB_XSUB(a, b), LAB_XSUB(c, d))

#define LAB_XADD_EQ(a, b) ((a) = LAB_XADD(a, b))
#define LAB_XSUB_EQ(a, b) ((a) = LAB_XSUB(a, b))
