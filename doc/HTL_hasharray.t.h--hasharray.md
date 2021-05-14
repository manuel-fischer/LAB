[&#8592; `EMPTY_FUNC`](HTL_hasharray.t.h--empty_func.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HTL_hasharray.t.h`](HTL_hasharray.t.h.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HTL_CALLOC`](HTL_hasharray.t.h--htl_calloc.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL_hasharray.t.h.xmd#L41)</small>
***

# `HashArray`
<small>*Class*</small>  
## Attributes
**[`capacity`](HTL_hasharray.t.h--hasharray--capacity.md)** &#8213; The capacity of the currently allocated table  
**[`size`](HTL_hasharray.t.h--hasharray--size.md)** &#8213; The number of elements in the hash array  
**[`table`](HTL_hasharray.t.h--hasharray--table.md)** &#8213; The pointer to the buffer  
**[`cached_entry`](HTL_hasharray.t.h--hasharray--cached_entry.md)** &#8213; The last requested entry  
## Functions
<small>**lifetime**</small>  
**[`HashArray_Create`](HTL_hasharray.t.h--hasharray--hasharray_create.md)**  
**[`HashArray_Destroy`](HTL_hasharray.t.h--hasharray--hasharray_destroy.md)**  
<small>**modifiers**</small>  
**[`HashArray_PutAlloc`](HTL_hasharray.t.h--hasharray--hasharray_putalloc.md)**  
**[`HashArray_RemoveEntry`](HTL_hasharray.t.h--hasharray--hasharray_removeentry.md)**  
**[`HashArray_Remove`](HTL_hasharray.t.h--hasharray--hasharray_remove.md)**  
**[`HashArray_Discard`](HTL_hasharray.t.h--hasharray--hasharray_discard.md)**  
<small>**observers**</small>  
**[`HashArray_Locate`](HTL_hasharray.t.h--hasharray--hasharray_locate.md)**  
**[`HashArray_Get`](HTL_hasharray.t.h--hasharray--hasharray_get.md)**  
**[`HashArray_IsEntry`](HTL_hasharray.t.h--hasharray--hasharray_isentry.md)**  
