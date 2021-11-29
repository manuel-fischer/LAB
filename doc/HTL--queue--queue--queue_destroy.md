[&#8592; `Queue_Create`](HTL--queue--queue--queue_create.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `Queue`](HTL--queue--queue.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `Queue_PushBack`](HTL--queue--queue--queue_pushback.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/queue.xmd#L42)</small>
***

# `Queue_Destroy`
<small>*Function* &nbsp; - &nbsp; **lifetime** &nbsp; - &nbsp; ["HTL/queue.t.h"](../include/HTL/queue.t.h)</small>  
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




