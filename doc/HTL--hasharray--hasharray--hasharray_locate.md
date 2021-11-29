[&#8592; `HashArray_Discard`](HTL--hasharray--hasharray--hasharray_discard.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL--hasharray--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_Get`](HTL--hasharray--hasharray--hasharray_get.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray.xmd#L74)</small>
***

# `HashArray_Locate`
<small>*Function* &nbsp; - &nbsp; **observers** &nbsp; - &nbsp; ["HTL/hasharray.t.h"](../include/HTL/hasharray.t.h)</small>  
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


