/*****************************************************************************
 * file: menu.c
 * description: Top-level functionality for the PETB. Based off of Xilinx IPI.
 * version: 0.03
 *
 * author: Irfan Punekar
 * company: Precision Optical Transceivers
 * date: 02 October 2018
 *
 *****************************************************************************/

#include "main.h"

/**
 * This function simply prints the prompt character to the command line
 */
void prompt() {
	print("\r\n>> ");
}

/**
 *	This function is used to prompt the user and get a command from
 *	the user input on the command line. It returns the command entered
 *	as a char*. (not working) In the case of the up or down arrow keys
 *	the function will instead go through previous commands and
 *	return whichever command is displayed when the user presses <enter>
 *	The left and right arrow keys will allow for the user to in-line
 *	edit the current command.
 */
void getUserInput(char *cmd, int rows, int cols, char cmdBuf[rows][cols], int *cmdBufLen, int *cmdIdx) {
	char *c = '\0';
	int idx = 0;
	int len = 0;
	prompt();
	while((*c = inbyte()) != ENTER) {
		//handle backspace
		if(*c == BS) {
			if(idx > 0) {
				idx--;
				len--;
				*(cmd + idx) = '\0';
				xil_printf("%c", *c);
				xil_printf(" ");
				xil_printf("%c", *c);
			}
		}
		else if(*c == 27) {
			//inbyte();
			char tmp = inbyte();
			switch(tmp) {
				//up
				//clear line and reprint what's at cmdIdx, cmdIdx must be < 49
				case 'A':
					//xil_printf("len = %d\r\n", len);
					if(*cmdIdx < 49 && strcmp(cmdBuf[*cmdIdx], "'\0'") != 0) {

						for(int i = len; i > 0; i--) {
							xil_printf("%c", BS);
							xil_printf("%c", SPACE);
							xil_printf("%c", BS);
						}
						xil_printf("%s", cmdBuf[*cmdIdx]);
						cmd = cmdBuf[*cmdIdx];
						len = strlen(cmdBuf[*cmdIdx]);
						idx = len;
						(*cmdIdx)++;
					}
					break;
				//down
				//clear line and enter what's at cmdIdx - 1, cmdIdx must be > 0
				case 'B':
					if(*cmdIdx > 0) {
						if(*cmdIdx == *cmdBufLen) {
							(*cmdIdx)--;
						}
						(*cmdIdx)--;
						for(int i = len; i > 0; i--) {
							xil_printf("%c", BS);
							xil_printf("%c", SPACE);
							xil_printf("%c", BS);
						}
						cmd = cmdBuf[*cmdIdx];
						xil_printf("%s", cmdBuf[*cmdIdx]);
						len = strlen(cmdBuf[*cmdIdx]);
						idx = len;
					}
					else if(*cmdIdx == 0) {
						for(int i = len; i > 0; i--) {
							xil_printf("%c", BS);
							xil_printf("%c", SPACE);
							xil_printf("%c", BS);
						}
						cmd = '\0';
						len = 0;
						idx = 0;
					}
					break;
				//right
				case 'C':
					//need to print everything again from idx to len
					if(idx < len) {
						idx++;
						xil_printf("%c", cmd[idx-1]);
					}
					//xil_printf("%c", "$1C");
					break;
				//left - need to reprint letters after the current idx
				case 'D':
					if(idx > 0) {
						idx--;
						xil_printf("%c", BS);
					}
					break;
			}
		}
		//if the idx != len, need to print new letter and reprint rest of the string
		else {
			if(idx != len) {
				//move everything over 1 idx to the right and then print
				//need to go from len to idx
				for(int i = len; i > idx; i--) {
					cmd[i] = cmd[i-1];
				}
				cmd[idx] = *c;
				for(int i = idx; i <= len; i++) {
					xil_printf("%c", cmd[i]);
				}
				for(int i = len; i > idx; i--) {
					xil_printf("%c", BS);
				}
			}
			else {
				*(cmd + idx) = *c;
				xil_printf("%c", *c);
			}
			idx++;
			len++;
		}
	}
	*(cmd+len) = '\0';
	strcpy(cmdBuf[*cmdBufLen], cmd);
	(*cmdBufLen)++;
	xil_printf("\r\nYou entered: %s\r\n", cmd);
}


