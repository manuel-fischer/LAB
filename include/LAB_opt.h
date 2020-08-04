#pragma once

#define LAB_NOINLINE __attribute__((noinline))
#define LAB_HOT      __attribute__((hot))

#define LAB_ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)

#define LAB_ALIGNED

#define LAB_UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#define LAB_LIKELY(cond) __builtin_expect(!!(cond), 1)
