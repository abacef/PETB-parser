/*
 * vcu108.c
 *
 *  Created on: Jul 3, 2019
 *      Author: Sidney Davis
 */

#include "vcu108.h"

XGpio LedGpio;
XGpio ButtonGpio;
XGpio SwitchGpio;

/*
 *	int vcu108_gpio_read(char *port, int pin)
 *	reads individual pins on a selected port, or all pins on all ports
 *  returns individal values read; success/failure if 'all' are read
 *  
 *	port = "LED",    acceptable pin = [0:7]
 *	port = "BUTTON", acceptable pin = [0:4]
 *	port = "SWITCH", acceptable pin = [0:3]
 *	port = "ALL",    no pin required
 *
 */
int vcu108_gpio_read(char *port, int pin) {
	XGpio_Initialize(&LedGpio, LED);
	XGpio_Initialize(&ButtonGpio, BUTTON);
	XGpio_Initialize(&SwitchGpio, SWITCH);

	XGpio_SetDataDirection(&LedGpio, 	1, GPIO_READ); 
	XGpio_SetDataDirection(&ButtonGpio, 1, GPIO_READ); 
	XGpio_SetDataDirection(&SwitchGpio, 1, GPIO_READ); 

	u32 DataRead;
	u32 IsolateBit;
	u32 FinalResult;

	if (strcmp(strupr(port), "LED") == 0) {
		if ((0 <= pin) && (pin <= 7)) {
			DataRead = XGpio_DiscreteRead(&LedGpio, 1);
			IsolateBit = DataRead & (1 << pin);
			FinalResult = (IsolateBit >> pin);
			return FinalResult;
		} else {
			xil_printf("Pin %d is out of range. Valid range for LED is [0:7]\r\n", pin);
			return XST_INVALID_PARAM;
		}
	} else

	if (strcmp(strupr(port), "BUTTON") == 0) {
		if ((0 <= pin) && (pin <= 4)) {
			DataRead = XGpio_DiscreteRead(&ButtonGpio, 1);
			IsolateBit = DataRead & (1 << pin);
			FinalResult = (IsolateBit >> pin);
			return FinalResult;
		} else {
			xil_printf("Pin %d is out of range. Valid range for BUTTON is [0:4]\r\n", pin);
			return XST_INVALID_PARAM;
		}
	} else

	if (strcmp(strupr(port), "SWITCH") == 0) {
		if ((0 <= pin) && (pin <= 3)) {
			DataRead = XGpio_DiscreteRead(&SwitchGpio, 1);
			IsolateBit = DataRead & (1 << pin);
			FinalResult = (IsolateBit >> pin);
			return FinalResult;
		} else {
			xil_printf("Pin %d is out of range. Valid range for SWITCH is [0:3]\r\n", pin);
			return XST_INVALID_PARAM;
		}
	} else

	if (strcmp(strupr(port), "ALL") == 0) {
		print("Reading all VCU108 GPIO:\r\n");
		print("pin#: 76543210\r\n");
		DataRead = XGpio_DiscreteRead(&LedGpio, 1);
		print("LEDs: ");
		for (int i=7; i>=0; i--) {
			IsolateBit = DataRead & (1 << i);
			FinalResult = (IsolateBit >> i);
			xil_printf("%d", FinalResult);
		}
		DataRead = XGpio_DiscreteRead(&ButtonGpio, 1);
		print("\r\nButtons: ");
		for (int i=4; i>=0; i--) {
			IsolateBit = DataRead & (1 << i);
			FinalResult = (IsolateBit >> i);
			xil_printf("%d", FinalResult);
		}
		DataRead = XGpio_DiscreteRead(&SwitchGpio, 1);
		print("\r\nSwitches: ");
		for (int i=3; i>=0; i--) {
			IsolateBit = DataRead & (1 << i);
			FinalResult = (IsolateBit >> i);
			xil_printf("%d", FinalResult);
		}
		print("\r\n");
		return XST_NO_DATA;
	} else{
		return XST_DEVICE_NOT_FOUND;
	}

}

/*
 *  int vcu108_gpio_clear(int pin)
 *  Sets the LED on the VCU108 at desired pin to 0
 *  valid for pin = [0:7]
 */
int vcu108_gpio_clear(int pin) {
	XGpio_Initialize(&LedGpio, LED);
	XGpio_SetDataDirection(&LedGpio, 1, GPIO_WRITE);

	u32 CurrentOutput;
	u32 WriteData;

	CurrentOutput = XGpio_DiscreteRead(&LedGpio, 1);
	WriteData = CurrentOutput & ~(1 << pin);
	XGpio_DiscreteWrite(&LedGpio, 1, WriteData);
	return XST_SUCCESS;
}

/*
 *  int vcu108_gpio_set(int pin)
 *  Sets the LED on the VCU108 at desired pin to 1
 *  valid for pin = [0:7]
 */
int vcu108_gpio_set(int pin) {
	XGpio_Initialize(&LedGpio, LED);
	XGpio_SetDataDirection(&LedGpio, 1, GPIO_WRITE);

	u32 CurrentOutput;
	u32 WriteData;

	CurrentOutput = XGpio_DiscreteRead(&LedGpio, 1);
	WriteData = CurrentOutput | (1 << pin);
	XGpio_DiscreteWrite(&LedGpio, 1, WriteData);
	return XST_SUCCESS;
}

/*
 *  int vcu108_gpio_toggle(int pin)
 *  Sets the LED on the VCU108 at the desired pin to the value that it isn't
 *  Returns the new value of the selected pin
 *	
 *	valid for pin = [0:7]
 */
int vcu108_gpio_toggle(int pin) {
	XGpio_Initialize(&LedGpio, LED);
	XGpio_SetDataDirection(&LedGpio, 1, GPIO_WRITE);

	u32 CurrentOutput;
	u32 WriteData;
	u32 IsolateBit;
	u32 NewValue;

	CurrentOutput = XGpio_DiscreteRead(&LedGpio, 1);
	WriteData = CurrentOutput ^ (1 << pin);
	XGpio_DiscreteWrite(&LedGpio, 1, WriteData);

	IsolateBit = WriteData & (1 << pin);
	NewValue = IsolateBit >> pin;

	return NewValue;
}



