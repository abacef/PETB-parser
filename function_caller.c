/*
 * function_caller.c
 *
 *  Created on: Aug 8, 2019
 *      Author: sxd9336
 */

#include "function_caller.h"

/*------------------------------------------------------- VCU108 --------------------------------------------------------------------*/
/*
 *  void vcu108_gpio_debug(void)
 *  tests every functionality of the VCU108 to ensure proper operation
 */

int status;
 
void call_vcu108_gpio_debug(void){
	xil_printf("Welcome to the VCU108 GPIO Debugger.\r\n");
	
	//--------------------------------BEGIN GPIO TESTS--------------------------------//
	
	xil_printf("\r\nTests to perform for GPIO include:\r\n");
	xil_printf("\tRead Buttons\r\n\tRead Switches\r\n\tWrite LEDs\r\n");
	
	// Test buttons
	xil_printf("\r\n----------Start Buttons Test----------\r\n");
	char *north = "North";
	char *south = "South";
	char *east = "East";
	char *west = "West";
	char *center = "Center";
	char *button_names[5] = {center, west, south, east, north};
	for (int i = 4; i >= 0; i--) {
		xil_printf("\r\nPress Button[%d] (%s): ", i, button_names[i]);
		while(vcu108_gpio_read("BUTTON", i) != 1); // wait until Button[i] becomes 1
		xil_printf("Success\r\n");
	}
	xil_printf("\r\n----------End Buttons Test----------\r\n");
	
	// Test switches
	xil_printf("\r\n----------Start Switches Test----------\r\n");
	for (int i = 0; i < 4; i++) {
		xil_printf("\r\nSet Switch[%d] High: ", i);
		while(vcu108_gpio_read("SWITCH", i) != 1); // wait until Switch[i] becomes 1
		xil_printf("Success\r\nSet Switch[%d] Low: ", i);
		while(vcu108_gpio_read("SWITCH", i) != 0); // wait until Switch[i] becomes 0
		xil_printf("Success\r\n");
	}
	xil_printf("\r\n----------End Switches Test----------\r\n");
	
	// Test LEDs
	xil_printf("\r\n----------Start LEDs Test----------\r\n");
	
	xil_printf("\r\nClearing all LEDs first: ");
	for (int i = 0; i < 8; i++) vcu108_gpio_clear(i);
	xil_printf("Success\r\n");
	
	xil_printf("\r\nWhen you press the Center Button, each LED from 0 to 7");
	xil_printf("\r\nwill toggle on for 1 second, then off for another second.\r\n");
	xil_printf("\r\nYou may begin when ready...\r\n");
	while(vcu108_gpio_read("BUTTON", 0) != 1); // wait until Center Button  becomes 1
	
	for (int i = 0; i < 8; i++) {
		// Turn LED on with SET
		xil_printf("\r\nLED[%d] = 1\r\n", i);
		vcu108_gpio_set(i);
		sleep(1);
		
		// Turn LED off with TOGGLE
		xil_printf("LED[%d] = 0\r\n", i);
		vcu108_gpio_toggle(i);
		sleep(1);
	}
	xil_printf("\r\n----------End LEDs Test----------\r\n");
	
	xil_printf("\r\nThere are no more GPIO tests to perform\r\n");
	
	//--------------------------------END GPIO TESTS--------------------------------//
	
	xil_printf("\r\nNow exiting the VCU108 GPIO Debugger. I hope everything worked for you!\r\n");

}

void call_vcu108_gpio_clear(char *str_pin){
	int pin;
	sscanf(str_pin, "%d", &pin);
		if ((0 <= pin) && (pin <= 7)) {
			vcu108_gpio_clear(pin);
			xil_printf("LED[%d] = 0\r\n", pin);
		} else {
			xil_printf("Pin %d is out of range. LED ranges from [0:7]\r\n", pin);
		}
}

void call_vcu108_gpio_set(char *str_pin){
	int pin;
	sscanf(str_pin, "%d", &pin);
		if ((0 <= pin) && (pin <= 7)) {
		vcu108_gpio_set(pin);
			xil_printf("LED[%d] = 1\r\n", pin);
		} else {
			xil_printf("Pin %d is out of range. LED ranges from [0:7]\r\n", pin);
		}
}
	
void call_vcu108_gpio_toggle(char *str_pin){
	int pin;
	sscanf(str_pin, "%d", &pin);
		if ((0 <= pin) && (pin <= 7)) {
			status = vcu108_gpio_toggle(pin);
			xil_printf("Before: LED[%d] = %d\r\n", pin, (status?0:1));
			xil_printf("After : LED[%d] = %d\r\n", pin, status);
		} else {
			xil_printf("Pin %d is out of range. LED ranges from [0:7]\r\n", pin);
		}
}

