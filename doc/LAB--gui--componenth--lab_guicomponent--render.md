[&#8592; `on_event`](LAB--gui--componenth--lab_guicomponent--on_event.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `LAB_GuiComponent`](LAB--gui--componenth--lab_guicomponent.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `destroy`](LAB--gui--componenth--lab_guicomponent--destroy.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L107)</small>
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
**[`x,y`](LAB--gui--componenth--lab_guicomponent--render--xy.md)** &#8213; The global offset into the surface  
