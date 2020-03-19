/*
 * cfp_mdio.c
 *
 *  Created on: Aug 5, 2019
 *      Author: sxd9336
 */

#include "cfp_mdio.h"

// GPIO port is setup such that
// 		pin 0: MDIO
//		pin 1: MCK
XGpio CfpMdio;

int cfp_mdio_read(u8 reg_addr, u8 *read_data) {
	bool write_buffer[14];
	bool read_buffer [16];
	u8 phy_addr = PHY_ID;

	// Start Sequence
	write_buffer[0] = 0;
	write_buffer[1] = 1;

	// Declare intent to write
	write_buffer[2] = 1;
	write_buffer[3] = 0;

	// write 5 bit phy address
	// the MSB is the 1st bit received
	for (int i = 4; i >= 0; i--) {
		write_buffer[i+4] = 0x01 & phy_addr; 	// isolate LSB
		phy_addr = phy_addr >> 1;				// shift new LSB
	}

	// write 5 bit reg address
	for (int i = 4; i >= 0; i--) {
		write_buffer[i+9] = 0x01 & reg_addr;
		reg_addr = reg_addr >> 1;
	}

	// start the GPIO block
	XGpio_Initialize(&CfpMdio, CFP_ID);
	XGpio_SetDataDirection(&CfpMdio, CFP_CHAN, MDIO_WRITE);

	// Send 32 '1's to start communication
	for (int i = 0; i < 32; i++) {
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x1);
		usleep(CLK_PERIOD/4);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x3); // 0b11
		usleep(CLK_PERIOD/2);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x1); // 0b01
		usleep(CLK_PERIOD/4);
	}

	// send buffer with phy_addr & reg_addr
	for (int i = 0; i < 14; i++) {
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0 + write_buffer[i]);
		usleep(CLK_PERIOD/4);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x2 + write_buffer[i]); // 0x2 = 0b10
		usleep(CLK_PERIOD/2);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0 + write_buffer[i]);
		usleep(CLK_PERIOD/4);
	}

	// prepare data line to read
	XGpio_SetDataDirection(&CfpMdio, CFP_CHAN, MDIO_READ);

	// allow 1.5 clock cycles for phy to take control of line
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0);
	usleep(CLK_PERIOD/4);
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x2);
	usleep(CLK_PERIOD/2);
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0);
	usleep(CLK_PERIOD/2);
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x2);

	// read data returned
	u32 data = 0;
	for (int i = 0; i < 16; i++) {
		usleep(CLK_PERIOD/2);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0);

		// read all 32 pins and isolate the data pin
		data = XGpio_DiscreteRead(&CfpMdio, CFP_CHAN);
		read_buffer[i] = (0x1 & data);

		usleep(CLK_PERIOD/2);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x2);

	}

	// convert binary buffer into 2, 1 byte values
	// LSB stored in the highest index
	u8 temp;
	for (int i = 1; i >= 0; i--) {
		temp = 0;
		for (int j = 0; j < 8; j++) {
			temp = (temp << 1) + read_buffer[8*i+j];
		}
		read_data[i] = temp;
	}

	return XST_SUCCESS;
}

int cfp_mdio_write(u8 reg_addr, u16 write_data) {
	bool write_buffer[32];
	u8 phy_addr = PHY_ID;

	// Start Sequence
	write_buffer[0] = 0;
	write_buffer[1] = 1;

	// Declare intent to write
	write_buffer[2] = 0;
	write_buffer[3] = 1;

	// write 5 bit phy address
	// the MSB is the 1st bit received
	for (int i = 4; i >= 0; i--) {
		write_buffer[i+4] = 0x01 & phy_addr; 	// isolate LSB
		phy_addr = phy_addr >> 1;				// shift new LSB
	}

	// write 5 bit reg address
	for (int i = 4; i >= 0; i--) {
		write_buffer[i+9] = 0x01 & reg_addr;
		reg_addr = reg_addr >> 1;
	}

	// Prepare to write
	write_buffer[14] = 1;
	write_buffer[15] = 0;

	// write 16 bit reg data
	for (int i = 15; i >= 0; i--) {
		write_buffer[i+16] = 0x01 & write_data;
		write_data = write_data >> 1;
	}

	// Start the GPIO block
	XGpio_Initialize(&CfpMdio, CFP_ID);
	XGpio_SetDataDirection(&CfpMdio, CFP_CHAN, MDIO_WRITE);

	// Send 32 '1's to start communication
	for (int i = 0; i < 32; i++) {
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x1);
		usleep(CLK_PERIOD/4);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x3); // 0b11
		usleep(CLK_PERIOD/2);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x1); // 0b01
		usleep(CLK_PERIOD/4);
	}

	// 1 clock cycle of data = 0
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0);
	usleep(CLK_PERIOD/4);
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x2); // 0b11
	usleep(CLK_PERIOD/2);
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0); // 0b01
	usleep(CLK_PERIOD/4);

	// send the buffer
	for (int i = 0; i < 32; i++) {
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0 + write_buffer[i]);
		usleep(CLK_PERIOD/4);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x2 + write_buffer[i]); // 0x2 = 0b10
		usleep(CLK_PERIOD/2);
		XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x0 + write_buffer[i]);
		usleep(CLK_PERIOD/4);
	}

	// set clock & data low
	XGpio_DiscreteWrite(&CfpMdio, CFP_CHAN, 0x00);
	return XST_SUCCESS;
}
