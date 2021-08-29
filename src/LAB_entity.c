#include "LAB_entity.h"
#include "LAB_memory.h"

// TODO

void LAB_EntitySet_Create(LAB_EntitySet* set)
{
    memset(set, 0, sizeof(*set));
}


void LAB_EntitySet_Destroy(LAB_EntitySet* set)
{
    for(int i = 0; i < LAB_EC_COUNT; ++i)
    {
        /// TODO call destructors here
        LAB_Free(set->components[i]);
    }

    LAB_Free(set->offsets);
    LAB_Free(set->entities);
}