/**
 * Main controlling thread of the program.
 */

int main() {
	init_platform();
	xil_printf("Welcome to the PIC Emulation Test Bed (PETB)");
	char cmdBuffer[50][50];
	for(int i = 0; i < 50; i++) {
		cmdBuffer[i][0] = '\0';
	}
	int bufLen = 0;
	int *cmdBufLen = &bufLen;
	int cmdIdxVal = 0;
	int word_count = 1;
	int *cmdIdx = &cmdIdxVal;
	char delim[] = " ";

	for(;;) {
		bufLen = 0;
		cmdIdxVal = 0;
		word_count = 0;
		char ** str_list;
		char str_input[50];
		for (int i = 0; i < 50; i++)
			str_input[i] = '\0';

		getUserInput(str_input, 50, 50, cmdBuffer, cmdBufLen, cmdIdx);

		int str_length = strlen(str_input);
		char *ptr = strtok(str_input, delim);

		// Strtok places a \0 in each space, and counts how many words are in the string
		// There are no commands with more than 7 keywords beyond data
		while((ptr != NULL) && (word_count < 6))
		{
			ptr = strtok(NULL, delim);
			word_count++;
		}

		// now that we know how many words we have, we know how many ptrs to words we will have
		str_list = malloc(word_count * sizeof(char*));

		// assign the ptrs at the beginning of each word to an index in command[]
		int index = 0;
		str_list[index] = &str_input[index];
		index++;

		for (int i = 1; i < str_length; i++) {
			if (str_input[i-1] == '\0') {
				str_list[index] = &str_input[i];
				index++;
			}
		}

		// make all words before word 6
		for (int i=0; i < word_count; i++)
			strupr(str_list[i]);

		if (word_count >= 1) // at least 1 keyword found
		{
			if (0 == strcmp((str_list[0]), "VCU108")) {
				*str_list[0] = VCU108 + '0';
			} else
			if (0 == strcmp((str_list[0]), "QSFP")) {
				*str_list[0] = QSFP + '0';
			} else
			if (0 == strcmp((str_list[0]), "CFP")) {
				*str_list[0] = CFP + '0';
			} else
			if (0 == strcmp((str_list[0]), "PEK")) {
				*str_list[0] = PEK + '0';
			} else
			if (0 == strcmp((str_list[0]), "QUIT")) {
				break;
			}
		}

		if (word_count >= 2) // at least 2 keywords found
		{
			if (0 == strcmp((str_list[1]), "GPIO")) {
				*str_list[1] = GPIO + '0';
			} else
			if (0 == strcmp((str_list[1]), "IIC")) {
				*str_list[1] = IIC + '0';
			} else
			if (0 == strcmp((str_list[1]), "MDIO")) {
				*str_list[1] = MDIO + '0';
			} else
			if (0 == strcmp((str_list[1]), "BERT")) {
				*str_list[1] = BERT + '0';
			} else
			if (0 == strcmp((str_list[1]), "EYESCAN")) {
				*str_list[1] = EYESCAN + '0';
			} else
			if (0 == strcmp((str_list[1]), "OPTOCOUPLER")) {
				*str_list[1] = OPTOCOUPLER + '0';
			}
		}

		if (word_count >= 3) // at least 3 keywords found
		{
			if (0 == strcmp((str_list[2]), "DEBUG")) {
				*str_list[2] = DEBUG + '0';
			} else
			if (0 == strcmp((str_list[2]), "CLEAR")) {
				*str_list[2] = CLEAR + '0';
			} else
			if (0 == strcmp((str_list[2]), "SET")) {
				*str_list[2] = SET + '0';
			} else
			if (0 == strcmp((str_list[2]), "TOGGLE")) {
				*str_list[2] = TOGGLE + '0';
			} else
			if (0 == strcmp((str_list[2]), "READ")) {
				*str_list[2] = READ + '0';
			} else
			if (0 == strcmp((str_list[2]), "WRITE")) {
				*str_list[2] = WRITE + '0';
			}
		}

		call_command(str_list);
		free(str_list);
	}

	xil_printf("\r\nThank you for using the PIC Emulation Test Bed. Goodbye!\r\n");
	cleanup_platform();
	return 0;
}
