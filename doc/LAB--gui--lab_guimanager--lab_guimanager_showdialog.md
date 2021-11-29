[&#8592; `LAB_GuiManager_Destroy`](LAB--gui--lab_guimanager--lab_guimanager_destroy.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `LAB_GuiManager`](LAB--gui--lab_guimanager.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8594; `LAB_GuiManager_Dismiss`](LAB--gui--lab_guimanager--lab_guimanager_dismiss.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB/gui.xmd#L267)</small>
***

# `LAB_GuiManager_ShowDialog`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/manager.h"](../include/LAB/gui/manager.h)</small>  
**Synopsis**

```cpp
void LAB_GuiManager_ShowDialog(LAB_GuiManager* manager, LAB_GuiComponent* component)
```

Show a new dialog

**Note**  

It will take the ownership of `*component`

## Parameters
**`manager`** &#8213; The gui manager  
**`component`** &#8213; The displayed dialog, allocated with `LAB_Malloc`  
