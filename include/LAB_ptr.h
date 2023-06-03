#pragma once

// export LAB_PTR_OFFSET
// export LAB_CPTR_OFFSET
// export LAB_LEN

#define LAB_PTR_OFFSET(ptr, index, size) ((void*)((char*)(ptr) + (index)*(size)))
#define LAB_CPTR_OFFSET(ptr, index, size) ((const void*)((const char*)(ptr) + (index)*(size)))

#define LAB_LEN(array) (sizeof(array) / sizeof(array[0]))