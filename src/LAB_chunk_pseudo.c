#include "LAB_chunk_pseudo.h"
#include "LAB_blocks.h"

#define DUP_16(...) __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                    __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                    __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                    __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__

#define DUP_16x16(...) DUP_16(__VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                              __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                              __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                              __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__)

#define DUP_16x16x16(...) DUP_16x16(__VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                                    __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                                    __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
                                    __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__)

#if 0
const LAB_Chunk LAB_empty_chunk_dark = {
    .blocks = { DUP_16x16x16(&LAB_BLOCK_OUTSIDE) },
    .light  = { DUP_16x16x16(LAB_RGB(15, 15, 15)) },
    .dirty = 0, .modified = 0, .light_generated = 0, .pseudo = 1,
    .age = 0
};
const LAB_Chunk LAB_empty_chunk_lit = {
    .blocks = { DUP_16x16x16(&LAB_BLOCK_OUTSIDE) },
    .light  = { DUP_16x16x16(LAB_RGB(255, 255, 255)) },
    .dirty = 0, .modified = 0, .light_generated = 0, .pseudo = 1,
    .age = 0
};
#endif
