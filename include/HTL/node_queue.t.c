#ifdef HTL_PARAM

HTL_DEF bool HTL_MEMBER(Create)(HTL_P(NAME)* q)
{
    q->head = NULL;
    q->tail = &q->head;
    return true;
}

HTL_DEF void HTL_MEMBER(Destroy)(HTL_P(NAME)* q)
{
    //HTL_MEMBER(DbgValidate)(q);
}

HTL_DEF bool HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q)
{
    //HTL_MEMBER(DbgValidate)(q);

    return q->head == NULL;
}

HTL_DEF bool HTL_MEMBER(PushBack)(HTL_P(NAME)* q, HTL_P(TYPE)* elem)
{
    //HTL_MEMBER(DbgValidate)(q);

    if(HTL_P(PREV)(elem) != NULL) return false;

    HTL_P(PREV)(elem) = q->tail;
    HTL_P(NEXT)(elem) = NULL;

    *q->tail = elem;
    q->tail = &HTL_P(NEXT)(elem);

    q->dbg_size++;

    return true;
}

HTL_DEF HTL_P(TYPE)* HTL_MEMBER(PopFront)(HTL_P(NAME)* q)
{
    //HTL_MEMBER(DbgValidate)(q);

    HTL_ASSERT(q->head != NULL);
    HTL_ASSERT(&q->head == HTL_P(PREV)(q->head));

    HTL_P(TYPE)* first = q->head;

    HTL_MEMBER(Splice)(q, first);
    
    return first;
}

HTL_DEF void HTL_MEMBER(Splice)(HTL_P(NAME)* q, HTL_P(TYPE)* elem)
{
    //HTL_MEMBER(DbgValidate)(q);

    HTL_P(TYPE)** prev = HTL_P(PREV)(elem);
    HTL_P(TYPE)*  next = HTL_P(NEXT)(elem);

    *prev = next;
    if(next)
        HTL_P(PREV)(next) = prev;
    else
        q->tail = prev;

    HTL_P(PREV)(elem) = NULL;
    HTL_P(NEXT)(elem) = NULL;

    q->dbg_size--;
}

HTL_DEF void HTL_MEMBER(DbgValidate)(HTL_P(NAME)* q)
{
    HTL_P(TYPE)** prev = &q->head;
    HTL_P(TYPE)* n;
    size_t counted_size = 0;

    while((n = *prev) != NULL)
    {
        HTL_ASSERT(HTL_P(PREV)(n) == prev);
        counted_size++;
        prev = &HTL_P(NEXT)(n);
    }
    HTL_ASSERT(q->tail == prev);

    HTL_ASSERT(q->dbg_size == counted_size);
}

#endif // HTL_PARAM