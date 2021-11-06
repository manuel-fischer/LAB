[&#8593; `button.h`](LAB--gui--buttonh.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L51)</small>
***

# `LAB_GuiButton`
<small>*Structure* &nbsp; - &nbsp; ["LAB/gui/button.h"](../include/LAB/gui/button.h)</small>  

Inherits `LAB_GUI_COMPONENT_INHERIT`

## Attributes
**[`title`](LAB--gui--buttonh--lab_guibutton--title.md)** &#8213; The text displayed on the button  
**`text_surf`** &#8213; The surface that is used to draw the text  
**`on_click,user`** &#8213; The callback, that gets called when the button is clicked  
**`state`** &#8213; The state of the button  
## Type
**[`LAB_GuiButtonState`](LAB--gui--buttonh--lab_guibutton--lab_guibuttonstate.md)** &#8213; Enumeration of the button states  
## Functions
**[`LAB_GuiButton_Create`](LAB--gui--buttonh--lab_guibutton--lab_guibutton_create.md)** &#8213; Initializes the button  
<small>**hooks**</small>  
**`LAB_GuiButton_Render`** &#8213; The `render` hook for the button  
**`LAB_GuiButton_OnEvent`** &#8213; The `on_event` hook for the button  
**`LAB_GuiButton_Destroy`** &#8213; The `destroy` hook for the button  
