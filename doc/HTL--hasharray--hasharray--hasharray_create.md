[&#8592; `cached_entry`](HTL--hasharray--hasharray--cached_entry.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `HashArray`](HTL--hasharray--hasharray.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `HashArray_Destroy`](HTL--hasharray--hasharray--hasharray_destroy.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/HTL/hasharray.xmd#L54)</small>
***

# `HashArray_Create`
<small>*Function* &nbsp; - &nbsp; **lifetime** &nbsp; - &nbsp; ["HTL/hasharray.t.h"](../include/HTL/hasharray.t.h)</small>  
**Synopsis**

```cpp
bool HashArray_Create(HashArray* hasharray)
```


Create an empty hasharray at the referenced location


**Note**  

Alternatively the bytes could be set to 0


## Preconditions

hasharray was not already constructed

## Postconditions

the hasharray is valid to be used with the other functions

## Return Value

Always returns 1 for success

## Side Effects

Clears the memory of the hash array with 0.


