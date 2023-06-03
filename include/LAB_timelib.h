#pragma once

#include <stdint.h> // uint64_t

// export LAB_NANOS_MAX
// export LAB_Nanos
// export LAB_NanoSeconds


#define LAB_NANOS_MAX UINT64_MAX
typedef uint64_t LAB_Nanos;

LAB_Nanos LAB_NanoSeconds(void);