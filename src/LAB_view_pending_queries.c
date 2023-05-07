#include "LAB_view_pending_queries.h"

#include "LAB_gl.h"


#define HTL_PARAM LAB_QUERY_QUEUE
#include "HTL/dyn_queue.t.c"
#undef HTL_PARAM



bool LAB_View_PendingQueries_Create(LAB_View_PendingQueries* q)
{
    return LAB_QueryQueue_Create(&q->queue);
}

void LAB_View_PendingQueries_Destroy(LAB_View_PendingQueries* q)
{
    while(!LAB_QueryQueue_IsEmpty(&q->queue))
    {
        LAB_View_PendingQueryEntry* e = LAB_QueryQueue_Front(&q->queue);
        LAB_GL_OBJ_FREE(glDeleteQueries, &e->query);
        LAB_QueryQueue_PopFront(&q->queue);
    }

    LAB_QueryQueue_Destroy(&q->queue);
}


bool LAB_View_PendingQueries_IsEmpty(LAB_View_PendingQueries* q)
{
    return LAB_QueryQueue_IsEmpty(&q->queue);
}

bool LAB_View_PendingQueries_HasAvailable(LAB_View_PendingQueries* q)
{
    if(LAB_View_PendingQueries_IsEmpty(q)) return false;

    LAB_View_PendingQueryEntry* e = LAB_QueryQueue_Front(&q->queue);
    unsigned query_state;
    glGetQueryObjectuiv(e->query.id, GL_QUERY_RESULT_AVAILABLE, &query_state);
    return query_state == GL_TRUE;
}

LAB_GL_Query LAB_View_PendingQueries_Push(LAB_View_PendingQueries* q, LAB_GL_Enum target, size_t param)
{
    LAB_View_PendingQueryEntry* e = LAB_QueryQueue_PushBack(&q->queue);
    if(!e) return (LAB_GL_Query) {0};

    e->target = target;
    e->param = param;
    LAB_GL_OBJ_ALLOC_TARGET(glCreateQueries, target, &e->query);

    return e->query;
}

LAB_View_QueryResult LAB_View_PendingQueries_Pop(LAB_View_PendingQueries* q)
{
    LAB_View_QueryResult result;

    LAB_View_PendingQueryEntry* e = LAB_QueryQueue_Front(&q->queue);

    unsigned query_state;
    glGetQueryObjectuiv(e->query.id, GL_QUERY_RESULT_AVAILABLE, &query_state);
    LAB_ASSERT(query_state == GL_TRUE);

    glGetQueryObjectuiv(e->query.id, GL_QUERY_RESULT, &result.result);
    result.target = e->target;
    result.param = e->param;

    LAB_GL_OBJ_FREE(glDeleteQueries, &e->query);
    LAB_QueryQueue_PopFront(&q->queue);

    return result;
}