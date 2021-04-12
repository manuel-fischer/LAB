[&#8592; Queue_Destruct](HTL_queue.t.h--queue--queue_destruct.md) | [&#8593; Queue](HTL_queue.t.h--queue.md) | [&#8594; Queue_PopFront](HTL_queue.t.h--queue--queue_popfront.md)
***

# `Queue_PushBack`
**Synopsis**

```cpp
ContentType* Queue_PushBack(Queue* q)
```

Creates room for an element at the back


## Return Value


the allocated entry on success (that is `Back()` ),
`NULL` on failure.
It fails, when the queue is full


