[&#8592; `LAB_GuiComponent`](LAB--gui--lab_guicomponent.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `gui`](LAB--gui.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB_GuiInventoryList`](LAB--gui--lab_guiinventorylist.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L122)</small>
***

# `LAB_GuiContainer`
<small>*Structure* &nbsp; - &nbsp; **components** &nbsp; - &nbsp; ["LAB/gui/container.h"](../include/LAB/gui/container.h)</small>  
## Attributes
**[`components`](LAB--gui--lab_guicontainer--components.md)** &#8213; `NULL`-terminated array of pointers to components.  
**`current`** &#8213; the current focused component or `NULL`  
## Functions
<small>**hooks**</small>  
**`LAB_GuiContainer_OnEvent`** &#8213; The default `on_event` hook for a container  
**`LAB_GuiContainer_Render`** &#8213; The default `render` hook for a container  
**`LAB_GuiContainer_Render_Framed`** &#8213; `render` hook for a container, that also renders a frame around the container  
**`LAB_GuiContainer_Destroy`** &#8213; The default `destroy` hook for a container, which also destroys childs  
## Macro Definition
**`LAB_GUI_CONTAINER_INHERIT`** &#8213; Used to inherit the fields of a `LAB_GuiContainer`  
