[&#8592; `title`](LAB--gui--lab_guibutton--title.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `LAB_GuiButton`](LAB--gui--lab_guibutton.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L61)</small>
***

# `LAB_GuiButton_Create`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/button.h"](../include/LAB/gui/button.h)</small>  
**Synopsis**

```cpp
void LAB_GuiButton_Create(LAB_GuiButton* button,
                            int x, int y, int w, int h,
                            const char* title,
                            void(*on_click)(void* user, LAB_GuiManager* mgr),
                            void* user)
```


Initializes the button

## Parameters
**`button`** &#8213; The button  
**`x,y,w,h`** &#8213; The position and size of the button  
**`title`** &#8213; The text displayed on the button  
**`on_click,user`** &#8213; The callback, that gets called when the button is clicked  
