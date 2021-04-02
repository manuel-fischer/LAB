#pragma once
/** \file LAB.h
 *
 *  LAB main include file
 */

#include "LAB_error.h"
#include "LAB_window.h"
#include "LAB_world.h"
#include "LAB_view.h"
#include "LAB_input.h"


/**
 *  Init LAB
 *
 *  \return 0 on Error
 */
int LAB_Init(void);

/**
 *  Quit Lab
 *
 *  \pre LAB_Init was called
 */
void LAB_Quit(void);
