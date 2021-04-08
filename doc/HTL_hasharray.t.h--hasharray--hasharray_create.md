[&#8593; HashArray](HTL_hasharray.t.h--hasharray.md) | [&#8594; HashArray_Destroy](HTL_hasharray.t.h--hasharray--hasharray_destroy.md)
***

# `HashArray_Create`
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


