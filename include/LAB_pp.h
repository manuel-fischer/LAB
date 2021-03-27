#pragma once

#define LAB_PP_DUP2(a) a, a

#define LAB_PP_IDENTITY(...) __VA_ARGS__
#define LAB_PP_UNPACK(tup) LAB_PP_IDENTITY tup
