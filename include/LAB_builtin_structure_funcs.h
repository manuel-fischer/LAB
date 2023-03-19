#pragma once

#include "LAB_game_structure.h"


typedef struct LAB_Builtin_TreePalette
{
    LAB_BlockID wood, leaves, ground;
} LAB_Builtin_TreePalette;

typedef struct LAB_Builtin_BuildingPalette
{
    LAB_BlockID corner, wall, floor, ceiling, roof;
} LAB_Builtin_BuildingPalette;


LAB_Game_IStructure LAB_Builtin_Patch(LAB_BlockID block, LAB_BlockID replaced);
LAB_Game_IStructure LAB_Builtin_Plant(LAB_BlockID block);
LAB_Game_IStructure LAB_Builtin_Bush(LAB_BlockID block);
LAB_Game_IStructure LAB_Builtin_Rock(LAB_BlockID block);
LAB_Game_IStructure LAB_Builtin_Tree(LAB_Builtin_TreePalette blocks);
LAB_Game_IStructure LAB_Builtin_LargeTree(LAB_Builtin_TreePalette blocks);
LAB_Game_IStructure LAB_Builtin_Tower(LAB_Builtin_BuildingPalette blocks);
LAB_Game_IStructure LAB_Builtin_House(LAB_Builtin_BuildingPalette blocks);
LAB_Game_IStructure LAB_Builtin_CeilingCrystal(void);
LAB_Game_IStructure LAB_Builtin_Ore(LAB_BlockID block, LAB_BlockID replaced);

