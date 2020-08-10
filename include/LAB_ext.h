#pragma once

#ifdef __GNUC__
#define LAB_CASE_RANGE(a, n) case (a)...(a+n)
#else
#define LAB_CASE_RANGE_1(a)                      case (a)
#define LAB_CASE_RANGE_2(a) LAB_CASE_RANGE_1(a): case (a)+1
#define LAB_CASE_RANGE_3(a) LAB_CASE_RANGE_2(a): case (a)+2
#define LAB_CASE_RANGE_4(a) LAB_CASE_RANGE_3(a): case (a)+3
#define LAB_CASE_RANGE_5(a) LAB_CASE_RANGE_4(a): case (a)+4
#define LAB_CASE_RANGE_6(a) LAB_CASE_RANGE_5(a): case (a)+5
#define LAB_CASE_RANGE_7(a) LAB_CASE_RANGE_6(a): case (a)+6
#define LAB_CASE_RANGE_8(a) LAB_CASE_RANGE_7(a): case (a)+7
#define LAB_CASE_RANGE_9(a) LAB_CASE_RANGE_8(a): case (a)+8
#define LAB_CASE_RANGE(a, n) LAB_CASE_RANGE_##n(a)
#endif // __GNUC__
