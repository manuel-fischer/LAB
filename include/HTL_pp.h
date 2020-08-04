#pragma once

#define HTL_CAT_U_0(param, name) param ##_## name
#define HTL_CAT_U(param, name) HTL_CAT_U_0(param, name)

#define HTL_P(name) HTL_CAT_U(HTL_PARAM, name)

#define HTL_MEMBER_1(prefix, postfix) prefix ##_## postfix
#define HTL_MEMBER_0(prefix, postfix) HTL_MEMBER_1(prefix, postfix)
#define HTL_MEMBER(name) HTL_MEMBER_0(HTL_P(NAME), name)
