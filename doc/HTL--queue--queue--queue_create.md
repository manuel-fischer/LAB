[&#8592; `capacity`](HTL--queue--queue--capacity.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `Queue`](HTL--queue--queue.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `Queue_Destroy`](HTL--queue--queue--queue_destroy.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/queue.xmd#L25)</small>
***

# `Queue_Create`
<small>*Function* &nbsp; - &nbsp; **lifetime** &nbsp; - &nbsp; ["HTL/queue.t.h"](../include/HTL/queue.t.h)</small>  
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

The queue was not already constructed


## Postconditions

The queue is valid to be used with the other functions



## Return Value

`true` on success


