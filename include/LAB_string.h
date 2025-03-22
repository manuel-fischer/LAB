#pragma once

#include "LAB_stdinc.h"
#include "LAB_array.h"
#include "LAB_attr.h"
#include <stdarg.h>
#include "LAB_error_state.h"

// export LAB_StringBuilder*
// export LAB_StringView*

typedef struct LAB_StringBuilder
{
    char* data; // padded by 1 for '\0'
    size_t size;
    LAB_Err err;
} LAB_StringBuilder;

typedef struct LAB_StringView
{
    const char* data;
    size_t size;
} LAB_StringView;

LAB_Err LAB_StringBuilder_Create(LAB_StringBuilder* s);
void LAB_StringBuilder_Destroy(LAB_StringBuilder* s);
LAB_Err LAB_StringBuilder_Cancel(LAB_StringBuilder* s, LAB_Err err);
LAB_Err LAB_StringBuilder_Cat(LAB_StringBuilder* s, const char* data, size_t count);
LAB_Err LAB_StringBuilder_VPrintF(LAB_StringBuilder* s, const char* format, va_list ap);
LAB_Err LAB_StringBuilder_PrintF(LAB_StringBuilder* s, const char* format, ...);

LAB_INLINE
LAB_Err LAB_StringBuilder_CatS(LAB_StringBuilder* s, const char* data)
{
    return LAB_StringBuilder_Cat(s, data, strlen(data));
}

LAB_INLINE
const char* LAB_StringBuilder_AsCStr(const LAB_StringBuilder* s)
{
    LAB_ASSERT(!LAB_FAILED(s->err));
    if(s->data == NULL) return "";
    return s->data;
}

LAB_INLINE
LAB_StringView LAB_StringBuilder_AsStringView(const LAB_StringBuilder* s)
{
    LAB_ASSERT(!LAB_FAILED(s->err));
    LAB_ASSERT_FMT(strlen(s->data) == s->size, "strlen(s->data) = %zi, s->size = %zi", strlen(s->data), s->size);
    return (LAB_StringView) { .data = s->data, .size = s->size };
}