#ifdef HTL_PARAM

HTL_DEF bool HTL_MEMBER(Create)(HTL_P(NAME)* q, size_t capacity)
{
    // Overflow
    if(HTL_SEMAPHORE_CREATE(&q->sem_cnt, 0))
    {
        if(HTL_SEMAPHORE_CREATE(&q->sem_free, capacity))
        {
            if(HTL_MUTEX_CREATE(&q->mtx_lock))
            {
                q->queue = HTL_MALLOC(capacity*sizeof(HTL_P(TYPE)));
                if(q->queue != NULL)
                {
                    q->first = q->count = 0;
                    q->capacity = capacity;
                    return true;
                }
                HTL_MUTEX_DESTROY(&q->mtx_lock);
            }
            HTL_SEMAPHORE_DESTROY(&q->sem_free);
        }
        HTL_SEMAPHORE_DESTROY(&q->sem_cnt);
    }
    return false;
}

HTL_DEF void HTL_MEMBER(Destroy)(HTL_P(NAME)* q)
{
    HTL_FREE((void*)q->queue);
    HTL_MUTEX_DESTROY(&q->mtx_lock);
    HTL_SEMAPHORE_DESTROY(&q->sem_free);
    HTL_SEMAPHORE_DESTROY(&q->sem_cnt);
}

HTL_DEF bool HTL_MEMBER(IsEmpty)(HTL_P(NAME)* q)
{
    HTL_MUTEX_LOCK(&q->mtx_lock);
    bool empty = q->count == 0;
    HTL_MUTEX_UNLOCK(&q->mtx_lock);
    return empty;
}

HTL_DEF bool HTL_MEMBER(IsFull)(HTL_P(NAME)* q)
{
    HTL_MUTEX_LOCK(&q->mtx_lock);
    bool full = q->count == q->capacity;
    HTL_MUTEX_UNLOCK(&q->mtx_lock);
    return full;
}

HTL_DEF void HTL_MEMBER(WaitPushBack)(HTL_P(NAME)* q, HTL_P(TYPE)* in)
{
    HTL_SEMAPHORE_WAIT(&q->sem_free);
    HTL_MUTEX_LOCK(&q->mtx_lock);

    size_t index = q->first+q->count;
    if(index >= q->capacity) index -= q->capacity;

    //memcpy(&q->queue[index], in, sizeof(*in));
    q->queue[index] = *in;

    ++q->count;

    //LAB_DbgPrintf(HTL_STR(HTL_P(NAME))" (%p) Psh: %i\n", q, q->count);

    HTL_MUTEX_UNLOCK(&q->mtx_lock);
    HTL_SEMAPHORE_POST(&q->sem_cnt);
}

HTL_DEF void HTL_MEMBER(WaitPopFront)(HTL_P(NAME)* q, HTL_P(TYPE)* out)
{
    HTL_SEMAPHORE_WAIT(&q->sem_cnt);
    HTL_MUTEX_LOCK(&q->mtx_lock);

    //memcpy(out, &q->queue[q->first], sizeof(*out));
    *out = q->queue[q->first];

    ++q->first;
    if(q->first == q->capacity) q->first = 0;
    --q->count;

    //LAB_DbgPrintf(HTL_STR(HTL_P(NAME)) " (%p) Pop: %i\n", q, q->count);

    HTL_MUTEX_UNLOCK(&q->mtx_lock);
    HTL_SEMAPHORE_POST(&q->sem_free);
}

#endif // HTL_PARAM