#pragma once
/** \file HTL_pp.h
 *
 *  Preprocessor macros used for implementing template headers
 */

#ifndef DOXYGEN

#define HTL_CAT_U_0(a, b) a ##_## b
#define HTL_CAT_U(a, b) HTL_CAT_U_0(a, b)

#define HTL_P(name) HTL_CAT_U(HTL_PARAM, name)

#define HTL_MEMBER(name) HTL_CAT_U(HTL_P(NAME), name)

#define HTL_STR_0(txt) #txt
#define HTL_STR(txt) HTL_STR_0(txt)

#ifndef HTL_DEF
#define HTL_DEF
#endif

#else /* DOXYGEN SECTION */

/**
 *  Concat two tokens( \a a and \a b ) with an underscore
 */
#define HTL_CAT_U(a, b)

/**
 *  Get a parameter of the current parameter set.
 *  The parameter set is specified by the current token in HTL_PARAM
 *
 *  @param name  the name of the parameter
 */
#define HTL_P(name)

/**
 *  Generate an identifier in the current namespace.
 *  The current namespace is specified by the current value
 *  of the parameter \a NAME
 *
 *  \param name
 */
#define HTL_MEMBER(name)

/**
 *  This macro can specify function attributes declared in
 *  template headers.
 *
 *  This macro can be defined before including HTL_pp.h
 *  to insert attributes. Usually this macro is empty.
 */
#define HTL_DEF
#endif
