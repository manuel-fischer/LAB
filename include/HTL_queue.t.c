#ifdef HTL_PARAM
//#include "HTL_queue.t.h"

HTL_DEF int HTL_MEMBER(Construct)(HTL_P(NAME)* q)
{
    q->first = q->count = 0;
    return 1;
}

HTL_DEF void HTL_MEMBER(Destruct)(HTL_P(NAME)* q)
{
    // NOTHING
}

HTL_DEF int HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q)
{
    return q->count == 0;
}

HTL_DEF int HTL_MEMBER(IsFull)(HTL_P(NAME)* q)
{
    return q->count == HTL_P(CAPACITY);
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(PushBack)(HTL_P(NAME)* q)
{
    if(q->count == HTL_P(CAPACITY)) return NULL;
    q->count++;
    return HTL_MEMBER(Back)(q);
}

HTL_DEF void HTL_MEMBER(PopFront)(HTL_P(NAME)* q)
{
    q->first = (q->first + 1) % HTL_P(CAPACITY);
    q->count--;
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Front)(HTL_P(NAME)* q)
{
    return &q->queue[q->first];
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Back)(HTL_P(NAME)* q)
{
    int index = (q->first+q->count) % (size_t)(HTL_P(CAPACITY));
    return &q->queue[index];
}


HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Find)(HTL_P(NAME)* q, int (*comp1)(void* ctx, HTL_P(TYPE)* content), void* ctx)
{
    for(int i = 0; i < q->count; ++i)
    {
        HTL_P(TYPE)* elem = &q->queue[i%HTL_P(CAPACITY)];
        if((*comp1)(elem, ctx) == 0) return elem;
    }
    return NULL;
}
#endif // HTL_PARAM
