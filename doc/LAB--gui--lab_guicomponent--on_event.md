[&#8593; `LAB_GuiComponent`](LAB--gui--lab_guicomponent.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `render`](LAB--gui--lab_guicomponent--render.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB/gui.xmd#L90)</small>
***

# `on_event`
<small>*Attribute* &nbsp; - &nbsp; ["LAB/gui/component.h"](../include/LAB/gui/component.h)</small>  
**Synopsis**

```cpp
bool (*on_event)(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Event* event)
```

## Parameters
**`self`** &#8213; The component as a `LAB_GuiComponent`  
**`mgr`** &#8213; The GUI manager  
**`event`** &#8213; The event that should be handled  
## Return Value

`true` if component got dirty and needs to be rerendered


