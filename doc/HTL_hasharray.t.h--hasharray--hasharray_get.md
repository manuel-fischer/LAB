[&#8592; `HashArray_Locate`](HTL_hasharray.t.h--hasharray--hasharray_locate.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL_hasharray.t.h--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_IsEntry`](HTL_hasharray.t.h--hasharray--hasharray_isentry.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_hasharray.t.h.xmd#L106)</small>
***

# `HashArray_Get`
<small>*Function* - **observers**</small>  
**Synopsis**

```cpp
EntryType* HashArray_Get(HashArray* hasharray, KeyType key)
```

Locate the element with the given key, if it is present.

## Postconditions

The function does not change the array


## Return Value

The element or `NULL` if it was not found