void call_vcu108_gpio_read(char *str_port, char *str_pin){
	char *port = str_port;
	int pin;
	sscanf(str_pin, "%d", &pin);
		status = vcu108_gpio_read(port, pin);
		switch (status) {
		case XST_DEVICE_NOT_FOUND:
			// invalid port
			xil_printf("The port you entered does not exist on the VCU108\r\n");
			xil_printf("Please enter: 'led', 'button', 'switch', or 'all'\r\n");
			break;
		case XST_INVALID_PARAM:
			// invalid pin specified by port in function
			print("Please enter a pin within this range\r\n");
			break;
		case XST_NO_DATA:
			// ALL pins were printed, no further action needed
			break;
		default:
			// no errors were passed, therefore it's correct
			xil_printf("%s[%d] = %d\r\n", port, pin, status);
		}
}

/*------------------------------------------------------- QSFP --------------------------------------------------------------------*/

void call_qsfp_gpio_debug(void){
	int status = -1;
	char *modsel 	= "MODSEL";
	char *lpmode	= "LPMODE";
	char *reset		= "RESET";
	char *port_names[3] = {modsel, lpmode, reset};
	
	xil_printf("Welcome to the QSFP GPIO Debugger.\r\n");
	
	//--------------------------------BEGIN GPIO TESTS--------------------------------//
	
	xil_printf("\r\nTests to perform for GPIO include:\r\n");
	xil_printf("\tRead Interrupts & ModPrs\r\n\tRead/Write to ModSel, LPMode, & Reset\r\n");
	
	// test interrupts
	xil_printf("\r\n----------Begin Interrupts Enabled Check----------\r\n");	
	xil_printf("\r\nInterrupts are %s\r\n", (qsfp_gpio_read("INT")?"disabled":"enabled"));
	xil_printf("\r\n----------End Interrupts Enabled Check----------\r\n");
	
	// test Module Present
	xil_printf("\r\n----------Begin Module Presence Check----------\r\n");
	status = qsfp_gpio_read("MODPRS");
	if (status == 0) {
		// a Module is present
		xil_printf("\r\nI've detected a QSFP Module plugged in. I recommend removing this");
		xil_printf("\r\ndevice before continuing, or it may be damaged during testing\r\n");
	} else 
	if (status == 1) {
		// a Module is not present
		xil_printf("\r\n'Module Present' reports there is no QSFP Module plugged in. \r\n");
		xil_printf("If this is a mistake, please remove the module before continuing.\r\n");
	}
	xil_printf("\r\nPress the Center Button on the VCU108 when ready...\r\n");
	while(vcu108_gpio_read("BUTTON", 0) != 1); // wait until Center Button becomes 1
	
	xil_printf("\r\n----------End Module Presence Check----------\r\n");
	
	// test Writable Ports
	xil_printf("\r\n----------Begin Read/Write Test----------\r\n");
	
	xil_printf("\r\nClearing all ports first: ");
	for (int i = 0; i < 3; i++) qsfp_gpio_clear(port_names[i]);
	xil_printf("Success\r\n");
	
	xil_printf("\r\nSetting each port to 1 for a second, then off for a second:\r\n");
	for (int i = 0; i < 3; i++) {
		// Turn on with SET
		qsfp_gpio_set(port_names[i]);
		xil_printf("\r\n%s = %d\r\n", port_names[i], qsfp_gpio_read(port_names[i]));
		sleep(1);
		
		// Turn off with TOGGLE
		qsfp_gpio_toggle(port_names[i]);
		xil_printf("%s = %d\r\n", port_names[i], qsfp_gpio_read(port_names[i]));
		sleep(1);
	}
	xil_printf("\r\n----------End Read/Write Test----------\r\n");
	
	xil_printf("\r\nThere are no more GPIO tests to perform\r\n");
	
	//--------------------------------END GPIO TESTS--------------------------------//
	
	xil_printf("\r\nNow exiting the QSFP GPIO Debugger. I hope everything worked for you!\r\n");	
}

void call_qsfp_gpio_clear(char *str_port){
	char *port = str_port;
	status = qsfp_gpio_clear(port);
		if (status == XST_SUCCESS)
			xil_printf("%s = 0\r\n", port);
		else {
			xil_printf("An error occurred while clearing port: %s\r\n", port);
			xil_printf("The port you entered cannot be written to on a QSFP connection\r\n");
			xil_printf("Please enter: 'modsel', 'reset', 'lpmode', or 'all'\r\n");
		}
}

void call_qsfp_gpio_set(char *str_port){
	char *port = str_port;
	status = qsfp_gpio_set(port);
		if (status == XST_SUCCESS)
			xil_printf("%s = 1\r\n", port);
		else {
			xil_printf("An error occurred while setting port: %s\r\n", port);
			xil_printf("The port you entered cannot be written to on a QSFP connection\r\n");
			xil_printf("Please enter: 'modsel', 'reset', 'lpmode', or 'all'\r\n");
		}
}

