/*****************************************************************************
 * file: pek_interface.c
  * description: interface that will be responsible for the interaction with the
 * PEK
 * version: 0.01
 *
 * author: Sean Wisnewski
 * company: Precision Optical Transceivers
 * date: 18 March 2019
 *****************************************************************************/

#include "pek.h"

u8 dest[BUFFER_SIZE];
char commitByte[1] = {"1"};

//dest is where to put stuff from response
//make cmd buffer 31 bytes - cmd will include port and pin to use
//page 10, address 128 to put data
//page 10, address 160 as response buffer
//after command entered, poll response byte at address 191 at a 10mS interval until a non-zero value is returned
//should probably do the printing of results in the functions themselves

/***********
 * response values in hex - what's in addr 191
 * 0x1: ack that cmd received correctly, and the cmd has completed execution and the next command may be issued
 * 0x2: ack that the command was received correctly, the cmd has completed execution, a response is available in the cmd buff
 * 0xF1: CRC failure
 * 0xF2: invalid command
 * 0xF3: unrecognized command
 */

void pek_gpio_debug_helper(int , int , u8 *, char *);

void pek_command_commit (void) {
	init_platform();
	XIic_Config qsfp_config;
	qsfp_startup(&qsfp_config);

	unsigned int test_length = 1;
	qsfp_iic_write_buffer(commitByte, COMMIT_BYTE_ADDR, test_length);
}

/**
 * Helper function to continuously read the value at the response byte address to determine
 * when another function can get data out of the resonse buffer. This is the implementation
 * of the polling until a response is ready as specified in the ICD.
 */
int pek_poll_for_response() {
	unsigned int attempts = 0;
	while (attempts <= 100) {
		int result = qsfp_iic_read(PAGE_NUM, RESPONSE_BYTE_ADDR, RESPONSE_BYTE_ADDR, dest);
		if(result == XST_SUCCESS) {
			//something has been written to the response byte
			if(dest[0] != 0) {
				return XST_SUCCESS;
			}
		}
		else {
			//sleep for 1 mSec
			sleep(.001);
			attempts++;
		}
	}
	return XST_FAILURE;
}

/**
 * This function reads the value at the specified port and pin from the PEK
 * @param port: port num to read from
 * @param pin: pin num to read from
 * @param dataDest: 31 byte array to potentially store data read into
 */
int pek_gpio_read(int port, int pin, u8 *dataDest) {
	int result = -1;
	char cmd[4]= {0,0,0,0};
	char portStr[4];
	char pinStr[4];
	sprintf(portStr, "%.2X ", port);
	sprintf(pinStr , "%.2X ", pin );
	strcat(cmd, "01 ");
	strcat(cmd, portStr);
	strcat(cmd, pinStr);
	strcat(cmd, "05");

	//write the cmd to the correct addr to ask the pek to do something
	//cmd should flip the value at [port][pin] and print what returns
	result = qsfp_iic_write(PAGE_NUM, CMD_BUFFER_ADDR, "hex", cmd);

	pek_command_commit();

	if(result == XST_SUCCESS) {
		int status = pek_poll_for_response();
		if (status != XST_SUCCESS) {
			return XST_DEVICE_BUSY;
		}
		//after getting a response, the response val is in dest
		int responseVal = *dest;
		switch(responseVal) {
		case 0x1:
			return XST_RECV_ERROR;
		case 0x2:
		{
			AddressType_iic startAddr = RETRIEVE_DATA_ADDR_START;
			AddressType_iic endAddr = RETRIEVE_DATA_ADDR_END;
			unsigned int length = (endAddr - startAddr + 1) ;
			u8 *dest = dataDest;

			init_platform();
			XIic_Config qsfp_config;
			qsfp_startup(&qsfp_config);

			qsfp_write_page(PAGE_NUM);
			qsfp_iic_read_buffer(dest, startAddr, length);
			qsfp_shutdown();
			return XST_SUCCESS;
		}
		case 0xF1:
			return XST_SEND_ERROR;
		case 0xF2:
			return XST_INVALID_PARAM;
		case 0xF3:
			return XST_NO_FEATURE;
		default:
			return XST_RECV_ERROR;
		}
	}
	else {
		return XST_FAILURE;
	}
}

/**
 * This function sets the value at the specified port and pin from the PEK
 * @param port: port num to read from
 * @param pin: pin num to read from
 */
int pek_gpio_set(int port, int pin) {
	int result = -1;
	char cmd[4]= {0,0,0,0};
	char portStr[4];
	char pinStr[4];
	sprintf(portStr, "%.2X ", port);
	sprintf(pinStr , "%.2X ", pin );
	strcat(cmd, "02 ");
	strcat(cmd, portStr);
	strcat(cmd, pinStr);
	strcat(cmd, "05");

	//write the cmd to the correct addr to ask the pek to do something
	//cmd should flip the value at [port][pin] and print what returns
	result = qsfp_iic_write(PAGE_NUM, CMD_BUFFER_ADDR, "hex", cmd);

	pek_command_commit();

	if(result == XST_SUCCESS) {
		int status = pek_poll_for_response();
		if (status != XST_SUCCESS) {
			return XST_DEVICE_BUSY;
		}
		//after getting a response, the response val is in dest
		int responseVal = *dest;
		switch(responseVal) {
		case 0x1:
			return XST_SUCCESS;
		//case 0x2 will never happen b/c nothing is ever returned from the set function
		case 0xF1:
			return XST_SEND_ERROR;
		case 0xF2:
			return XST_INVALID_PARAM;
		case 0xF3:
			return XST_NO_FEATURE;
		default:
			return XST_RECV_ERROR;
		}
	}
	else {
		return XST_FAILURE;
	}
}
/**
 * This function clears the value at the specified port and pin from the PEK
 * @param port: port num to read from
 * @param pin: pin num to read from
 */
