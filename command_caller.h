/*****************************************************************************
 * file: command_caller.h
 * description: header file for the command_caller file
 * version: 0.01
 *
 * author: Sean Wisnewski
 * company: Precision Optical Transceivers
 * date: 18 March 2019
 *****************************************************************************/

#ifndef COMMAND_CALLER_H
#define COMMAND_CALLER_H

#include <ctype.h>
#include "xil_printf.h"
#include "cfp_mdio.h"
#include "command_constants.h"
#include "pek.h"
#include "qsfp_gpio.h"
#include "qsfp_iic.h"
#include "vcu108.h"
#include "function_caller.h"

void call_command(char **command);

#endif