void call_qsfp_gpio_toggle(char *str_port){
	char *port = str_port;
	status = qsfp_gpio_toggle(port);
		if (status != XST_INVALID_PARAM) {
			if (strcmp(port, "ALL")==0) {
				u8 rst = (status & (1 << RESET )) >> RESET ;
				u8 lpm = (status & (1 << LPMODE)) >> LPMODE;
				u8 mod = (status & (1 << MODSEL)) >> MODSEL;
				print("Before Toggling:\r\n");
				xil_printf("Reset  = %d\r\n", (rst?0:1));
				xil_printf("LPMode = %d\r\n", (lpm?0:1));
				xil_printf("ModSel = %d\r\n", (mod?0:1));
				print("\r\nAfter Toggling:\r\n");
				xil_printf("Reset  = %d\r\n", rst);
				xil_printf("LPMode = %d\r\n", lpm);
				xil_printf("ModSel = %d\r\n", mod);
			} else {
				xil_printf("Before: %s = %d\r\n", port, (status?0:1));
				xil_printf("After : %s = %d\r\n", port, status);
			}
		} else {
			xil_printf("An error occurred while toggling port: %s\r\n", port);
			xil_printf("The port you entered cannot be written to on a QSFP connection\r\n");
			xil_printf("Please enter: 'modsel', 'reset', 'lpmode', or 'all'\r\n");
		}
}

void call_qsfp_gpio_read(char *str_port){
	char *port = str_port;
	status = qsfp_gpio_read(port);
		switch (status) {
		case XST_INVALID_PARAM:
			// invalid port
			xil_printf("An error occurred while reading port: %s\r\n", port);
			xil_printf("The port you entered does not exist on a QSFP connection\r\n");
			xil_printf("Please enter: 'modprs', 'int', 'modsel', 'reset', 'lpmode', or 'all'\r\n");
			break;
		case XST_NO_DATA:
			// ALL ports were printed, no further action needed
			break;
		default:
			// no errors were passed, therefore it's correct
			xil_printf("%s = %d\r\n", port, status);
		}
}

