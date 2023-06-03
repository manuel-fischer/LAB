#include "LAB_util.h"

size_t LAB_StrHash(const char* str)
{
    size_t hash = 0xabcdef;

    for(;*str; ++str)
        hash = ((hash << 5u) + hash) + (size_t)*str;

    return hash;
}
