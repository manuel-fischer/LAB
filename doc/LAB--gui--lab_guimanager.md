[&#8592; `LAB_GuiEvent`](LAB--gui--lab_guievent.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `gui`](LAB--gui.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB_GuiButton`](LAB--gui--lab_guibutton.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB/gui.xmd#L234)</small>
***

# `LAB_GuiManager`
<small>*Structure* &nbsp; - &nbsp; ["LAB/gui/manager.h"](../include/LAB/gui/manager.h)</small>  

Manages a single gui dialog

## Attributes
**`component`** &#8213; The current dialog or `NULL` if none is displayed  
**`dismiss_component`** &#8213; A previously dismissed component, that needs to be cleaned up  
**`gl_id`** &#8213; The texture index, that is used to render the dialog  
**`surf`** &#8213; The surface, to which the dialog gets drawn to  
**`rerender`** &#8213; The whole dialog needs to be redrawn  
**`scale`** &#8213; GUI metrics scale factor  
**`zoom`** &#8213; Zoom, finally rendered surface scale factor  
<small>**fonts**</small>  
**`button_font`** &#8213; The font commonly used to render buttons and other components  
**`title_font`** &#8213; The font commonly used to render the title of a dialog  
**`mono_font`** &#8213; Monospace font  
## Functions
**[`LAB_GuiManager_Create`](LAB--gui--lab_guimanager--lab_guimanager_create.md)** &#8213; create the gui manager  
**[`LAB_GuiManager_Destroy`](LAB--gui--lab_guimanager--lab_guimanager_destroy.md)** &#8213; destroy the gui manager  
**[`LAB_GuiManager_ShowDialog`](LAB--gui--lab_guimanager--lab_guimanager_showdialog.md)** &#8213; Show a new dialog  
**[`LAB_GuiManager_Dismiss`](LAB--gui--lab_guimanager--lab_guimanager_dismiss.md)** &#8213; Close the current dialog  
**[`LAB_GuiManager_Tick`](LAB--gui--lab_guimanager--lab_guimanager_tick.md)** &#8213; Tick the GUI  
**[`LAB_GuiManager_Render`](LAB--gui--lab_guimanager--lab_guimanager_render.md)** &#8213; Render the dialog to OpenGL  
**[`LAB_GuiManager_HandleEvent`](LAB--gui--lab_guimanager--lab_guimanager_handleevent.md)** &#8213; Handle an SDL-event  
## TODO

Extend it to a stack of dialogs


