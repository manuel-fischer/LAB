[&#8592; `on_event`](LAB--gui--lab_guicomponent--on_event.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `LAB_GuiComponent`](LAB--gui--lab_guicomponent.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `destroy`](LAB--gui--lab_guicomponent--destroy.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L96)</small>
***

# `render`
<small>*Attribute* &nbsp; - &nbsp; ["LAB/gui/component.h"](../include/LAB/gui/component.h)</small>  
**Synopsis**

```cpp
void (*render)(LAB_GuiComponent* self, LAB_GuiManager* mgr, SDL_Surface* surf, int x, int y)
```

## Parameters
**`self`** &#8213; The component as a `LAB_GuiComponent`  
**`mgr`** &#8213; The GUI manager  
**`surf`** &#8213; The surface to draw to  
**[`x,y`](LAB--gui--lab_guicomponent--render--xy.md)** &#8213; The global offset into the surface  
