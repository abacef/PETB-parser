/*****************************************************************************
 * file: menu.h
 * description: Top-level functionality for the PETB. Based off of Xilinx IPI.
 * version: 0.03
 *
 * author: Irfan Punekar
 * company: Precision Optical Transceivers
 * date: 02 October 2018
 *****************************************************************************/
/***************************** Include Files *********************************/
#ifndef MENU_H
#define MENU_H

#include "xparameters.h"	/* defines XPAR values */
#include "xuartns550_l.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xil_printf.h>
#include <xil_types.h>

#include "command_caller.h"
#include "platform.h"
#include "command_constants.h"
#include "pek.h"

/************************** Constant Definitions ****************************/
#define BS 		8
#define ENTER 	13
#define ESC 	27
#define SPACE 	32
#define DEL 	127

#define UP_ARROW	72
#define DOWN_ARROW	80
#define LEFT_ARROW	75
#define RIGHT_ARROW	77

#define TEST_SIZE 	19

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions ******************************/

/************************** Function Prototypes *****************************/

int hello_iic           (void);
int hello_gpio          (void);
int hello_gty           (void);
int hello_drp           (void);
int hello_error_count   (void);

/************************** Variable Definitions ****************************/

#endif
