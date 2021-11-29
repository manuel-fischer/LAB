[&#8592; `HTL_HASHARRAY_REMOVE`](HTL--hasharray_util.h--htl_hasharray_remove.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `hasharray_util.h`](HTL--hasharray_util.h.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HTL_HASHARRAY_REMOVE_DEREF`](HTL--hasharray_util.h--htl_hasharray_remove_deref.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray_util.h.xmd#L29)</small>
***

# `HTL_HASHARRAY_EACH_DEREF`
<small>*Macro Definition*</small>  
**Synopsis**

```cpp
HTL_HASHARRAY_EACH_DEREF(type array_type, array_type* arr, entry_type# e, {} ...code)
```

Iterate through the hash array.
Receive value copy of the elements.


## Parameters
**`array_type`** &#8213; hash array type,         text (no alias)  
**`arr`** &#8213; pointer to a hasharray  
**`e`** &#8213; entry value,             by name (lvalue)  
**`code`** &#8213; iteration statement,     code block  
