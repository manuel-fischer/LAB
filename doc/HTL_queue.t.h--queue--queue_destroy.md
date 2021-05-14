[&#8592; `Queue_Create`](HTL_queue.t.h--queue--queue_create.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `Queue`](HTL_queue.t.h--queue.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `Queue_PushBack`](HTL_queue.t.h--queue--queue_pushback.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_queue.t.h.xmd#L35)</small>
***

# `Queue_Destroy`
<small>*Function* - **lifetime**</small>  
**Synopsis**

```cpp
void Queue_Destroy(Queue* q);
```

Destruct a queue at the referenced location.
Nothing happens, the function exists only for formal reasons.


## Postconditions


All references to entries get invalidated, because
the array gets freed, Construct could be called
again




