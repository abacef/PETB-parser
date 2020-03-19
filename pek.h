/*****************************************************************************
 * file: pek_interface.h
 * description: interface that will be responsible for the interaction with the
 * PEK
 * version: 0.01
 *
 * author: Sean Wisnewski
 * company: Precision Optical Transceivers
 * date: 18 March 2019
 *****************************************************************************/
/***************************** Include Files *********************************/
#ifndef PEK_INTERFACE_H
#define PEK_INTERFACE_H

#include <stdio.h>
#include <math.h>
#include <xil_types.h>
#include <xiic.h>
#include <xparameters.h>
#include <xstatus.h>
#include "platform.h"
#include "qsfp_iic.h"

/************************** Constant Definitions ****************************/

#define PAGE_NUM 					10
#define BUFFER_SIZE 				31
#define CMD_BUFFER_ADDR 			128
#define COMMIT_BYTE_ADDR 			159
#define RETRIEVE_DATA_ADDR_START 	160
#define RETRIEVE_DATA_ADDR_END 		190
#define RESPONSE_BYTE_ADDR 			191

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions ******************************/

/************************** Function Prototypes *****************************/

int pek_gpio_read	(int port, int pin, u8 *dataDest);
int pek_gpio_set	(int port, int pin);
int pek_gpio_clear	(int port, int pin);
int pek_gpio_toggle	(int port, int pin, u8 *dataDest);

int pek_optocoupler (int speed, u8 *dataDest);

/************************** Variable Definitions ****************************/
#endif
