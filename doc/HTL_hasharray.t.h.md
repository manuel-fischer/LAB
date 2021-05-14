[&#8592; Template Headers/Template sources](topic-template-headers.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; Table](table.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HTL_hasharray_util.h`](HTL_hasharray_util.h.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_hasharray.t.h.xmd#L1)</small>
***

# `HTL_hasharray.t.h`
<small>*File*</small>  

Hasharray with linear forward probing.

Template header (multiple inclusion)


## Parameters
**`NAME`** &#8213; name of the generated type, prefix of all functions  
**`KEY_TYPE`** &#8213; key type (`KeyType`)  
**`ENTRY_TYPE`** &#8213; entry type (`EntryType`)  
**[`KEY_FUNC`](HTL_hasharray.t.h--key_func.md)** &#8213; key function, takes pointer to entry  
**[`HASH_FUNC`](HTL_hasharray.t.h--hash_func.md)** &#8213; hash function, takes key, return `size_t`  
**[`COMP_FUNC`](HTL_hasharray.t.h--comp_func.md)** &#8213; comparison function, takes key, return 0 if equal  
**[`EMPTY_FUNC`](HTL_hasharray.t.h--empty_func.md)** &#8213; Function to check if entry is empty.  
## Constants
**`HTL_HASHARRAY_LOAD_NUM`** &#8213; load factor numerator, its 3 (lesser than load denominator)  
**`HTL_HASHARRAY_LOAD_DEN`** &#8213; load factor denominator, its 4  
**`HTL_HASHARRAY_GROW_FACTOR`** &#8213; the grow factor, with which the array gets resized, its 2  
**`HTL_HASHARRAY_INITIAL_CAPACITY`** &#8213; the initial capacity of the array, when the first element gets pushed, its 16  
**`HTL_HASHARRAY_CACHE_LAST`** &#8213; 0 or 1, used in `#if-#else` to enable fast cached check (NOT multithreading compatible), its 0  
## Class
**[`HashArray`](HTL_hasharray.t.h--hasharray.md)**  
## Macro Definitions
**[`HTL_CALLOC`](HTL_hasharray.t.h--htl_calloc.md)** &#8213; Required to be defined  
**[`HTL_FREE`](HTL_hasharray.t.h--htl_free.md)** &#8213; Required to be defined  
