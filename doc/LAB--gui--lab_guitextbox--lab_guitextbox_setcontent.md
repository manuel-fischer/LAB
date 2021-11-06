[&#8592; `LAB_GuiTextBox_Create`](LAB--gui--lab_guitextbox--lab_guitextbox_create.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `LAB_GuiTextBox`](LAB--gui--lab_guitextbox.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L325)</small>
***

# `LAB_GuiTextBox_SetContent`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/text_box.h"](../include/LAB/gui/text_box.h)</small>  
**Synopsis**

```cpp
bool LAB_GuiTextBox_SetContent(LAB_GuiTextBox* cself, const char* txt);
```

Set the contents of the text box

## Parameters
**`cself`** &#8213; The text box  
**`txt`** &#8213; The new contents  
## Return Value

`true` on success. `false` on failure, that is when the internal memory couldn't be allocated



