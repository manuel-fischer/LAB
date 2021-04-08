[&#8593; table](table.md) | [&#8594; HTL_hasharray_util.h](HTL_hasharray_util.h.md)
***

# `HTL_hasharray.t.h`

Hasharray with linear forward probing

Template header (multiple inclusion)


## Parameters
**`NAME`** &#8213; name of the generated type, prefix of all functions  
**`KEY_TYPE`** &#8213; key type (`KeyType`)  
**`ENTRY_TYPE`** &#8213; entry type (`EntryType`)  
**[`KEY_FUNC`](HTL_hasharray.t.h--key_func.md)** &#8213; key function, takes pointer to entry  
**[`HASH_FUNC`](HTL_hasharray.t.h--hash_func.md)** &#8213; hash function, takes key, return `size_t`  
**[`COMP_FUNC`](HTL_hasharray.t.h--comp_func.md)** &#8213; comparison function, takes key, return 0 if equal  
**[`EMPTY_FUNC`](HTL_hasharray.t.h--empty_func.md)** &#8213; Function to check if entry is empty.  
**[`CALLOC`](HTL_hasharray.t.h--calloc.md)**  
**[`FREE`](HTL_hasharray.t.h--free.md)**  
**`LOAD_NUM`** &#8213; load factor numerator   eg. 3 (lesser than load denominator)  
**`LOAD_DEN`** &#8213; load factor denominator eg. 4  
**`GROW_FACTOR`** &#8213; the grow factor, with which the array gets resized, eg. 2  
**`INITIAL_CAPACITY`** &#8213; the initial capacity of the array, when the first element gets pushed  
**`CACHE_LAST`** &#8213; 0 or 1, used in `#if-#else` to enable fast cached check (NOT multithreading compatible)  
## Class
**[`HashArray`](HTL_hasharray.t.h--hasharray.md)**  
