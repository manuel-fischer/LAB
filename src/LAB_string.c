#include "LAB_string.h"

#include <stdio.h> // vsnprintf
#include <string.h> // strnlen

#include "LAB_select.h"

LAB_Err LAB_StringBuilder_Create(LAB_StringBuilder* s)
{
    *s = (LAB_StringBuilder) {.data = NULL, .size = 0, .err = LAB_OK};
    return s->err;
}

void LAB_StringBuilder_Destroy(LAB_StringBuilder* s)
{
    if(LAB_FAILED(s->err)) return;
    LAB_Free(s->data);
}

LAB_Err LAB_StringBuilder_Cancel(LAB_StringBuilder* s, LAB_Err err)
{
    LAB_ASSERT(LAB_ISOK(s->err));
    LAB_Free(s->data);
    s->err = err;
    return err;
}

LAB_Err LAB_StringBuilder_Cat(LAB_StringBuilder* s, const char* data, size_t count)
{
    if(LAB_FAILED(s->err)) return s->err;
    if(count == 0) return LAB_OK;

    // Allocate one additional for NULL-terminator
    char* buffer = LAB_ARRAY_APPEND_SOME_PAD((char, s->data, s->size), count, 1);
    if(!buffer)
        return LAB_StringBuilder_Cancel(s, LAB_RAISE_C());

    LAB_ASSERT(buffer == s->data + s->size - count);

    memcpy(buffer, data, count);
    s->data[s->size] = '\0';

    return LAB_OK;
}

LAB_STATIC
LAB_Err LAB_StringBuilder_VHPrintF(LAB_StringBuilder* s, const char* format, va_list ap, size_t size_hint)
{
    if(LAB_FAILED(s->err)) return s->err;

    size_hint = LAB_MIN(size_hint, 16);

    size_t insert_pos = s->size;

    char* buffer_start;
    char* buffer_end = s->data + LAB_PaddedCapacity(s->size, 1);

    if(!LAB_ARRAY_APPEND_SOME_PAD((char, s->data, s->size), size_hint, 1))
        return LAB_StringBuilder_Cancel(s, LAB_RAISE_C());

    buffer_start = s->data + insert_pos;
    buffer_end = s->data + LAB_PaddedCapacity(s->size, 1);


    va_list ap0;
    va_copy(ap0, ap);
    int c = vsnprintf(buffer_start, buffer_end-buffer_start, format, ap0);
    va_end(ap0);

    if(c < 0)
        return LAB_StringBuilder_Cancel(s, LAB_RAISE_C());

    if(c+1 > buffer_end-buffer_start)
    {
        size_t rest = c - (buffer_end-1-buffer_start);

        if(!LAB_ARRAY_APPEND_SOME_PAD((char, s->data, s->size), rest, 1))
            return LAB_StringBuilder_Cancel(s, LAB_RAISE_C());

        buffer_start = s->data + insert_pos;
        buffer_end = s->data + LAB_PaddedCapacity(s->size, 1);

        int c2 = vsnprintf(buffer_start, buffer_end-buffer_start, format, ap);
        if(c < 0 || c2 != c)
            return LAB_StringBuilder_Cancel(s, LAB_RAISE_C());

        c = c2;
    }

    s->size = insert_pos + c;

    return LAB_OK;
}

LAB_Err LAB_StringBuilder_VPrintF(LAB_StringBuilder* s, const char* format, va_list ap)
{
    size_t hint = strlen(format)+1;
    return LAB_StringBuilder_VHPrintF(s, format, ap, hint);
}

LAB_Err LAB_StringBuilder_PrintF(LAB_StringBuilder* s, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    LAB_Err err = LAB_StringBuilder_VPrintF(s, format, ap);
    va_end(ap);
    return err;
}