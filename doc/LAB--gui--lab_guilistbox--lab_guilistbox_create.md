[&#8592; `selected_element`](LAB--gui--lab_guilistbox--selected_element.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `LAB_GuiListBox`](LAB--gui--lab_guilistbox.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB/gui.xmd#L222)</small>
***

# `LAB_GuiListBox_Create`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/list_box.h"](../include/LAB/gui/list_box.h)</small>  
**Synopsis**

```cpp
void LAB_GuiListBox_Create(LAB_GuiListBox* cself,
                            int x, int y, int w, int h,
                            size_t element_count, const char* const* elements,
                            void (*on_selection)(void* user, size_t element),
                            void*  on_selection_user);
```

## Parameters
**`cself`** &#8213; The list box  
**`x,y,w,h`** &#8213; The position and size of the list box  
**`element_count,elements`** &#8213; The elements as strings  
**`on_selection,on_selection_user`** &#8213; Callback on selection  
