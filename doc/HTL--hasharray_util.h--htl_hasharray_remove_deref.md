[&#8592; `HTL_HASHARRAY_EACH_DEREF`](HTL--hasharray_util.h--htl_hasharray_each_deref.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `hasharray_util.h`](HTL--hasharray_util.h.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray_util.h.xmd#L38)</small>
***

# `HTL_HASHARRAY_REMOVE_DEREF`
<small>*Macro Definition*</small>  
**Synopsis**

```cpp
HTL_HASHARRAY_REMOVE_DEREF(type array_type, array_type* arr, entry_type# e, bool# cond, {} ...code)
```


Iterate through the hash array and remove entries.
Receive value copy of the elements.


**Note**  


Do not jump out of `code` after invalidation.
Use `cond` to skip elements instead!
Otherwise the entry wouldn't be removed correctly.


## Parameters
**`array_type`** &#8213; hash array type,         text (no alias)  
**`arr`** &#8213; pointer to a hasharray  
**`e`** &#8213; entry value,             by name (lvalue)  
**`cond`** &#8213; condition,               by name (re-evaluated)  
**`code`** &#8213; cleanup statement,       code block  
## TODO

there should be a way to break out of the loop directly

