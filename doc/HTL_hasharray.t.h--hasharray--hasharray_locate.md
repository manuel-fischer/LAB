[&#8593; HashArray](HTL_hasharray.t.h--hasharray.md) | [&#8594; HashArray_PutAlloc](HTL_hasharray.t.h--hasharray--hasharray_putalloc.md)
***

# `HashArray_Locate`
**Synopsis**

```cpp
EntryType* HashArray_Locate(HashArray* hasharray, KeyType key)
```

Locate the element with the given key or where it should be


**Note**  

You should use `Put` to insert elements


## Preconditions


The element is available in the array
or there is enough room for the new element


## Postconditions

Return value points to a valid memory location


