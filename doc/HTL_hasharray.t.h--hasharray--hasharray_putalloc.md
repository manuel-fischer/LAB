[&#8592; `HashArray_Destroy`](HTL_hasharray.t.h--hasharray--hasharray_destroy.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL_hasharray.t.h--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_RemoveEntry`](HTL_hasharray.t.h--hasharray--hasharray_removeentry.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_hasharray.t.h.xmd#L89)</small>
***

# `HashArray_PutAlloc`
<small>*Function* - **modifiers**</small>  
**Synopsis**

```cpp
EntryType* HashArray_PutAlloc(HashArray* hasharray, KeyType key)
```

Create room for the entry with the given key, if it is not already present

The return value points to NULL, when allocation failed.
If it was already present, the return value points to that entry

The entry itself is not inserted.
To insert an entry, the caller should change the given entry to a
representation, that is not an empty entry,
When the entry was not inserted, the caller should call Discard


## Postconditions


All references to entries get invalidated, because the array
might have been relocated


## See also

HashArray_Discard


