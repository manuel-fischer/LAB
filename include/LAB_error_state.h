#pragma once

#include "LAB_stdinc.h"
#include "LAB_memory.h"
#include "LAB_error.h"
#include <errno.h>

// export LAB_Err
// export LAB_FAILED
// export LAB_OK
// export LAB_RAISE*

//#ifdef __GNUC__
//#define LAB_ERR_ATTR __attribute__((warn_unused_result))
//#else
#define LAB_ERR_ATTR
//#endif

typedef struct LAB_ERR_ATTR LAB_Err { bool failed; } LAB_Err;

#define LAB_FAILED(error) LAB_UNLIKELY((error).failed)
#define LAB_OK ((LAB_Err) { false })
#define LAB_ISOK(error) LAB_LIKELY(!(error).failed)

#define LAB_RAISE_CURRENT() ((LAB_Err) { true })

#define LAB_RAISE_FMT(message_fmt, ...) ( \
    LAB_SetError(message_fmt, __VA_ARGS__), \
    LAB_AddErrorContext(__FILE__, __LINE__, NULL), \
    LAB_RAISE_CURRENT() \
)
#define LAB_RAISE(message) LAB_RAISE_FMT("%s", message)

#define LAB_RAISE_C() ( \
    LAB_SetCError(), \
    LAB_AddErrorContext(__FILE__, __LINE__, NULL), \
    LAB_RAISE_CURRENT() \
)

#define LAB_RAISE_GL() ( \
    LAB_SetCurrentGLError(), \
    LAB_AddErrorContext(__FILE__, __LINE__, NULL), \
    LAB_RAISE_CURRENT() \
)

#define LAB_RAISE_SDL() ( \
    LAB_SetSDLError(), \
    LAB_AddErrorContext(__FILE__, __LINE__, NULL), \
    LAB_RAISE_CURRENT() \
)


#define LAB_RAISE_CTX(err) ( \
    LAB_AddErrorContext(__FILE__, __LINE__, NULL), \
    LAB_EnshureFailed(err) \
)

#define LAB_RAISE_CTX_MSG(err, msg) ( \
    LAB_AddErrorContext(__FILE__, __LINE__, msg), \
    LAB_EnshureFailed(err) \
)

#define LAB_RAISE_CTX_FMT(err, fmt, ...) ( \
    LAB_AddErrorContextFmt(__FILE__, __LINE__, fmt, __VA_ARGS__), \
    LAB_EnshureFailed(err) \
)

LAB_INLINE
LAB_Err LAB_EnshureFailed(LAB_Err err)
{
    LAB_ASSERT(LAB_FAILED(err));
    return err;
}


#define LAB_ASSERT_OR_RAISE(cond) ( \
    LAB_LIKELY(cond) ? LAB_OK : LAB_RAISE_FMT("Condition not satisfied: %s", #cond) \
)

#define LAB_TRY(err) do { \
    LAB_Err LAB_TRY_RETURN_err = (err); \
    if(LAB_FAILED(LAB_TRY_RETURN_err)) return LAB_RAISE_CTX(LAB_TRY_RETURN_err); \
} while(0)
