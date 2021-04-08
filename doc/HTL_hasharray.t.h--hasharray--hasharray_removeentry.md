[&#8592; HashArray_PutAlloc](HTL_hasharray.t.h--hasharray--hasharray_putalloc.md) | [&#8593; HashArray](HTL_hasharray.t.h--hasharray.md) | [&#8594; HashArray_Remove](HTL_hasharray.t.h--hasharray--hasharray_remove.md)
***

# `HashArray_RemoveEntry`
**Synopsis**

```cpp
void HashArray_RemoveEntry(HashArray* hasharray, EntryType* entry)
```

Remove an entry by the given reference to the entry


**Note**  


This function can be used in a loop that iterates over all the
entries linearly from [0] to [hasharray->capacity-1] in hasharray->table
and removes elements conditionally. After this function has been called,
the memory location of entry might contain another valid entry that needs
to be tested. Then it is guaranteed that all elements are visited.


## Preconditions

entry is a valid entry that is not an empty entry


## Postconditions


All references to entries get invalidated but keep dereferenceable,
because some entries might get relocated but not reallocated


