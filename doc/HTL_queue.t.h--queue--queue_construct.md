[&#8593; Queue](HTL_queue.t.h--queue.md) | [&#8594; Queue_Destruct](HTL_queue.t.h--queue--queue_destruct.md)
***

# `Queue_Construct`
**Synopsis**

```cpp
bool Queue_Construct(Queue* q)
```

Construct an empty queue at the referenced location.

The function never fails, because no memory allocations
are done, thus always 1 is returned


**Note**  

Alternatively the bytes could be set to 0


## Preconditions

queue was not already constructed


## Postconditions

the queue is valid to be used with the other functions



## Return Value

Always returns 1 for success


