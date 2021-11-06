[&#8592; `LAB_GuiQuit`](LAB--gui--lab_guiquit.md)&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;[&#8593; `gui`](LAB--gui.md)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;||&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>[\* xdoc](../xdoc/LAB\gui.xmd#L39)</small>
***

# `LAB_RenderRect`
<small>*Function* &nbsp; - &nbsp; ["LAB/gui.h"](../include/LAB/gui.h)</small>  
**Synopsis**

```cpp
void LAB_RenderRect(SDL_Surface* surf, int scale, int x, int y, int w, int h, int i, int j)
```

Render a rectangle from the gui-spritemap

## Parameters
**`surf`** &#8213; The destination surface to render to  
**`scale`** &#8213; The scaling/size of the pixels  
**`x,y,w,h`** &#8213; The destination rectangle  
**`i,j`** &#8213; The source cell in the gui-spritemap  
## Preconditions

The GUI was initialized

## TODO

Move the spritemap to the gui-manager



