[&#8592; Queue_Construct](HTL_queue.t.h--queue--queue_construct.md) | [&#8593; Queue](HTL_queue.t.h--queue.md) | [&#8594; Queue_PushBack](HTL_queue.t.h--queue--queue_pushback.md)
***

# `Queue_Destruct`
**Synopsis**

```cpp
void Queue_Destruct(Queue* q);
```

Destruct a queue at the referenced location.
Nothing happens, the function exists only for formal reasons.


## Postconditions


All references to entries get invalidated, because
the array gets freed, Construct could be called
again




