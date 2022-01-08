#pragma once
#include "LAB_block.h"
#include <stdarg.h>
#include <stdbool.h>

// TODO remove

typedef enum LAB_BlockField
{
    LAB_BlockField_flags = 1,
    LAB_BlockField_lum,
    LAB_BlockField_dia,
    //LAB_BlockField_model,
    LAB_BlockField_bounds,
} LAB_BlockField;

LAB_INLINE
bool LAB_Block_Set_VL(LAB_Block* b, va_list ap)
{
    LAB_BlockField field;
    while((field = va_arg(ap, LAB_BlockField)))
    {
        switch (field)
        {
        case LAB_BlockField_flags:
            b->flags = va_arg(ap, LAB_BlockFlags);
            break;

        case LAB_BlockField_lum:
            b->lum = va_arg(ap, LAB_Color);
            break;

        case LAB_BlockField_dia:
            b->dia = va_arg(ap, LAB_Color);
            break;

        /*case LAB_BlockField_model:
            b->model = va_arg(ap, LAB_Model*);
            break;*/

        case LAB_BlockField_bounds:
            b->bounds[0][0] = (float)va_arg(ap, double);
            b->bounds[0][1] = (float)va_arg(ap, double);
            b->bounds[0][2] = (float)va_arg(ap, double);
            b->bounds[1][0] = (float)va_arg(ap, double);
            b->bounds[1][1] = (float)va_arg(ap, double);
            b->bounds[1][2] = (float)va_arg(ap, double);
            break;
        }
    }
    return true;
}

#define LAB_Block_Set(b, ...) LAB_Block_Set_V(b, __VA_ARGS__, 0)
LAB_INLINE bool LAB_Block_Set_V(LAB_Block* b, ... /*0*/)
{
    va_list ap;
    va_start(ap, b);
    bool retval = LAB_Block_Set_VL(b, ap);
    va_end(ap);
    return retval;
}
