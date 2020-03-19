/*****************************************************************************
 * file: qsfp_iic.h
 * description: Top-level functionality for the PETB. Based off of Xilinx IPI.
 * version: 0.01
 *
 * author: Irfan Punekar
 * company: Precision Optical Transceivers
 * date: 01 November 2018
 *****************************************************************************/
/***************************** Include Files *********************************/
#ifndef QSFP_IIC_H
#define QSFP_IIC_H

//#include <xiic_l.h>
//#include <xil_exception.h>
//#include <xil_types.h>

#include <stdlib.h>
#include <xiic.h>
#include <xintc.h>
#include <xparameters.h>
#include <xstatus.h>
#include "platform.h"
#include "qsfp_gpio.h"

/************************** Constant Definitions ****************************/

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions ******************************/
/*
 * The AddressType for ML300/ML310/ML410/ML510 boards should be u16 as the address
 * pointer in the on board EEPROM is 2 bytes.
 * The AddressType for ML403/ML501/ML505/ML507/ML605/SP601/SP605/KC705/ZC702
 * /ZC706 boards should be u8 as the address pointer in the on board EEPROM is
 * 1 bytes.
 */
typedef u8 AddressType_iic;
/************************** Function Prototypes *****************************/
int qsfp_read_page(void);
int qsfp_write_page(int page);
int qsfp_iic_read_buffer(u8 *dest, AddressType_iic addr, u16 length);
int qsfp_iic_write_buffer(char *data, AddressType_iic addr, u16 length);
int qsfp_startup(XIic_Config *qsfp_config);
int qsfp_shutdown(void);
int qsfp_iic_read (int page, AddressType_iic startAddr, AddressType_iic endAddr, u8 *dataRead);
int qsfp_iic_write (int page, AddressType_iic startAddr, char *dataType, char *dataList);

int qsfp_read_serial(void);
int qsfp_read_rssi(void);
int qsfp_read_vendor();
int qsfp_write_vendor(char *data);
int qsfp_read_password();
int qsfp_write_password(char *pwd);


#endif
