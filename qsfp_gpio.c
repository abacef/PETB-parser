/*****************************************************************************
 * file: qsfp_gpio.c
 * description: GPIO interface for QSFP on the PETB.
 * device: vcu108
 * version: 0.03
 *
 * author: Irfan Punekar
 * company: Precision Optical Transceivers
 * date: 02 October 2018
 *****************************************************************************/

#include "qsfp_gpio.h"

XGpio QsfpGpio; /* The driver instance for GPIO QSFP Device */

/*
 *	int qsfp_gpio_read(char *port)
 *	reads value on a selected port
 *
 *	Available ports: 'modprs', 'int', 'modsel', 'reset', 'lpmode', or 'all'
 */
int qsfp_gpio_read(char *port) {
	XGpio_Initialize(&QsfpGpio, QSFP_ID);
	
	XGpio_SetDataDirection(&QsfpGpio, 1, GPIO_READ);
	XGpio_SetDataDirection(&QsfpGpio, 2, GPIO_READ);

	u32 DataRead;
	u32 IsolateBit;
	u32 FinalResult;

	if (strcmp(strupr(port), "MODPRS") == 0) {
		DataRead = XGpio_DiscreteRead(&QsfpGpio, 1);
		IsolateBit = DataRead & (1 << MODPRS);
		FinalResult = (IsolateBit >> MODPRS);
		return FinalResult;
	} else

	if (strcmp(strupr(port), "INT") == 0) {
		DataRead = XGpio_DiscreteRead(&QsfpGpio, 1);
		IsolateBit = DataRead & (1 << INT);
		FinalResult = (IsolateBit >> INT);
		return FinalResult;
	} else

	if (strcmp(strupr(port), "MODSEL") == 0) {
		DataRead = XGpio_DiscreteRead(&QsfpGpio, 2);
		IsolateBit = DataRead & (1 << MODSEL);
		FinalResult = (IsolateBit >> MODSEL);
		return FinalResult;
	} else

	if (strcmp(strupr(port), "LPMODE") == 0) {
		DataRead = XGpio_DiscreteRead(&QsfpGpio, 2);
		IsolateBit = DataRead & (1 << LPMODE);
		FinalResult = (IsolateBit >> LPMODE);
		return FinalResult;
	} else

	if (strcmp(strupr(port), "RESET") == 0) {
		DataRead = XGpio_DiscreteRead(&QsfpGpio, 2);
		IsolateBit = DataRead & (1 << RESET);
		FinalResult = (IsolateBit >> RESET);
		return FinalResult;
	} else

	if (strcmp(strupr(port), "ALL") == 0) {
		xil_printf("Reading all QSFP GPIO:\r\n");
		
		DataRead = XGpio_DiscreteRead(&QsfpGpio, 1);
		IsolateBit = DataRead & (1 << MODPRS);
		FinalResult = (IsolateBit >> MODPRS);
		xil_printf("Module Present: %d\r\n", FinalResult);
		
		DataRead = XGpio_DiscreteRead(&QsfpGpio, 1);
		IsolateBit = DataRead & (1 << INT);
		FinalResult = (IsolateBit >> INT);
		xil_printf("Interrupt:      %d\r\n", FinalResult);

		DataRead = XGpio_DiscreteRead(&QsfpGpio, 2);
		IsolateBit = DataRead & (1 << MODSEL);
		FinalResult = (IsolateBit >> MODSEL);
		xil_printf("Module Select:  %d\r\n", FinalResult);

		DataRead = XGpio_DiscreteRead(&QsfpGpio, 2);
		IsolateBit = DataRead & (1 << LPMODE);
		FinalResult = (IsolateBit >> LPMODE);
		xil_printf("Low Power Mode: %d\r\n", FinalResult);

		DataRead = XGpio_DiscreteRead(&QsfpGpio, 2);
		IsolateBit = DataRead & (1 << RESET);
		FinalResult = (IsolateBit >> RESET);
		xil_printf("Reset:          %d\r\n", FinalResult);

		return XST_NO_DATA;
		
	} else{
		return XST_INVALID_PARAM;
	}

}

