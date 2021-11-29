[&#8592; `HashArray_Create`](HTL--hasharray--hasharray--hasharray_create.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL--hasharray--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_PutAlloc`](HTL--hasharray--hasharray--hasharray_putalloc.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray.xmd#L65)</small>
***

# `HashArray_Destroy`
<small>*Function* &nbsp; - &nbsp; **lifetime** &nbsp; - &nbsp; ["HTL/hasharray.t.h"](../include/HTL/hasharray.t.h)</small>  
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


