#include "LAB_string.h"

#include <stdio.h> // vsnprintf

bool LAB_StringBuilder_Create(LAB_StringBuilder* s)
{
    *s = (LAB_StringBuilder) {.data = NULL, .size = 0, .success = true};
    return s->success;
}

void LAB_StringBuilder_Destroy(LAB_StringBuilder* s)
{
    if(!s->success) return;
    LAB_Free(s->data);
}

bool LAB_StringBuilder_Cancel(LAB_StringBuilder* s)
{
    if(!s->success) return false;
    LAB_Free(s->data);
    s->success = false;
    return false;
}

bool LAB_StringBuilder_Cat(LAB_StringBuilder* s, const char* data, size_t count)
{
    if(!s->success) return false;
    if(count == 0) return true;

    // Allocate one additional for NULL-terminator
    char* buffer = LAB_ARRAY_APPEND_SOME_PAD((char, s->data, s->size), count, 1);
    if(!buffer)
        return LAB_StringBuilder_Cancel(s);

    LAB_ASSERT(buffer == s->data + s->size - count);

    memcpy(buffer, data, count);
    s->data[s->size] = '\0';

    return true;
}

LAB_STATIC
bool LAB_StringBuilder_VHPrintF(LAB_StringBuilder* s, const char* format, va_list ap, size_t size_hint)
{
    if(!s->success) return false;

    LAB_MIN_EQ(size_hint, 16);

    size_t insert_pos = s->size;

    char* buffer_start;
    char* buffer_end = s->data + LAB_PaddedCapacity(s->size, 1);

    if(!LAB_ARRAY_APPEND_SOME_PAD((char, s->data, s->size), size_hint, 1))
        return LAB_StringBuilder_Cancel(s);

    buffer_start = s->data + insert_pos;
    buffer_end = s->data + LAB_PaddedCapacity(s->size, 1);


    va_list ap0;
    va_copy(ap0, ap);
    int c = vsnprintf(buffer_start, buffer_end-buffer_start, format, ap0);
    va_end(ap0);

    if(c < 0)
        return LAB_StringBuilder_Cancel(s);

    if(c+1 > buffer_end-buffer_start)
    {
        size_t rest = c - (buffer_end-1-buffer_start);

        if(!LAB_ARRAY_APPEND_SOME_PAD((char, s->data, s->size), rest, 1))
            return LAB_StringBuilder_Cancel(s);

        buffer_start = s->data + insert_pos;
        buffer_end = s->data + LAB_PaddedCapacity(s->size, 1);

        int c2 = vsnprintf(buffer_start, buffer_end-buffer_start, format, ap);
        if(c < 0 || c2 != c)
            return LAB_StringBuilder_Cancel(s);

        c = c2;
    }

    s->size = insert_pos + c;

    return true;
}

bool LAB_StringBuilder_VPrintF(LAB_StringBuilder* s, const char* format, va_list ap)
{
    size_t hint = strnlen(format, 1024) + 1;
    return LAB_StringBuilder_VHPrintF(s, format, ap, hint);
}

bool LAB_StringBuilder_PrintF(LAB_StringBuilder* s, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    bool success = LAB_StringBuilder_VPrintF(s, format, ap);
    va_end(ap);
    return success;
}