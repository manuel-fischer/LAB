#include "LAB_game.h"

#include "LAB_obj.h"

LAB_STATIC
bool LAB_Game_Obj(LAB_OBJ_Action action)
{
    LAB_BEGIN_OBJ(action);

    LAB_OBJ(LAB_Blocks_Init(),
            LAB_Blocks_Quit(),
            
    LAB_OBJ(LAB_Dimension_Init(),
            LAB_Dimension_Quit(),

        LAB_YIELD_OBJ(true);
    ););
    
    LAB_END_OBJ(false);
}

bool LAB_Game_Init(void) { return LAB_Game_Obj(LAB_OBJ_CREATE); }
void LAB_Game_Quit(void) { LAB_Game_Obj(LAB_OBJ_DESTROY); }