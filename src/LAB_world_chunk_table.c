#include "LAB_world_chunk_table.h"

#define HTL_PARAM LAB_CHUNK_TBL
#include "HTL/hasharray.t.c"
#undef HTL_PARAM

#define HTL_PARAM LAB_CHUNK_BUF_QUEUE
#include "HTL/queue.t.c"
#undef HTL_PARAM