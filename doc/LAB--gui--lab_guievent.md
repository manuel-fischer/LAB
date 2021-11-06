[&#8592; `util.h`](LAB--gui--utilh.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `gui`](LAB--gui.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB_GuiManager`](LAB--gui--lab_guimanager.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L4)</small>
***

# `LAB_GuiEvent`
<small>*Type* &nbsp; - &nbsp; ["LAB/gui.h"](../include/LAB/gui.h)</small>  

Custom events


## Constants
**`LAB_GUI_EVENT_FOCUS`**  
**`LAB_GUI_EVENT_UNFOCUS`**  
**`LAB_GUI_EVENT_DROPITEM`**  
**`LAB_GUI_EVENT_COUNT`** &#8213; The number ov enumeration values in `LAB_GuiEvent`  
## Variable
**`Uint32`** &#8213; LAB_GuiEvent_base The first index of the registered SDL-events  
## Macro Definitions
**[`LAB_GUI_EVENT`](LAB--gui--lab_guievent--lab_gui_event.md)** &#8213; Converts SDL-event back to the corresponding enumeration value  
**[`LAB_IS_GUI_EVENT`](LAB--gui--lab_guievent--lab_is_gui_event.md)** &#8213; Checks if the SDL-event is one of the custom events  
**[`LAB_GUI_EVENT2SDL`](LAB--gui--lab_guievent--lab_gui_event2sdl.md)** &#8213; Converts an enumeration value to the registered SDL-event  
