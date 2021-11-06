[&#8592; `HashArray_Create`](HTL_hasharray.t.h--hasharray--hasharray_create.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL_hasharray.t.h--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_PutAlloc`](HTL_hasharray.t.h--hasharray--hasharray_putalloc.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_hasharray.t.h.xmd#L69)</small>
***

# `HashArray_Destroy`
<small>*Function* &nbsp; - &nbsp; **lifetime**</small>  
**Synopsis**

```cpp
void HashArray_Destroy(HashArray* hasharray)
```

Destroy a hasharray at the referenced location
Dynamic memory gets cleaned up


## Postconditions


All references to entries get invalidated, because
the array gets freed, `Construct` could be called
again


