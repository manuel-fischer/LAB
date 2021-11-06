[&#8592; `LAB_GuiInventoryList_Create`](LAB--gui--inventory_listh--lab_guiinventorylist--lab_guiinventorylist_create.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `LAB_GuiInventoryList`](LAB--gui--inventory_listh--lab_guiinventorylist.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L198)</small>
***

# `LAB_GuiInventoryList_Create`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui/inventory_list.h"](../include/LAB/gui/inventory_list.h)</small>  
**Synopsis**

```cpp
vLAB_GuiInventoryList_Create_Columns(LAB_GuiInventoryList* lst,
                                 int x, int y, int columns,
                                 LAB_IInventory const* inventory,
                                 void* inventory_user);

```


Initializes the inventory list based on the number of columns

## Parameters
**`lst`** &#8213; The inventory list  
**`x,y`** &#8213; The position of the list  
**`colums`** &#8213; The number of columns of the list  
**`inventory,inventory_user`** &#8213; The inventory  