/*
 *  int qsfp_gpio_set(char *port)
 *  Sets the desired port to 1
 *  
 *  Available ports: 'modsel', 'reset', 'lpmode', or 'all'
 */
int qsfp_gpio_set(char *port) {
	XGpio_Initialize(&QsfpGpio, QSFP_ID);
	XGpio_SetDataDirection(&QsfpGpio, 2, GPIO_WRITE);

	u32 CurrentOutput;
	u32 WriteData;

	if (strcmp(strupr(port), "MODSEL") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput | (1 << MODSEL);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);
		return XST_SUCCESS;
	} else

	if (strcmp(strupr(port), "LPMODE") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput | (1 << LPMODE);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);
		return XST_SUCCESS;
	} else

	if (strcmp(strupr(port), "RESET") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput | (1 << RESET);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);
		return XST_SUCCESS;
	} else

	if (strcmp(strupr(port), "ALL") == 0) {
		XGpio_DiscreteWrite(&QsfpGpio, 2, 0x7); // binary 0b111
		return XST_SUCCESS;
	
	} else {
		return XST_INVALID_PARAM;
	}
}

/*
 *  int qsfp_gpio_clear(char *port)
 *  Sets the desired port to 0
 *  
 *  Available ports: 'modsel', 'reset', 'lpmode', or 'all'
 */
int qsfp_gpio_clear(char *port) {
	XGpio_Initialize(&QsfpGpio, QSFP_ID);
	XGpio_SetDataDirection(&QsfpGpio, 2, GPIO_WRITE);

	u32 CurrentOutput;
	u32 WriteData;

	if (strcmp(strupr(port), "MODSEL") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput & ~(1 << MODSEL);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);
		return XST_SUCCESS;
	} else

	if (strcmp(strupr(port), "LPMODE") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput & ~(1 << LPMODE);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);
		return XST_SUCCESS;
	} else

	if (strcmp(strupr(port), "RESET") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput & ~(1 << RESET);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);
		return XST_SUCCESS;
	} else

	if (strcmp(strupr(port), "ALL") == 0) {
		XGpio_DiscreteWrite(&QsfpGpio, 2, 0x0);
		return XST_SUCCESS;
	
	} else {
		return XST_INVALID_PARAM;
	}
}

/*
 *  int qsfp_gpio_toggle(char *port)
 *  Toggle the desired port
 *  Returns the new value of individual ports, success/failure if 'all' are toggled
 *  
 *  Available ports: 'modsel', 'reset', 'lpmode', or 'all'
 */
int qsfp_gpio_toggle(char *port) {
	XGpio_Initialize(&QsfpGpio, QSFP_ID);
	XGpio_SetDataDirection(&QsfpGpio, 2, GPIO_WRITE);

	u32 CurrentOutput;
	u32 WriteData;
	u32 IsolateBit;
	u32 NewValue;

	if (strcmp(strupr(port), "MODSEL") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput ^ (1 << MODSEL);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);

		IsolateBit = WriteData & (1 << MODSEL);
		NewValue = IsolateBit >> MODSEL;

		return NewValue;
	} else

	if (strcmp(strupr(port), "LPMODE") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput ^ (1 << LPMODE);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);

		IsolateBit = WriteData & (1 << LPMODE);
		NewValue = IsolateBit >> LPMODE;

		return NewValue;
	} else

	if (strcmp(strupr(port), "RESET") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput ^ (1 << RESET);
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);

		IsolateBit = WriteData & (1 << RESET);
		NewValue = IsolateBit >> RESET;

		return NewValue;
	} else

	if (strcmp(strupr(port), "ALL") == 0) {
		CurrentOutput = XGpio_DiscreteRead(&QsfpGpio, 2);
		WriteData = CurrentOutput ^ 0x7; // binary 0b111
		XGpio_DiscreteWrite(&QsfpGpio, 2, WriteData);
		return WriteData;
	
	} else {
		return XST_INVALID_PARAM;
	}
}
