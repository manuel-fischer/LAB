#pragma once

#define LAB_CHECK_STRUCT_SIZE(type) _Static_assert((sizeof(type)&(sizeof(type)-1)) == 0, "Struct size should be power of 2")
