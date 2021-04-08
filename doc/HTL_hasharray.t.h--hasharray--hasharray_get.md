[&#8592; HashArray_PutAlloc](HTL_hasharray.t.h--hasharray--hasharray_putalloc.md) | [&#8593; HashArray](HTL_hasharray.t.h--hasharray.md) | [&#8594; HashArray_RemoveEntry](HTL_hasharray.t.h--hasharray--hasharray_removeentry.md)
***

# `HashArray_Get`
**Synopsis**

```cpp
EntryType* HashArray_Get(HashArray* hasharray, KeyType key)
```

Locate the element with the given key, if it is present.

## Postconditions

The function does not change the array


## Return Value

The element or `NULL` if it was not found


