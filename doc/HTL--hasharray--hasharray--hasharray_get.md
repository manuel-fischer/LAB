[&#8592; `HashArray_Locate`](HTL--hasharray--hasharray--hasharray_locate.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL--hasharray--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_IsEntry`](HTL--hasharray--hasharray--hasharray_isentry.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray.xmd#L102)</small>
***

# `HashArray_Get`
<small>*Function* &nbsp; - &nbsp; **observers** &nbsp; - &nbsp; ["HTL/hasharray.t.h"](../include/HTL/hasharray.t.h)</small>  
**Synopsis**

```cpp
EntryType* HashArray_Get(HashArray* hasharray, KeyType key)
```

Locate the element with the given key, if it is present.

## Postconditions

The function does not change the array


## Return Value

The element or `NULL` if it was not found


