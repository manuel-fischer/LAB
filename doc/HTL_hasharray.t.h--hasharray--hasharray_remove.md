[&#8592; `HashArray_RemoveEntry`](HTL_hasharray.t.h--hasharray--hasharray_removeentry.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL_hasharray.t.h--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_Discard`](HTL_hasharray.t.h--hasharray--hasharray_discard.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_hasharray.t.h.xmd#L128)</small>
***

# `HashArray_Remove`
<small>*Function* - **modifiers**</small>  
**Synopsis**

```cpp
void HashArray_Remove(HashArray* hasharray, KeyType key)
```

Remove the entry with the given key, if it is present


## Postconditions


All references to entries get invalidated but keep dereferenceable,
because some entries might get relocated but not reallocated


