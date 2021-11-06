[&#8592; `Queue_Destroy`](HTL_queue.t.h--queue--queue_destroy.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `Queue`](HTL_queue.t.h--queue.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `Queue_PopFront`](HTL_queue.t.h--queue--queue_popfront.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_queue.t.h.xmd#L46)</small>
***

# `Queue_PushBack`
<small>*Function* &nbsp; - &nbsp; **modifiers**</small>  
**Synopsis**

```cpp
ContentType* Queue_PushBack(Queue* q)
```

Creates room for an element at the back


## Return Value


the allocated entry on success (that is `Back()` ),
`NULL` on failure.
It fails, when the queue is full


