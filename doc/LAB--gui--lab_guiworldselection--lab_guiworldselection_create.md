[&#8593; `LAB_GuiWorldSelection`](LAB--gui--lab_guiworldselection.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB/gui.xmd#L358)</small>
***

# `LAB_GuiWorldSelection_Create`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/world_selection.h"](../include/LAB/gui/world_selection.h)</small>  
**Synopsis**

```cpp
void LAB_GuiWorldSelection_Create(LAB_GuiWorldSelection* gui, const char* title, LAB_WorldSelectProc on_select)
```

Initialize the world selection

## Parameters
**`gui`** &#8213; The world selection dialog  
**`title`** &#8213; The title of the dialog  
**`on_select`** &#8213; Callback, when a world was selected  
