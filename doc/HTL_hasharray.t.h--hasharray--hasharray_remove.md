[&#8592; HashArray_RemoveEntry](HTL_hasharray.t.h--hasharray--hasharray_removeentry.md) | [&#8593; HashArray](HTL_hasharray.t.h--hasharray.md) | [&#8594; HashArray_Discard](HTL_hasharray.t.h--hasharray--hasharray_discard.md)
***

# `HashArray_Remove`
**Synopsis**

```cpp
void HashArray_Remove(HashArray* hasharray, KeyType key)
```

Remove the entry with the given key, if it is present


## Postconditions


All references to entries get invalidated but keep dereferenceable,
because some entries might get relocated but not reallocated


