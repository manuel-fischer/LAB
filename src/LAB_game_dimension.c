#include "LAB_game_dimension.h"

#include "LAB_obj.h"




static LAB_IDimension main_dimension = {0};



bool LAB_Dimension_Init(void) { return true; }
void LAB_Dimension_Quit(void)
{
    if(main_dimension.destroy)
        main_dimension.destroy(main_dimension.ctx);
}

LAB_IDimension* LAB_Dimension_GetDefault(void) { return &main_dimension; }


bool LAB_Dimension_Register(const LAB_IDimension* dim)
{
    main_dimension = *dim;
    return true;
}

bool LAB_Dimension_FinishInit(void)
{
    bool success = true;
    if(main_dimension.finish_init)
        success &= main_dimension.finish_init(main_dimension.ctx);

    return success;
}