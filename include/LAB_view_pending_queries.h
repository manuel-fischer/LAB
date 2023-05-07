#pragma once

#include "LAB_stdinc.h"
#include "LAB_gl_types.h"
#include "LAB_htl_config.h"

typedef struct LAB_View_PendingQueryEntry
{
    LAB_GL_Query query;
    LAB_GL_Enum target;
    size_t param;
} LAB_View_PendingQueryEntry;

typedef struct LAB_View_QueryResult
{
    LAB_GL_Enum target;
    size_t param;
    unsigned result;
} LAB_View_QueryResult;

#define LAB_QUERY_QUEUE_NAME     LAB_QueryQueue
#define LAB_QUERY_QUEUE_TYPE     LAB_View_PendingQueryEntry

#define HTL_PARAM LAB_QUERY_QUEUE
#include "HTL/dyn_queue.t.h"
#undef HTL_PARAM

typedef struct LAB_View_PendingQueries
{
    LAB_QueryQueue queue;
} LAB_View_PendingQueries;

bool LAB_View_PendingQueries_Create(LAB_View_PendingQueries* q);
void LAB_View_PendingQueries_Destroy(LAB_View_PendingQueries* q);
bool LAB_View_PendingQueries_IsEmpty(LAB_View_PendingQueries* q);
bool LAB_View_PendingQueries_HasAvailable(LAB_View_PendingQueries* q);
LAB_GL_Query LAB_View_PendingQueries_Push(LAB_View_PendingQueries* q, LAB_GL_Enum target, size_t param);
LAB_View_QueryResult LAB_View_PendingQueries_Pop(LAB_View_PendingQueries* q);