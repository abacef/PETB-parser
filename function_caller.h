/*
 * function_caller.h
 *
 *  Created on: Aug 8, 2019
 *  Author: sxd9336
 */

#ifndef SRC_FUNCTION_CALLER_H_
#define SRC_FUNCTION_CALLER_H_

#include <ctype.h>
#include "xil_printf.h"

#include "cfp_mdio.h"
#include "command_constants.h"
#include "pek.h"
#include "qsfp_gpio.h"
#include "qsfp_iic.h"
#include "vcu108.h"
#include "command_caller.h"

void call_vcu108_gpio_debug (void);
void call_vcu108_gpio_clear (char *str_pin);
void call_vcu108_gpio_set (char *str_pin);
void call_vcu108_gpio_toggle (char *str_pin);
void call_vcu108_gpio_read (char *str_port, char *str_pin);

void call_qsfp_gpio_debug (void);
void call_qsfp_gpio_clear (char *str_port);
void call_qsfp_gpio_set (char *str_port);
void call_qsfp_gpio_toggle (char *str_port);
void call_qsfp_gpio_read (char *str_port);
void call_qsfp_iic_debug(void);
void call_qsfp_iic_read (char *str_page, char *str_startAddr, char *str_endAddr);
void call_qsfp_bert (void);
void call_qsfp_eyescan (void);

void call_cfp_gpio_debug (void);
void call_cfp_gpio_clear (char *str_port);
void call_cfp_gpio_set (char *str_port);
void call_cfp_gpio_toggle (char *str_port);
void call_cfp_gpio_read (char *str_port);
void call_cfp_mdio_read (char *str_addr);
void call_cfp_mdio_write (char *str_addr, char *str_write_data);

void call_pek_gpio_debug (void);
void call_pek_iic_debug (void);
void call_pek_gpio_clear (char *str_port, char *str_pin);
void call_pek_gpio_set (char *str_port, char *str_pin);
void call_pek_gpio_toggle (char *str_port, char *str_pin, u8 *data);
void call_pek_gpio_read (char *str_port, char *str_pin, u8 *data);
void call_pek_bert (void);
void call_pek_eyescan (void);
void call_pek_iic_read (char *str_page, char *str_startAddr, char *str_endAddr);
void call_pek_iic_write (char *str_page, char *str_startAddr, char *str_dataType, char *str_dataList);
void call_pek_optocoupler (char *str_speed, u8 *dataDest);


#endif /* SRC_FUNCTION_CALLER_H_ */
