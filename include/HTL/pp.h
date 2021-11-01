#pragma once

#define HTL_CAT_U_0(a, b) a ##_## b
#define HTL_CAT_U(a, b) HTL_CAT_U_0(a, b)

#define HTL_P(name) HTL_CAT_U(HTL_PARAM, name)

#define HTL_MEMBER(name) HTL_CAT_U(HTL_P(NAME), name)

#define HTL_STR_0(txt) #txt
#define HTL_STR(txt) HTL_STR_0(txt)

#ifndef HTL_DEF
#define HTL_DEF
#endif