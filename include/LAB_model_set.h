#pragma once

#include "LAB_model.h"
#include <stdbool.h>


#include "LAB_memory.h"
#include "LAB_util.h"

struct LAB_ModelNode
{
    LAB_Model model;
    struct LAB_ModelNode* next;
};

typedef struct LAB_ModelSet
{
    struct LAB_ModelNode* head;
} LAB_ModelSet;

LAB_INLINE
bool LAB_ModelSet_Create(LAB_ModelSet* s)
{
    s->head = NULL;
    return true;
}

LAB_INLINE
void LAB_ModelSet_Destroy(LAB_ModelSet* s)
{
    struct LAB_ModelNode* p,* next;
    p = s->head;
    while(p)
    {
        next = p->next;
        LAB_Free(p->model.data);
        LAB_Free(p);
        p = next;
    }
}

LAB_INLINE
LAB_Model* LAB_ModelSet_NewModel(LAB_ModelSet* s)
{
    struct LAB_ModelNode* n = LAB_Malloc(sizeof(*n));
    if(n == NULL) return NULL;
    LAB_ObjClear(&n->model);
    n->next = s->head;
    s->head = n;
    return &n->model;
}