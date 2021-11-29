[&#8592; `Queue_Destroy`](HTL--queue--queue--queue_destroy.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `Queue`](HTL--queue--queue.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `Queue_PopFront`](HTL--queue--queue--queue_popfront.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/queue.xmd#L53)</small>
***

# `Queue_PushBack`
<small>*Function* &nbsp; - &nbsp; **modifiers** &nbsp; - &nbsp; ["HTL/queue.t.h"](../include/HTL/queue.t.h)</small>  
**Synopsis**

```cpp
ContentType* Queue_PushBack(Queue* q)
```

Creates room for an element at the back


## Return Value


the allocated entry on success (that is `Back()` ),
`NULL` on failure.
It fails, when the queue is full


