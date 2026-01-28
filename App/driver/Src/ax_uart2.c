/*
 * ax_uart2.c
 *
 *  Created on: Sep 26, 2025
 *      Author: alex
 */

#include "user_def.h"
#include "ax_uart2.h"


extern UART_HandleTypeDef huart2;


void uart2_putc(char ch)
{

#ifdef	UART2_DEBUG_ON
	HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, 30000);
#endif

}

void uart2_puts(char *str)
{
	while(*str)	uart2_putc(*str++);
}

char uart2_i2h(uint8_t dat8)
{
	char ret8;

	if( (dat8 >= 0) && (dat8 < 10))			ret8 = '0' + dat8;
	else if( (dat8 >= 10) && (dat8 < 16))	ret8 = 'A' + dat8 - 10;
	else									ret8 = '0';

	return (ret8);
}

char uart2_i2d(uint8_t dat8)
{
	char ret8;

	if( (dat8 >= 0) && (dat8 < 10))			ret8 = '0' + dat8;
	else									ret8 = '0';

	return (ret8);
}


void uart2_puts_8h(char *str, uint8_t dat8)
{
	uart2_puts(str);
	uart2_putc( uart2_i2h(dat8>>4) );
	uart2_putc( uart2_i2h(dat8&0xF) );
}

void uart2_puts_16h(char *str, uint16_t dat16)
{
	uart2_puts(str);
	uart2_putc( uart2_i2h((char)((dat16>>12) & 0xf) ));
	uart2_putc( uart2_i2h((char)((dat16>>8) & 0xf) ));
	uart2_putc( uart2_i2h((char)((dat16>>4) & 0xf) ));
	uart2_putc( uart2_i2h((char)(dat16 & 0xF)) );
}


void uart2_puts_8d(char *str, uint8_t dat8)
{
	uint8_t flag = 0;

	uart2_puts(str);
	if( (dat8/100) > 0)
	{
		uart2_putc( uart2_i2d(dat8/100) );
		flag = 1;
	}

	if( (flag==1) || (((dat8%100)/10) > 0) )
	{
		uart2_putc( uart2_i2d((dat8%100)/10) );
	}

	uart2_putc( uart2_i2d(dat8%10) );
}

void uart2_puts_16d(char *str, uint16_t dat16)
{
	uint8_t flag = 0;

	uart2_puts(str);
	if( (dat16/10000) > 0)
	{
		uart2_putc( uart2_i2d((uint8_t)(dat16/10000)) );
		flag = 1;
	}

	if( (flag==1) || (((dat16%10000)/1000) > 0) )
	{
		uart2_putc( uart2_i2d((uint8_t)((dat16%10000)/1000)) );
	}

	if( (flag==1) || (((dat16%1000)/100) > 0) )
	{
		uart2_putc( uart2_i2d((uint8_t)((dat16%1000)/100)) );
	}

	if( (flag==1) || (((dat16%100)/10) > 0) )
	{
		uart2_putc( uart2_i2d((uint8_t)((dat16%100)/10)) );
	}

	uart2_putc( uart2_i2d((uint8_t)(dat16%10)) );
}

void uart2_putsn(char *str)
{
	uart2_puts(str);
	uart2_puts("\r\n");
}


void uart2_puts_8hn(char *str, uint8_t dat8)
{
	uart2_puts_8h(str, dat8);
	uart2_puts("\r\n");
}

void uart2_puts_16hn(char *str, uint16_t dat16)
{
	uart2_puts_16h(str, dat16);
	uart2_puts("\r\n");
}

void uart2_puts_8dn(char *str, uint8_t dat8)
{
	uart2_puts_8d(str, dat8);
	uart2_puts("\r\n");
}

void uart2_puts_16dn(char *str, uint16_t dat16)
{
	uart2_puts_16d(str, dat16);
	uart2_puts("\r\n");
}

