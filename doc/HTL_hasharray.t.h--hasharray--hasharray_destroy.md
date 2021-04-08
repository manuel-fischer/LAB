[&#8592; HashArray_Create](HTL_hasharray.t.h--hasharray--hasharray_create.md) | [&#8593; HashArray](HTL_hasharray.t.h--hasharray.md)
***

# `HashArray_Destroy`
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