int pek_gpio_clear(int port, int pin) {
	int result = -1;
	char cmd[4]= {0,0,0,0};
	char portStr[4];
	char pinStr[4];
	sprintf(portStr, "%.2X ", port);
	sprintf(pinStr , "%.2X ", pin );
	strcat(cmd, "03 ");
	strcat(cmd, portStr);
	strcat(cmd, pinStr);
	strcat(cmd, "05");
	//write the cmd to the correct addr to ask the pek to do something
	//I assume this write should set the [port][pin] to 0, but idk how to do that or if this will do that

	result = qsfp_iic_write(PAGE_NUM, CMD_BUFFER_ADDR, "hex", cmd);

	pek_command_commit();

	if(result == XST_SUCCESS) {
		int status = pek_poll_for_response();
		if (status != XST_SUCCESS) {
			return XST_DEVICE_BUSY;
		}
		//after getting a response, the response val is in dest
		int responseVal = *dest;
		switch(responseVal) {
		case 0x1:
			return XST_SUCCESS;
		//case 0x2 will never happen b/c nothing is ever returned from the set function
		case 0xF1:
			return XST_SEND_ERROR;
		case 0xF2:
			return XST_INVALID_PARAM;
		case 0xF3:
			return XST_NO_FEATURE;
		default:
			return XST_RECV_ERROR;
		}
	}
	else {
		return XST_FAILURE;
	}
}

/**
 * This function toggles the value at the specified port and pin from the PEK
 * @param port: port num to read from
 * @param pin: pin num to read from
  @param dataDest: 31 byte array to potentially store data read into
 */
int pek_gpio_toggle(int port, int pin, u8 *dataDest) {
	int result = -1;
	char cmd[4]= {0,0,0,0};
	char portStr[4];
	char pinStr[4];
	sprintf(portStr, "%.2X ", port);
	sprintf(pinStr , "%.2X ", pin );
	strcat(cmd, "04 ");
	strcat(cmd, portStr);
	strcat(cmd, pinStr);
	strcat(cmd, "05");

	//write the cmd to the correct addr to ask the pek to do something
	//cmd should flip the value at [port][pin] and print what returns
	result = qsfp_iic_write(PAGE_NUM, CMD_BUFFER_ADDR, "hex", cmd);

	pek_command_commit();

	if(result == XST_SUCCESS) {
		int status = pek_poll_for_response();
		if (status != XST_SUCCESS) {
			return XST_DEVICE_BUSY;
		}
		//after getting a response, the response val is in dest
		int responseVal = *dest;
		switch(responseVal) {
		case 0x1:
			return XST_RECV_ERROR;
		case 0x2:
		{
			AddressType_iic startAddr = RETRIEVE_DATA_ADDR_START;
			AddressType_iic endAddr = RETRIEVE_DATA_ADDR_END;
			unsigned int length = (endAddr - startAddr + 1) ;
			u8 *dest = dataDest;

			init_platform();
			XIic_Config qsfp_config;
			qsfp_startup(&qsfp_config);

			qsfp_write_page(PAGE_NUM);
			qsfp_iic_read_buffer(dest, startAddr, length);
			qsfp_shutdown();
			return XST_SUCCESS;
		}
		case 0xF1:
			return XST_SEND_ERROR;
		case 0xF2:
			return XST_INVALID_PARAM;
		case 0xF3:
			return XST_NO_FEATURE;
		default:
			return XST_RECV_ERROR;
		}
	}
	else {
		return XST_FAILURE;
	}
}

int pek_optocoupler (int speed, u8 *dataDest) {

	u16 conversion =  1/(speed*(pow(10, -6)));
	u8 conv_low  = 0x00FF & conversion; // store the lower byte
	u8 conv_high = (conversion >> 8);	// shift to the lower position

	int result = -1;
	char cmd[3]= {0,0,0};

	char str_high[4], str_low[4];
	sprintf(str_high, "%.2X ", conv_high);
	sprintf(str_low,  "%.2X ", conv_low );
	strcat(cmd, "0B ");
	strcat(cmd, str_high);
	strcat(cmd, str_low );

	//write the cmd to the correct addr to ask the pek to do something
	result = qsfp_iic_write(PAGE_NUM, CMD_BUFFER_ADDR, "hex", cmd);

	pek_command_commit();

	if(result == XST_SUCCESS) {
		int status = pek_poll_for_response();
		if (status != XST_SUCCESS) {
			return XST_DEVICE_BUSY;
		}
		//after getting a response, the response val is in dest
		int responseVal = *dest;
		switch(responseVal) {
		case 0x1:
			return XST_RECV_ERROR;
		case 0x2:
		{
			AddressType_iic startAddr = RETRIEVE_DATA_ADDR_START;
			AddressType_iic endAddr = RETRIEVE_DATA_ADDR_END;
			unsigned int length = (endAddr - startAddr + 1) ;
			u8 *dest = dataDest;

			init_platform();
			XIic_Config qsfp_config;
			qsfp_startup(&qsfp_config);

			qsfp_write_page(PAGE_NUM);
			qsfp_iic_read_buffer(dest, startAddr, length);
			qsfp_shutdown();
			return XST_SUCCESS;
		}
		case 0xF1:
			return XST_SEND_ERROR;
		case 0xF2:
			return XST_INVALID_PARAM;
		case 0xF3:
			return XST_NO_FEATURE;
		default:
			return XST_RECV_ERROR;
		}
	}
	else {
		return XST_FAILURE;
	}
}
