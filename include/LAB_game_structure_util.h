#pragma once

#include "LAB_game_structure.h"

bool LAB_Game_Structure_Make(LAB_Game_Structure* s);
LAB_Game_IStructureDensity LAB_Game_StructureDensity_ProbAndCount(double probability, int min, int max);
LAB_Game_Structure LAB_Game_Structure_RestrictYRange_Move(LAB_Game_Structure s, int min, int max);

LAB_Game_IStructurePlacement LAB_Game_PlaceAnywhere(void);