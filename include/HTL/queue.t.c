#ifdef HTL_PARAM

HTL_DEF bool HTL_MEMBER(Create)(HTL_P(NAME)* q, size_t capacity)
{
    // Overflow
    if((capacity*sizeof(HTL_P(TYPE)))/sizeof(HTL_P(TYPE)) < capacity)
        return 0;


    q->queue = HTL_MALLOC(capacity*sizeof(HTL_P(TYPE)));
    q->first = q->count = 0;
    q->capacity = capacity;
    return q->queue != NULL;
}

HTL_DEF void HTL_MEMBER(Destroy)(HTL_P(NAME)* q)
{
    HTL_FREE(q->queue);
}

HTL_DEF bool HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q)
{
    return q->count == 0;
}

HTL_DEF bool HTL_MEMBER(IsFull)(HTL_P(NAME)* q)
{
    return q->count == q->capacity;
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(PushBack)(HTL_P(NAME)* q)
{
    if(q->count == q->capacity) return NULL;
    q->count++;
    return HTL_MEMBER(Back)(q);
}

HTL_DEF void HTL_MEMBER(PopFront)(HTL_P(NAME)* q)
{
    //q->first = (q->first + 1) % q->capacity;
    q->first++;
    if(q->first == q->capacity) q->first = 0;
    q->count--;
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Front)(HTL_P(NAME)* q)
{
    return &q->queue[q->first];
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Back)(HTL_P(NAME)* q)
{
    //int index = (q->first+q->count) % q->capacity;
    size_t index = q->first+q->count;
    if(index >= q->capacity) index -= q->capacity;
    return &q->queue[index];
}


HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Find)(HTL_P(NAME)* q, int (*comp1)(void* ctx, HTL_P(TYPE)* content), void* ctx)
{
    size_t j = q->first;
    for(size_t i = 0; i < q->count; ++i)
    {
        HTL_P(TYPE)* elem = &q->queue[j];
        if((*comp1)(elem, ctx) == 0) return elem;
        j++;
        if(j == q->capacity) j = 0;
    }
    return NULL;
}

#endif // HTL_PARAM