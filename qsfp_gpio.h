/*****************************************************************************
 * file: qsfp_gpio.h
 * description: Top-level functionality for the PETB. Based off of Xilinx IPI.
 * version: 0.01
 *
 * author: Irfan Punekar
 * company: Precision Optical Transceivers
 * date: 01 November 2018
 * 
 * author: Sidney Davis
 * date: 10 July 2019
 *****************************************************************************/
/***************************** Include Files *********************************/
#ifndef QSFP_GPIO_H
#define QSFP_GPIO_H

#include <sleep.h>
#include <xgpio.h>
#include <xparameters.h>
#include <xstatus.h>
#include "vcu108.h"

/************************** Constant Definitions ****************************/

#define QSFP_ID 	XPAR_AXI_GPIO_QSFP_DEVICE_ID

#define GPIO_WRITE 	0x00000000
#define GPIO_READ  	0xFFFFFFFF

// Channel 1 (Read only)
#define MODPRS	1
#define INT		0

// Channel 2 (Read/Write)
#define MODSEL	2
#define LPMODE	1
#define RESET 	0

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions ******************************/

/************************** Function Prototypes *****************************/
int qsfp_gpio_read		(char *port);
int qsfp_gpio_set		(char *port);
int qsfp_gpio_clear		(char *port);
int qsfp_gpio_toggle	(char *port);

/************************** Variable Definitions ****************************/

#endif
