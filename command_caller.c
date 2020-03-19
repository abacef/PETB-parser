/*****************************************************************************
 * file: command_caller.c
 *
 * author: Shreya Patel
 * date: 1 July 2019
 *
 *****************************************************************************/

#include "command_caller.h"

int status;

void call_command(char **command) {

	int object = 	*command[0]-'0';
	int specifier = *command[1]-'0';
	int function = 	*command[2]-'0';

	switch(object) {

	case VCU108: {
		switch(specifier) {
		case GPIO: {
			
			switch(function) {

			case DEBUG:
				call_vcu108_gpio_debug();
			break;

			case CLEAR:
				call_vcu108_gpio_clear(command[3]);
			break;

			case SET:
				call_vcu108_gpio_set(command[3]);
			break;

			case TOGGLE:
				call_vcu108_gpio_toggle(command[3]);
			break;

			case READ:
				call_vcu108_gpio_read(command[3], command[4]);
			break;

			default:
				xil_printf("Invalid function for GPIO. Try 'debug' , 'clear' , 'set' , 'toggle' or 'read'.\r\n");
			break;
			}

		break;
		}

		default:
			xil_printf("Invalid specifier for VCU108. Try 'gpio'.\r\n");
		break;
		}

	break;
	}

	case QSFP: {
		switch(specifier) {

		case GPIO: {
			switch(function) {

			case DEBUG:
				call_qsfp_gpio_debug();
			break;

			case CLEAR:
				call_qsfp_gpio_clear(command[3]);
			break;

			case SET:
				call_qsfp_gpio_set(command[3]);
			break;

			case TOGGLE:
				call_qsfp_gpio_toggle(command[3]);
			break;

			case READ:
				call_qsfp_gpio_read(command[3]);
			break;

			default:
				xil_printf("Invalid function for GPIO. Try 'debug' , 'clear' , 'set' , 'toggle' or 'read'.\r\n");
			break;
			}

		break;
		}

		case IIC: {
			// check if qsfp module is missing before executing
			if (qsfp_gpio_read("MODPRS") == 1) {
				print("There is no module connected in the QSFP port,\r\n");
				print("or the device is insecure. Please (re)connect \r\n");
				print("the module under test and try again.\r\n");
			} else {
			
				switch(function) {

				case DEBUG:
					call_qsfp_iic_debug();
				break;

				case READ: {
					call_qsfp_iic_read(command[3], command[4], command[5]);
				break;
				}

				default:
					xil_printf("Invalid IIC function for QSFP. Try 'debug' or 'read'.\r\n");
				break;
				}
			}
		break;
		}

		case BERT:
			call_qsfp_bert();
		break;

		case EYESCAN:
			call_qsfp_eyescan();
		break;

		default:
			xil_printf("Invalid specifier for QSFP. Try 'gpio' , 'iic' , 'bert' , or 'eyescan'.\r\n");
		break;
		}
	break;

	case CFP: {

		switch(specifier) {

		case GPIO: {

			switch(function) {

			case DEBUG:
				call_cfp_gpio_debug();
			break;

			case CLEAR:
				call_cfp_gpio_clear(command[3]);
			break;

			case SET:
				call_cfp_gpio_set(command[3]);
			break;

			case TOGGLE:
				call_cfp_gpio_toggle(command[3]);
			break;

			case READ:
				call_cfp_gpio_read(command[3]);
			break;

			default:
				xil_printf("Invalid function for GPIO. Try 'debug' , 'clear' , 'set' , 'toggle' or 'read'.\r\n");
			break;
			}

		break;
		}

		case MDIO: {

			switch(function) {

			case READ: {
				call_cfp_mdio_read(command[3]);
			break; }

			case WRITE: {
				call_cfp_mdio_write(command[3], command[4]);
			break; }

			default:
				xil_printf("Invalid function for MDIO. Try 'read' or 'write'.\r\n");
			break;
			}

		break;
		}

		default:
			xil_printf("Invalid specifier for CFP. Try 'gpio' or 'mdio'.\r\n");
		break;
		}
	break;
	}

	case PEK: {
		// check if qsfp module is missing before executing
		if (qsfp_gpio_read("MODPRS") == 1) {
			print("There is no module connected in the QSFP port,\r\n");
			print("or the device is insecure. Please (re)connect \r\n");
			print("the module under test and try again.\r\n");
		} else {
			u8 data[31];
			switch(specifier) {

			case GPIO: {

				switch(function) {

				case DEBUG:
					call_pek_gpio_debug();
				break;

				case CLEAR:
					call_pek_gpio_clear(command[3], command[4]);
				break;

				case SET:
					call_pek_gpio_set(command[3], command[4]);
				break;

				case TOGGLE:
					call_pek_gpio_toggle(command[3], command[4], data);
				break;

				case READ:
					call_pek_gpio_read(command[3], command[4], data);
				break;

				default:
					xil_printf("Invalid function for GPIO. Try 'debug' , 'clear' , 'set' , 'toggle' or 'read'.\r\n");
				break;
				}
			break;
			}

			case BERT:
				call_pek_bert();
			break;

			case EYESCAN:
				call_pek_eyescan();
			break;

			case IIC: {
				switch(function) {
				case DEBUG:
					call_pek_iic_debug();
				break;

				case READ: {
					call_pek_iic_read(command[3], command[4], command[5]);
				break;
				}

				case WRITE: {
					call_pek_iic_write(command[3], command[4], command[5], command[6]);
				break;
				}

				default:
					xil_printf("Invalid IIC function for PEK. Try 'debug' , 'read' or 'write'.\r\n");
				break;
				}
			break;
			}
			case OPTOCOUPLER:
				call_pek_optocoupler(command[2], data);
			break;

			default:
				xil_printf("Invalid specifier for PEK. Try 'gpio' , 'iic' , 'bert' or 'eyescan'.\r\n");
			break;
			}
		}
	break;
	}
	default:
		xil_printf("Invalid object name. Try 'vcu108' , 'qsfp' , 'cfp' or 'pek'.\r\n");
	break;
	}
}
}
