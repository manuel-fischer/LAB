#ifdef HTL_PARAM

HTL_DEF bool HTL_MEMBER(Create)(HTL_P(NAME)* q)
{
    size_t base_capacity = 16;
    q->queue = HTL_MALLOC(base_capacity*sizeof(HTL_P(TYPE)));
    q->first = q->count = 0;
    q->capacity = base_capacity;
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

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(PushBack)(HTL_P(NAME)* q)
{
    if(q->count == q->capacity)
    {
        size_t new_capacity = q->capacity*2u;
        if(new_capacity*sizeof(HTL_P(TYPE))/sizeof(HTL_P(TYPE)) < q->capacity)
            return NULL; // overflow
        HTL_P(TYPE)* new_buffer = HTL_MALLOC(new_capacity*sizeof(HTL_P(TYPE)));
        if(new_buffer == NULL) return NULL;

        size_t pos = q->first;
        for(size_t i = 0; i < q->count; ++i)
        {
            new_buffer[i] = q->queue[pos];
            pos++;
            if(pos == q->capacity) pos = 0;
        }

        q->first = 0;
        q->capacity = new_capacity;
    }

    q->count++;
    return HTL_MEMBER(Back)(q);
}

HTL_DEF void HTL_MEMBER(PopFront)(HTL_P(NAME)* q)
{
    HTL_ASSERT(q->count != 0);
    //q->first = (q->first + 1) % q->capacity;
    q->first++;
    if(q->first == q->capacity) q->first = 0;
    q->count--;
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Front)(HTL_P(NAME)* q)
{
    HTL_ASSERT(q->count != 0);
    return &q->queue[q->first];
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(Back)(HTL_P(NAME)* q)
{
    HTL_ASSERT(q->count != 0);
    //int index = (q->first+q->count) % q->capacity;
    size_t index = q->first+q->count-1;
    if(index >= q->capacity) index -= q->capacity;
    return &q->queue[index];
}

#endif // HTL_PARAM