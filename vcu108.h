/*
 * vcu108.h
 *
 *  Created on: Jul 3, 2019
 *      Author: Sidney Davis
 */

#ifndef SRC_VCU108_H_
#define SRC_VCU108_H_

#include <xparameters.h>
#include <xgpio.h>
#include <sleep.h>

#define LED 	XPAR_AXI_GPIO_LED_DEVICE_ID
#define BUTTON	XPAR_AXI_GPIO_PUSH_BUTTONS_DEVICE_ID
#define SWITCH 	XPAR_AXI_GPIO_DIP_SWITCHES_DEVICE_ID

#define GPIO_WRITE 0x00000000
#define GPIO_READ  0xFFFFFFFF 

int vcu108_gpio_read	(char *port, int pin);
int vcu108_gpio_clear	(int pin);
int vcu108_gpio_set		(int pin);
int vcu108_gpio_toggle	(int pin);

#endif /* SRC_VCU108_H_ */
