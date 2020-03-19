/*
 * cfp_mdio.h
 *
 *  Created on: Aug 5, 2019
 *      Author: sxd9336
 */

#ifndef SRC_CFP_MDIO_H_
#define SRC_CFP_MDIO_H_

#include <stdbool.h>
#include "xparameters.h"
#include <sleep.h>
#include <xgpio.h>
#include <xstatus.h>

#define CFP_ID		XPAR_AXI_GPIO_CFP_DEVICE_ID
#define CFP_CHAN	1
#define MDIO_WRITE	0x00 	// 0b00 both write
#define MDIO_READ	0x01 	// 0b01 clk writes, data reads
#define CLK_PERIOD	1024	// measured in microseconds
#define PHY_ID		31		// slave address of MSP430

int cfp_mdio_read(u8 reg_addr, u8 *read_data);
int cfp_mdio_write(u8 reg_addr, u16 write_data);

#endif /* SRC_CFP_MDIO_H_ */
