[&#8592; HTL_HASHARRAY_EACH](HTL_hasharray_util.h--htl_hasharray_each.md) | [&#8593; HTL_hasharray_util.h](HTL_hasharray_util.h.md) | [&#8594; HTL_HASHARRAY_EACH_DEREF](HTL_hasharray_util.h--htl_hasharray_each_deref.md)
***

# `HTL_HASHARRAY_REMOVE`
**Synopsis**

```cpp
void HTL_HASHARRAY_REMOVE(type array_type, HashArray* arr, HashEntry*# e, bool# cond, {} ...code)
```

Iterate through the hash array and remove entries.
Receive pointers to the elements.


**Note**  


Do not jump out of `code` after invalidation.
Use `cond` to skip elements instead!
Otherwise the entry wouldn't be removed correctly.


## Parameters
**`array_type`** &#8213; hash array type,         text (no alias)  
**`arr`** &#8213; pointer to a hasharray  
**`e`** &#8213; entry pointer,           by name (lvalue)  
**`cond`** &#8213; condition,               by name (re-evaluated)  
**`...`** &#8213; cleanup statement,       code block  
## TODO

there should be a way to break out of the loop directly


