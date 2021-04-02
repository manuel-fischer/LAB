#pragma once

#define LAB_ARRAY(type) \
    struct { size_t size; type* data; }

#define LAB_ARRAY_CREATE(array) \
    memset(array, 0, sizeof(array))

#define LAB_ARRAY_DESTROY(array) \
    LAB_Free((array).data)