void call_qsfp_iic_debug(void){
	init_platform();
	XIic_Config qsfp_config;
	qsfp_startup(&qsfp_config);

	xil_printf("\r\n--------------------------------Read RSSI------------------------------ \r\n");

	qsfp_read_rssi();

	xil_printf("\r\n---------------------------Read Serial Number-------------------------- \r\n");

	qsfp_read_serial();

	xil_printf("\r\n------------------------------Read Page----------------------------- \r\n");

	qsfp_read_page();
	qsfp_write_page(0x01);
	qsfp_read_page();
	qsfp_write_page(0x02);
	qsfp_read_page();
	qsfp_write_page(0x03);
	qsfp_read_page();
	qsfp_write_page(0x00);
	qsfp_read_page();

	xil_printf("\r\n----------------------------Read Password--------------------------- \r\n");

	qsfp_read_password();

	xil_printf("\r\n--------------------------Read Vendor Name-------------------------- \r\n");

	qsfp_read_vendor();

	xil_printf("\r\n----------------------Begin Read-All-Memory Test--------------------- \r\n");

	int x;
	int y;
	unsigned int test_length = 1;
	u8 test_dest[test_length];

	for (int page = 0x00; page <= 0x03; page++) {
		qsfp_write_page(page);

		xil_printf("\r\n\t\tRead in Hexadecimal\r\n");
		xil_printf("\r\naddr.\t\t|\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\r\n");
		xil_printf("-----------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
		for (x = 0; x < 16; x++){
			xil_printf("%d - %d   \t|",x*16,x*16+15);
			for (y = 0; y < 16; y++) {
				AddressType_iic test_addr = x*16 + y;
				qsfp_iic_read_buffer(test_dest, test_addr, test_length);
				xil_printf("\t%X", test_dest[0]);
			}
			xil_printf("\r\n\r\n");
		}

		xil_printf("\r\n\t\tRead Characters\r\n");
		xil_printf("\r\naddr.\t\t|\t0\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\r\n");
		xil_printf("-----------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
		for (x = 0; x < 16; x++){
			xil_printf("%d - %d   \t|",x*16,x*16+15);
			for (y = 0; y < 16; y++) {
				AddressType_iic test_addr = x*16 + y ;
				qsfp_iic_read_buffer(test_dest, test_addr, test_length);
				xil_printf("\t%c", test_dest[0]);
			}
			xil_printf("\r\n\r\n");
		}
	}

	xil_printf("\r\n----------------------End of QSFP Debug---------------------- \r\n");

	qsfp_shutdown();

}

void call_qsfp_iic_read(char *str_page, char *str_startAddr, char *str_endAddr){
	u8 data[256];
	int page;
	int startAddr;
	int endAddr;
	sscanf(str_page, "%d", &page);
	sscanf(str_startAddr, "%d", &startAddr);
	sscanf(str_endAddr, "%d", &endAddr);

	if ((0 <= page) && (page <= 3)) {
		if ((0 <= startAddr) && (startAddr <= 255)) {
			if ((0 <= endAddr) && (endAddr <= 255)) {
				if (endAddr >= startAddr) {
					qsfp_iic_read(page, startAddr, endAddr, data);

					unsigned int length = (endAddr - startAddr + 1) ;
					xil_printf("\r\nReading %d byte(s) from address %d to %d:\r\n", length, startAddr, endAddr);

					xil_printf("\r\nData in Hex : [");
					for(int i = 0; i < length; i++){
						xil_printf("%X ", data[i]);
					}
					xil_printf("]\r\n");

					xil_printf("\r\nData in Char: [");
					for(int i = 0; i < length; i++){
						xil_printf("%c", data[i]);
					}
					xil_printf("]\r\n");
				} else {
					print("The end address is smaller than the start address.\r\n");
				}
			} else {
				xil_printf("The end address %d is out of range\r\n", startAddr);
				print("One page allows access to addresses within [0:255]\r\n");
			}
		} else {
			xil_printf("The start address %d is out of range\r\n", startAddr);
			print("One page allows access to addresses within [0:255]\r\n");
		}
	} else {
		xil_printf("Standard QSFP modules typically don't support the page %d\r\n",page);
		print("The valid page range is [0:3]\r\n");
	}
}

void call_qsfp_bert(void){
	// update this information as the project is being finalized
	print("Sorry for the confusion, but a Bit-Error Ratio Test cannot be performed\r\n");
	print("on this terminal. However, you can perform this test if you have Vivado\r\n");
	print("installed. Navigate to 'C:\\Xilinx\\Vivado\\2017.4\\bin' and open the  \r\n");
	print("command line in that location (look in the File tab). Type:\r\n\r\n");
	print("vivado -mode tcl < C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\Full_BERT.tcl\r\n");
	print("\r\nPress Enter, and the test will run. Once complete, you can open the\r\n");
	print("results in 'C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\petb_bert.csv'\r\n");
}

void call_qsfp_eyescan(void){
	// update this information as the project is being finalized
	print("Sorry for the confusion, but an Eye Scan cannot be performed\r\n");
	print("on this terminal. However, you can perform this test if you have Vivado\r\n");
	print("installed. Navigate to 'C:\\Xilinx\\Vivado\\2017.4\\bin' and open the  \r\n");
	print("command line in that location (look in the File tab). Type:\r\n\r\n");
	print("vivado -mode tcl < C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\Full_Eyescan.tcl\r\n");
	print("\r\nPress Enter, and the test will run. Once complete, you can open the\r\n");
	print("results in 'C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\petb_eyescan.csv'\r\n");
}

/*------------------------------------------------------- CFP --------------------------------------------------------------------*/

void call_cfp_gpio_debug(void){
	//cfp_debug_function
	printf("There are no results to display. This function has not been implemented by the developer.");  //delete this print statement once the function has been implemented
}

void call_cfp_gpio_clear(char *str_port){
	//char *port = str_port;
	//cfp_gpio_clear(port);
	printf("There are no results to display. This function has not been implemented by the developer.");  //delete this print statement once the function has been implemented

}

void call_cfp_gpio_set(char *str_port){
	//char *port = str_port;
	//cfp_gpio_clear(port);
	printf("There are no results to display. This function has not been implemented by the developer.");  //delete this print statement once the function has been implemented
}

void call_cfp_gpio_toggle(char *str_port){
	//char *port = str_port;
	//cfp__gpio_toggle(port);
	printf("There are no results to display. This function has not been implemented by the developer.");  //delete this print statement once the function has been implemented
}

void call_cfp_gpio_read(char *str_port){
	//char *port = str_port;
	//cfp_gpio_read(port);
	printf("No results to display. This function has not been implemented by the developer.");  //delete this print statement once the function has been implemented
}

void call_cfp_mdio_read (char *str_addr){
	u8 read_data[2]; // = {0xFA, 0x50};
	int addr;
	sscanf(str_addr, "%d", &addr);

	if ((0 <= addr) && (addr <= 31)) {
		cfp_mdio_read(addr, read_data);
		xil_printf("Data at Address %d = %02X%02X\r\n", addr, read_data[0], read_data[1]);
	} else {
		xil_printf("The address %d is out of range\r\n", addr);
		print("Valid range for CFP memory addresses is [0:31]");
	}
}

void call_cfp_mdio_write (char *str_addr, char *str_write_data){
	int addr;
	unsigned int write_data;
	sscanf(str_addr, "%d", &addr );
	sscanf(str_write_data, "%4X", &write_data);
	if ((0 <= addr) && (addr <= 31)) {
		cfp_mdio_write(addr, write_data);
		xil_printf("Hex data %04X has been written to Address %d\r\n", write_data, addr);
	} else {
		xil_printf("The address %d is out of range\r\n", addr);
		print("Valid range for CFP memory addresses is [0:31]");
	}
}

/*------------------------------------------------------- PEK --------------------------------------------------------------------*/

void pek_gpio_debug_helper(int port, int pin, u8 *dataDest, char *pin_name) {
	// read, clear, read, set, read, toggle, read
	int initial_value = 0;

	xil_printf("\r\nRead   %s = ", pin_name);
	pek_gpio_read(port, pin, dataDest);
	xil_printf("%d\r\n", dataDest[0]);
	initial_value = dataDest[0]; // return to initial value after testing

	xil_printf("Clear  %s:  ", pin_name);
	pek_gpio_clear(port, pin);
	xil_printf("Success\r\n");

	xil_printf("Read   %s = ", pin_name);
	pek_gpio_read(port, pin, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	xil_printf("Set    %s:  ", pin_name);
	pek_gpio_set(port, pin);
	xil_printf("Success\r\n");

	xil_printf("Read   %s = ", pin_name);
	pek_gpio_read(port, pin, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	xil_printf("Toggle %s:  ", pin_name);
	pek_gpio_toggle(port, pin, dataDest);
	xil_printf("Success\r\n");

	xil_printf("Read   %s = ", pin_name);
	pek_gpio_read(port, pin, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	xil_printf("Returning to original value: ");
	if (initial_value == 1)
		pek_gpio_set(port, pin);
	else
		pek_gpio_clear(port, pin);
	pek_gpio_read(port, pin, dataDest);
	xil_printf("%d\r\n", dataDest[0]);
}


void call_pek_gpio_debug (void){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {

	u8 dataDest[31];

	xil_printf("Welcome to the PEK GPIO Debugger.\r\n");

	//--------------------------------BEGIN GPIO TESTS--------------------------------//

	xil_printf("\r\nTests to perform for GPIO include:\r\n");
	xil_printf("\tRead Only Ports-Pin Combos\r\n\tRead/Write Remaining Port/Pin Combos\r\n");

	xil_printf("\r\nFor reference, the notation (PORT.PIN) will be used throughout the tests\r\n");

	// Read from Read-Only Pins on PEK
	xil_printf("\r\n----------Start Read-Only Test----------\r\n");

	xil_printf("\r\nOn-Board Button (1.1) = ");
	pek_gpio_read(1, 1, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	xil_printf("\r\nOptocoupler Output (1.4) = ");
	pek_gpio_read(1, 4, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	xil_printf("\r\nModule Select (2.3) = ");
	pek_gpio_read(2, 3, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	xil_printf("\r\nLow Power Mode (2.4) = ");
	pek_gpio_read(2, 4, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	xil_printf("\r\nReset (5.4) = ");
	pek_gpio_read(5, 4, dataDest);
	xil_printf("%d\r\n", dataDest[0]);

	print("\r\n");
	for (int i = 0; i < 4; i++) {
		xil_printf("Faulty Laser Driver [%d] (3.%d) = ", i, i);
		pek_gpio_read(3, i, dataDest);
		xil_printf("%d\r\n", dataDest[0]);
	}

	print("\r\n");
	for (int i = 0; i < 4; i++) {
		xil_printf("Amplifier Loss of Signal [%d] (3.%d) = ", i, i+4);
		pek_gpio_read(3, i+4, dataDest);
		xil_printf("%d\r\n", dataDest[0]);
	}
	xil_printf("\r\n----------End Read-Only Test----------\r\n");

	xil_printf("\r\n----------Start Read/Write Test----------\r\n");
	pek_gpio_debug_helper(1, 0, dataDest, "On-Board LED (1.0)");
	pek_gpio_debug_helper(2, 0, dataDest, "Optocoupler Input (2.0)");
	pek_gpio_debug_helper(2, 1, dataDest, "Module Present (2.1)");
	pek_gpio_debug_helper(2, 5, dataDest, "Enable Interrupts (2.5)");
	pek_gpio_debug_helper(5, 0, dataDest, "Disable Laser Driver [0] (5.0)");
	pek_gpio_debug_helper(5, 1, dataDest, "Disable Laser Driver [1] (5.1)");
	pek_gpio_debug_helper(5, 2, dataDest, "Disable Laser Driver [2] (5.2)");
	pek_gpio_debug_helper(5, 3, dataDest, "Disable Laser Driver [3] (5.3)");
	pek_gpio_debug_helper(6, 0, dataDest, "Disable Amplifiers [0] (6.0)");
	pek_gpio_debug_helper(6, 1, dataDest, "Disable Amplifiers [1] (6.1)");
	pek_gpio_debug_helper(6, 2, dataDest, "Disable Amplifiers [2] (6.2)");
	pek_gpio_debug_helper(6, 3, dataDest, "Disable Amplifiers [3] (6.3)");
	xil_printf("\r\n----------End Read/Write Test----------\r\n");

	xil_printf("\r\nThere are no more GPIO tests to perform\r\n");

	//--------------------------------END GPIO TESTS--------------------------------//

	xil_printf("\r\nNow exiting the PEK GPIO Debugger. I hope everything worked for you!\r\n");
}
}

void call_pek_gpio_clear (char *str_port, char *str_pin){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
		int port;
		int pin;
		sscanf(str_port, "%d", &port);
		sscanf(str_pin, "%d", &pin );
		if ((0 <= port) && (port <= 6)) {
			if ((0 <= pin) && (pin <= 7)) {
				status = pek_gpio_clear(port, pin);
				switch (status) {
				case XST_SUCCESS:
					xil_printf("The pin %d.%d = 0\r\n", port, pin);
				break;

				case XST_SEND_ERROR:
					xil_printf("Data transmission failure\r\n");
				break;

				case XST_INVALID_PARAM:
					xil_printf("Specified Port & Pin are invalid for that command\r\n");
				break;

				case XST_NO_FEATURE:
					xil_printf("Invalid command\r\n");
				break;

				case XST_RECV_ERROR:
					xil_printf("Data receiving failure\r\n");
				break;

				case XST_DEVICE_BUSY:
					xil_printf("PEK took too long to respond\r\n");
				break;

				case XST_FAILURE:
					xil_printf("Invalid status received from PEK\r\n");
				break;

				default:
					xil_printf("Invalid status received from pek_gpio_clear()\r\n");
				}
			} else {
				xil_printf("The pin %d is out of range. Valid range is [0:7]\r\n", pin);
			}
		} else {
			xil_printf("The port %d is out of range. Valid range is [0:6]\r\n", port);
		}
	}
}

void call_pek_gpio_set (char *str_port, char *str_pin){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
		int port;
		int pin;
		sscanf(str_port, "%d", &port);
		sscanf(str_pin, "%d", &pin );
		if ((0 <= port) && (port <= 6)) {
			if ((0 <= pin) && (pin <= 7)) {
				status = pek_gpio_set(port, pin);
				switch (status) {
				case XST_SUCCESS:
					xil_printf("The pin %d.%d = 1\r\n", port, pin);
				break;

				case XST_SEND_ERROR:
					xil_printf("Data transmission failure\r\n");
				break;

				case XST_INVALID_PARAM:
					xil_printf("Specified Port & Pin are invalid for that command\r\n");
				break;

				case XST_NO_FEATURE:
					xil_printf("Invalid command\r\n");
				break;

				case XST_RECV_ERROR:
					xil_printf("Data receiving failure\r\n");
				break;

				case XST_DEVICE_BUSY:
					xil_printf("PEK took too long to respond\r\n");
				break;

				case XST_FAILURE:
					xil_printf("Invalid status received from PEK\r\n");
				break;

				default:
					xil_printf("Invalid status received from pek_gpio_set()\r\n");
				}
			} else {
				xil_printf("The pin %d is out of range. Valid range is [0:7]\r\n", pin);
			}
		} else {
			xil_printf("The port %d is out of range. Valid range is [0:6]\r\n", port);
		}
	}
}

void call_pek_gpio_toggle (char *str_port, char *str_pin, u8*data){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
		int port;
		int pin;
		sscanf(str_port, "%d", &port);
		sscanf(str_pin, "%d", &pin );
		if ((0 <= port) && (port <= 6)) {
			if ((0 <= pin) && (pin <= 7)) {
				status = pek_gpio_toggle(port, pin, data);
				switch (status) {
				case XST_SUCCESS:
					xil_printf("The pin %d.%d has been toggled\r\n", port, pin);
					xil_printf("The previous value was: %d\r\n", data[0]);
					xil_printf("The new value is now  : %d\r\n", data[1]);
				break;

				case XST_SEND_ERROR:
					xil_printf("Data transmission failure\r\n");
				break;

				case XST_INVALID_PARAM:
					xil_printf("Specified Port & Pin are invalid for that command\r\n");
				break;

				case XST_NO_FEATURE:
					xil_printf("Invalid command\r\n");
				break;

				case XST_RECV_ERROR:
					xil_printf("Data receiving failure\r\n");
				break;

				case XST_DEVICE_BUSY:
					xil_printf("PEK took too long to respond\r\n");
				break;

				case XST_FAILURE:
					xil_printf("Invalid status received from PEK\r\n");
				break;

				default:
					xil_printf("Invalid status received from pek_gpio_toggle()\r\n");
				}
			} else {
				xil_printf("The pin %d is out of range. Valid range is [0:7]\r\n", pin);
			}
		} else {
			xil_printf("The port %d is out of range. Valid range is [0:6]\r\n", port);
		}
	}
}

void call_pek_gpio_read (char *str_port, char *str_pin, u8 *data){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
		int port;
		int pin;
		sscanf(str_port, "%d", &port);
		sscanf(str_pin, "%d", &pin );
		if ((0 <= port) && (port <= 6)) {
			if ((0 <= pin) && (pin <= 7)) {
				status = pek_gpio_read(port, pin, data);
				switch (status) {
				case XST_SUCCESS:
					xil_printf("The pin %d.%d = %d\r\n", port, pin, data[0]);
				break;

				case XST_SEND_ERROR:
					xil_printf("Data transmission failure\r\n");
				break;

				case XST_INVALID_PARAM:
					xil_printf("Specified Port & Pin are invalid for that command\r\n");
				break;

				case XST_NO_FEATURE:
					xil_printf("Invalid command\r\n");
				break;

				case XST_RECV_ERROR:
					xil_printf("Data receiving failure\r\n");
				break;

				case XST_DEVICE_BUSY:
					xil_printf("PEK took too long to respond\r\n");
				break;

				case XST_FAILURE:
					xil_printf("Invalid status received from PEK\r\n");
				break;

				default:
					xil_printf("Invalid status received from pek_gpio_read()\r\n");
				}
			} else {
				xil_printf("The pin %d is out of range. Valid range is [0:7]\r\n", pin);
			}
		} else {
			xil_printf("The port %d is out of range. Valid range is [0:6]\r\n", port);
		}
	}
}

void call_pek_bert (void){
	// update this information as the project is being finalized
	print("Sorry for the confusion, but a Bit-Error Ratio Test cannot be performed\r\n");
	print("on this terminal. However, you can perform this test if you have Vivado\r\n");
	print("installed. Navigate to 'C:\\Xilinx\\Vivado\\2017.4\\bin' and open the  \r\n");
	print("command line in that location (look in the File tab). Type:\r\n\r\n");
	print("vivado -mode tcl < C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\Full_BERT.tcl\r\n");
	print("\r\nPress Enter, and the test will run. Once complete, you can open the\r\n");
	print("results in 'C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\petb_bert.csv'\r\n");
}

void call_pek_eyescan (void){
	// update this information as the project is being finalized
				print("Sorry for the confusion, but an Eye Scan cannot be performed\r\n");
				print("on this terminal. However, you can perform this test if you have Vivado\r\n");
				print("installed. Navigate to 'C:\\Xilinx\\Vivado\\2017.4\\bin' and open the  \r\n");
				print("command line in that location (look in the File tab). Type:\r\n\r\n");
				print("vivado -mode tcl < C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\Full_Eyescan.tcl\r\n");
				print("\r\nPress Enter, and the test will run. Once complete, you can open the\r\n");
				print("results in 'C:\\EVT\\PETB\\Summer_2019\\Vivado\\TCL_scripts\\PETB_R_v2_0\\petb_eyescan.csv'\r\n");
}

void call_pek_iic_debug (void){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
		init_platform();
		XIic_Config qsfp_config;
		qsfp_startup(&qsfp_config);

		xil_printf("\r\n--------------------------------Read RSSI------------------------------ \r\n");

		qsfp_read_rssi();

		xil_printf("\r\n---------------------------Read Serial Number-------------------------- \r\n");

		qsfp_read_serial();

		xil_printf("\r\n------------------------------Page setting----------------------------- \r\n");

		qsfp_read_page();
		qsfp_write_page(0x01);
		qsfp_read_page();
		qsfp_write_page(0x02);
		qsfp_read_page();
		qsfp_write_page(0x03);
		qsfp_read_page();
		qsfp_write_page(0x00);
		qsfp_read_page();

		xil_printf("\r\n----------------------------Password setting--------------------------- \r\n");

		char qsfp_password[4] = {0x00, 0x01, 0x10, 0x11};
		qsfp_read_password();
		qsfp_write_password(qsfp_password);
		qsfp_read_password();

		xil_printf("\r\n--------------------------Vendor name setting-------------------------- \r\n");

		char *newname = "Center for Ducks";
		qsfp_read_vendor();
		qsfp_write_vendor(newname);
		qsfp_read_vendor();

		xil_printf("\r\n---------------------------End of PEK Debug-------------------------- \r\n");

		qsfp_shutdown();
	}
}

void call_pek_iic_read (char *str_page, char *str_startAddr, char *str_endAddr){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
		u8 data[256];
		int page;
		int startAddr;
		int endAddr;
		sscanf(str_page, "%d", &page);
		sscanf(str_startAddr, "%d", &startAddr);
		sscanf(str_endAddr, "%d", &endAddr);

		if ((0 <= page) && (page <= 3)) {
			if ((0 <= startAddr) && (startAddr <= 255)) {
				if ((0 <= endAddr) && (endAddr <= 255)) {
					if (endAddr >= startAddr) {
						qsfp_iic_read(page, startAddr, endAddr, data);

						unsigned int length = (endAddr - startAddr + 1) ;
						xil_printf("\r\nReading %d byte(s) from address %d to %d:\r\n", length, startAddr, endAddr);

						xil_printf("\r\nData in Hex : [");
						for(int i = 0; i < length; i++){
							xil_printf("%X ", data[i]);
						}
						xil_printf("]\r\n");

						xil_printf("\r\nData in Char: [");
						for(int i = 0; i < length; i++){
							xil_printf("%c", data[i]);
						}
						xil_printf("]\r\n");
					} else {
						print("The end address is smaller than the start address.\r\n");
					}
				} else {
					xil_printf("The end address %d is out of range\r\n", startAddr);
					print("One page allows access to addresses within [0:255]\r\n");
				}
			} else {
				xil_printf("The start address %d is out of range\r\n", startAddr);
				print("One page allows access to addresses within [0:255]\r\n");
			}
		} else {
			xil_printf("Standard QSFP modules typically don't support the page %d\r\n",page);
			print("The valid page range is [0:3]\r\n");
		}
	}
}

void call_pek_iic_write (char *str_page, char *str_startAddr, char *str_dataType, char *str_dataList){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
		int page;
		int startAddr;
		sscanf(str_page, "%d", &page);
		sscanf(str_startAddr, "%d", &startAddr);
		char * dataType = str_dataType;
		char * dataList = str_dataList;
		int length = strlen(dataList);
		if ((0 <= page) && (page <= 3)) {
			if ((0 <= startAddr) && (startAddr <= 255)) {
				if ((startAddr + length) <= 255) {
					xil_printf("\r\nWriting %d byte(s) from address %d to %d:\r\n", length, startAddr, startAddr+strlen(dataList));
					qsfp_iic_write(page, startAddr, dataType, dataList);
					xil_printf("\r\nNew Data: %s\r\n",dataList);
				} else {
					print("The data provided will not fit in the requested space\r\n");
					xil_printf("Start Address: %d\r\nData Length: %d\r\nFinal Address: %d\r\n",
							startAddr, length, startAddr+length);
				}
			} else {
				xil_printf("The start address %d is out of range\r\n", startAddr);
				print("One page allows access to addresses within [0:255]\r\n");
			}
		} else {
			xil_printf("The QSFP PEK does not support the page %d\r\n",page);
			print("The valid page range is [0:3]\r\n");
		}

	}
}

void call_pek_optocoupler (char *str_speed, u8 *data){
	if (qsfp_gpio_read("MODPRS") == 1) {
		print("There is no module connected in the QSFP port,\r\n");
		print("or the device is insecure. Please (re)connect \r\n");
		print("the module under test and try again.\r\n");
	} else {
	int speed;
	sscanf(str_speed, "%d", &speed);
	if ((16 <= speed) && (speed <= 1000000)) {
		status = pek_optocoupler(speed, data);
		switch (status) {
		case XST_SUCCESS:
			xil_printf("Number of bits transmitted & received correctly  : %d\r\n", data[0]);
			xil_printf("Number of bits transmitted & received incorrectly: %d\r\n", 256 - data[0]);
		break;

		case XST_SEND_ERROR:
			xil_printf("Data transmission failure\r\n");
		break;

		case XST_INVALID_PARAM:
			xil_printf("Specified Port & Pin are invalid for that command\r\n");
		break;

		case XST_NO_FEATURE:
			xil_printf("Invalid command\r\n");
		break;

		case XST_RECV_ERROR:
			xil_printf("Data receiving failure\r\n");
		break;

		case XST_DEVICE_BUSY:
			xil_printf("PEK took too long to respond\r\n");
		break;

		case XST_FAILURE:
			xil_printf("Invalid status received from PEK\r\n");
		break;

		default:
			xil_printf("Invalid status received from pek_optocoupler()\r\n");
		}
	} else {
		xil_printf("The speed %d Hz is out of range\r\n", speed);
		print("Valid range for speed is between 16 Hz and 1 MHz\r\n");
	}
	}
}
