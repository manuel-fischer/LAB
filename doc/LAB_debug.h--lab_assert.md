[&#8593; `LAB_debug.h`](LAB_debug.h.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB_debug.h.xmd#L1)</small>
***

# LAB_ASSERT
<small>*Macro Definition*</small>  
**Synopsis**

```cpp

void LAB_ASSERT(bool# cond);
void LAB_ASSUME(bool# cond);
void LAB_PRECONDITION(bool# cond);
void LAB_POSTCONDITION(bool# cond);
```


Assert a condition to be true.
If the condition is false, the program traps if NDEBUG is not defined, otherwise
the codepath with the false condition yields undefined behavior.
The undefined behavior allows the compiler to generate code that is optimal for
the codepath true condition

## Parameter
**`cond`** &#8213; The condition to check  
