[&#8593; `component.h`](LAB--gui--componenth.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB_GetMouseCoordPtr`](LAB--gui--componenth--lab_getmousecoordptr.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L99)</small>
***

# `LAB_GuiComponent`
<small>*Structure* &nbsp; - &nbsp; ["LAB/gui/component.h"](../include/LAB/gui/component.h)</small>  
## Attributes
**[`on_event`](LAB--gui--componenth--lab_guicomponent--on_event.md)** &#8213; The callback for any event  
**[`render`](LAB--gui--componenth--lab_guicomponent--render.md)** &#8213; The callback to render the component  
**[`destroy`](LAB--gui--componenth--lab_guicomponent--destroy.md)** &#8213; The callback to destroy the resources of the component  
## Functions
<small>**hooks**</small>  
**`LAB_Gui_OnEvent_Ignore`** &#8213; The default `on_event` hook, that does nothing  
**`LAB_GuiComponent_Destroy_Ignore`** &#8213; The default `destroy` hook, that cleans up nothing  
## Macro Definition
**`LAB_GUI_COMPONENT_INHERIT`** &#8213; Used to inherit the basic fields of a `LAB_GuiComponent` into another struct  
