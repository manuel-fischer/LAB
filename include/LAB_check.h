/** \file LAB_check.h
 *
 *  Various static checks
 */

#pragma once

/**
 *  Assume and check a given expression at compile time, otherwise quit compilation
 */
#define LAB_STATIC_ASSUME(expr, msg) _Static_assert(expr, msg)

/**
 *  Check if a type has a size that is a power of two
 */
#define LAB_CHECK_STRUCT_SIZE(type) \
    LAB_STATIC_ASSUME((sizeof(type)&(sizeof(type)-1)) == 0, \
                      "Struct size should be power of 2")
