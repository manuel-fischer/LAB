[&#8592; `count`](HTL_queue.t.h--queue--count.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `Queue`](HTL_queue.t.h--queue.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `Queue_Destroy`](HTL_queue.t.h--queue--queue_destroy.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_queue.t.h.xmd#L18)</small>
***

# `Queue_Create`
<small>*Function* - **lifetime**</small>  
**Synopsis**

```cpp
bool Queue_Create(Queue* q, size_t capacity)
```

Construct an empty queue at the referenced location.

The function never fails, because no memory allocations
are done, thus always 1 is returned


**Note**  

Alternatively the bytes could be set to 0


## Parameter
**`capacity`** &#8213; maximum amount of elements  
## Preconditions

queue was not already constructed


## Postconditions

the queue is valid to be used with the other functions



## Return Value

Always returns 1 for success


