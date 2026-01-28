/*
 * ax_uart2.h
 *
 *  Created on: Sep 26, 2025
 *      Author: alex
 */

#ifndef INC_AX_UART2_H_
#define INC_AX_UART2_H_

#include "main.h"


void uart2_putc(char ch);
void uart2_puts(char *str);
char uart2_i2h(uint8_t dat8);
char uart2_i2d(uint8_t dat8);
void uart2_puts_8h(char *str, uint8_t dat8);
void uart2_puts_16h(char *str, uint16_t dat16);
void uart2_puts_8d(char *str, uint8_t dat8);
void uart2_puts_16d(char *str, uint16_t dat16);
void uart2_putsn(char *str);
void uart2_puts_8hn(char *str, uint8_t dat8);
void uart2_puts_8dn(char *str, uint8_t dat8);
void uart2_puts_16hn(char *str, uint16_t dat16);
void uart2_puts_16dn(char *str, uint16_t dat16);



#endif /* INC_AX_UART2_H_ */
