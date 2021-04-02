#pragma once

#include "LAB_block.h"

typedef uint16_t LAB_BlockID;

typedef struct LAB_BlockTable
{

} LAB_BlockTable;

typedef struct LAB_StructureTable
{

} LAB_StructureTable;

typedef struct LAB_Table
{
    LAB_BlockTable blocks;
    LAB_StructureTable structures;
} LAB_Table;
