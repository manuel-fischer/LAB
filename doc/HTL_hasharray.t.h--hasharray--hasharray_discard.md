[&#8592; HashArray_Remove](HTL_hasharray.t.h--hasharray--hasharray_remove.md) | [&#8593; HashArray](HTL_hasharray.t.h--hasharray.md) | [&#8594; HashArray_Locate](HTL_hasharray.t.h--hasharray--hasharray_locate.md)
***

# `HashArray_Discard`
**Synopsis**

```cpp
void HashArray_Discard(HashArray* hasharray, EntryType* entry)
```

When an entry has been allocated, this function allows to do a fast
return of the memory. This should only be called when the last call
to the map was Put (or Put followed by one or more IsEntry calls)
and the entry itself was kept empty, either not written to or changed
to an empty representation. If the entry is kept fully 0, it is
guaranteed that PutAlloc always returns entries that are fully 0,
when no element was found


## See also

HashArray_PutAlloc


## TODO

should locate/get be allowed in between?


