[&#8592; `LAB_GuiComponent`](LAB--gui--componenth--lab_guicomponent.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `component.h`](LAB--gui--componenth.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB_GuiHitTest`](LAB--gui--componenth--lab_guihittest.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L132)</small>
***

# `LAB_GetMouseCoordPtr`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/component.h"](../include/LAB/gui/component.h)</small>  
**Synopsis**

```cpp
bool LAB_GetMouseCoordPtr(SDL_Event* event, int** x, int** y)
```


Get the mouse coordinates from the event, if it is a mouse event

## Parameters
**`event`** &#8213; The event  
**[`x,y`](LAB--gui--componenth--lab_getmousecoordptr--xy.md)** &#8213; Pointers to the coordinates in the event struct  
## Return Value

`true` if the event is a mouse event


