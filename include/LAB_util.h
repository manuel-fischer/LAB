#pragma once
/** \file LAB_util.h
 *
 *  Common utility funcions
 */

#include <stddef.h> // size_t
#include <stdbool.h> // bool
#include "LAB_opt.h"
#include "LAB_attr.h"
#include "LAB_debug.h"
#include "LAB_bits.h"

// export LAB_StrHash
// export LAB_ObjCopy
// export LAB_MakeTrue
// export LAB_ObjClear
// export LAB_ArrayClear


LAB_POINTER_CONST
size_t LAB_StrHash(const char* str);



#define LAB_ObjCopy(dstp, srcp) LAB_CORRECT_IF( \
    sizeof(*(dstp)) == sizeof(*(srcp)), \
    memcpy(dstp, srcp, sizeof(*(dstp))))



/*LAB_PURE*/ LAB_ALWAYS_INLINE LAB_UNUSED LAB_INLINE 
bool LAB_MakeTrue(void) { return true; }


#define LAB_ObjClear(dstp) (memset(dstp, 0, sizeof(*(dstp))), LAB_MakeTrue())
#define LAB_ArrayClear(dstp) (memset(dstp, 0, sizeof(dstp)), LAB_MakeTrue())
