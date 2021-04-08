[&#8593; HTL_hasharray_util.h](HTL_hasharray_util.h.md) | [&#8594; HTL_HASHARRAY_REMOVE](HTL_hasharray_util.h--htl_hasharray_remove.md)
***

# `HTL_HASHARRAY_EACH`
**Synopsis**

```cpp
void HTL_HASHARRAY_EACH(type array_type, array_type* arr, entry_type*# e, {} ...code)
```

Iterate through the hash array.
Receive pointers to the elements.


## Parameters
**`array_type`** &#8213; hash array type,         text (no alias)  
**`arr`** &#8213; pointer to a hasharray  
**`e`** &#8213; entry pointer,           by name (lvalue)  
**`code`** &#8213; iteration statement,     code block  
