[&#8593; `util.h`](LAB--gui--utilh.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB_GuiHitTest`](LAB--gui--utilh--lab_guihittest.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB/gui.xmd#L335)</small>
***

# `LAB_GetMouseCoordPtr`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/util.h"](../include/LAB/gui/util.h)</small>  
**Synopsis**

```cpp
bool LAB_GetMouseCoordPtr(SDL_Event* event, int** x, int** y)
```

Get the mouse coordinates from the event, if it is a mouse event

## Parameters
**`event`** &#8213; The event  
**[`x,y`](LAB--gui--utilh--lab_getmousecoordptr--xy.md)** &#8213; Pointers to the coordinates in the event struct  
## Return Value

`true` if the event is a mouse event


