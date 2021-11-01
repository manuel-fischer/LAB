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

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Push)(HTL_P(NAME)* q, HTL_P(PRIO_TYPE) prio)
{
    // if the queue is full, the element with the highest probabilty
    // is replaced, all positions get invalidated


    // shift elements to the right and find insertion position    
    size_t i = q->first+q->count;
    if(i >= q->capacity) i -= q->capacity;

    size_t r = q->count;
    for(; r > 0; --r)
    {
        size_t i_prev = (i > 0) ? i-1 : q->capacity-1;

        if(!HTL_P(PRIO_HIGHER_THAN)(prio, HTL_P(PRIO)(&q->queue[i_prev])))
            break;
            
        if(r != q->capacity)
            q->queue[i] = q->queue[i_prev];
            
        i = i_prev;
    }
    if(r == q->capacity)
        return NULL;

    if(q->count != q->capacity) ++q->count;

    return &q->queue[i];
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

#endif // HTL_PARAM