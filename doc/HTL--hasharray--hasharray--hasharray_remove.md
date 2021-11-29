[&#8592; `HashArray_RemoveEntry`](HTL--hasharray--hasharray--hasharray_removeentry.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL--hasharray--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_Discard`](HTL--hasharray--hasharray--hasharray_discard.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray.xmd#L124)</small>
***

# `HashArray_Remove`
<small>*Function* &nbsp; - &nbsp; **modifiers** &nbsp; - &nbsp; ["HTL/hasharray.t.h"](../include/HTL/hasharray.t.h)</small>  
**Synopsis**

```cpp
void HashArray_Remove(HashArray* hasharray, KeyType key)
```

Remove the entry with the given key, if it is present


## Postconditions


All references to entries get invalidated but keep dereferenceable,
because some entries might get relocated but not reallocated


