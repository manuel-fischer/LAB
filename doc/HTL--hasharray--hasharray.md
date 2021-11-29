[&#8592; `EMPTY_FUNC`](HTL--hasharray--empty_func.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `hasharray.t.h`](HTL--hasharray.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray.xmd#L36)</small>
***

# `HashArray`
<small>*Class* &nbsp; - &nbsp; ["HTL/hasharray.t.h"](../include/HTL/hasharray.t.h)</small>  
## Attributes
**[`capacity`](HTL--hasharray--hasharray--capacity.md)** &#8213; The capacity of the currently allocated table  
**[`size`](HTL--hasharray--hasharray--size.md)** &#8213; The number of elements in the hash array  
**[`table`](HTL--hasharray--hasharray--table.md)** &#8213; The pointer to the buffer  
**[`cached_entry`](HTL--hasharray--hasharray--cached_entry.md)** &#8213; The last requested entry  
## Functions
<small>**lifetime**</small>  
**[`HashArray_Create`](HTL--hasharray--hasharray--hasharray_create.md)**  
**[`HashArray_Destroy`](HTL--hasharray--hasharray--hasharray_destroy.md)**  
<small>**modifiers**</small>  
**[`HashArray_PutAlloc`](HTL--hasharray--hasharray--hasharray_putalloc.md)**  
**[`HashArray_RemoveEntry`](HTL--hasharray--hasharray--hasharray_removeentry.md)**  
**[`HashArray_Remove`](HTL--hasharray--hasharray--hasharray_remove.md)**  
**[`HashArray_Discard`](HTL--hasharray--hasharray--hasharray_discard.md)**  
<small>**observers**</small>  
**[`HashArray_Locate`](HTL--hasharray--hasharray--hasharray_locate.md)**  
**[`HashArray_Get`](HTL--hasharray--hasharray--hasharray_get.md)**  
**[`HashArray_IsEntry`](HTL--hasharray--hasharray--hasharray_isentry.md)**  
