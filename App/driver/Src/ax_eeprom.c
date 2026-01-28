/*
 * ax_eeprom.c
 *
 *  Created on: Nov 29, 2025
 *      Author: alex
 */

#include "ax_eeprom.h"
#include "ax_uart2.h"
#include "packet_task.h"

extern I2C_HandleTypeDef hi2c4;


void eeprom_write_nbyte(uint16_t addr, uint8_t *p_para, uint16_t len)
{
	if(HAL_I2C_Mem_Write(&hi2c4, 0xA0, addr, 2, p_para, len, 1000) != HAL_OK)
	{
		uart2_puts_16hn("eeprom write error !! : 0x", addr);
	}
	HAL_Delay(20);
}


void eeprom_read_nbyte(uint16_t addr, uint8_t *p_para, uint16_t len)
{
	if(HAL_I2C_Mem_Read(&hi2c4, 0xA1, addr, 2, p_para, len, 1000) != HAL_OK)
	{
		uart2_puts_16hn("eeprom read error !! : 0x", addr);
	}
}


void eeprom_write_channel_con(uint16_t addr, pst_channel_con p_para)
{
	uint8_t wdata[7];

	wdata[0] = p_para->trig_mode;
	wdata[1] = (uint8_t)(p_para->delay>>8);
	wdata[2] = (uint8_t)(p_para->delay & 0xff);
	wdata[3] = (uint8_t)(p_para->on>>8);
	wdata[4] = (uint8_t)(p_para->on & 0xff);
	wdata[5] = (uint8_t)(p_para->block>>8);
	wdata[6] = (uint8_t)(p_para->block & 0xff);

	if(HAL_I2C_Mem_Write(&hi2c4, 0xA0, addr, 2, wdata, 7, 1000) != HAL_OK)
	{
		uart2_puts_16hn("eeprom write error !! : 0x", addr);
	}
//	else
//	{
//		uart3_puts("eeprom write ok~ \r\n");
//	}
	HAL_Delay(20);	// important !!
}

void eeprom_read_channel_con(uint16_t addr, pst_channel_con p_para)
{
	uint8_t rdata[7];

	if(HAL_I2C_Mem_Read(&hi2c4, 0xA1, addr, 2, rdata, 7, 1000) != HAL_OK)
	{
		uart2_puts_16hn("eeprom read error !! : 0x", addr);
	}
	else
	{
//		uart2_puts("eeprom read ok~ \r\n");
		p_para->trig_mode = rdata[0];
		p_para->delay = (uint16_t)((rdata[1]<<8)|rdata[2]);
		p_para->on    = (uint16_t)((rdata[3]<<8)|rdata[4]);
		p_para->block = (uint16_t)((rdata[5]<<8)|rdata[6]);
	}
}


void eepram_test_1byte(void)
{
	uint8_t  data_w[10], data_r[10];

	data_w[0] = 0x33;
	eeprom_write_nbyte(0x0000, data_w, 1);

	eeprom_read_nbyte(0x0000, data_r, 1);

	uart2_puts_8hn("eeprom read : ", data_r[0]);
}


// page : 32 bytes
void eepram_test_page(void)
{
	uint16_t addr_w, addr_r;
	uint8_t  data_w[40], data_r[40];
	int i, error_num=0;

	// 1-page write
	for(i=0; i<32; i++)
		data_w[i] = i*3;
	addr_w = 0x0000;

	if(HAL_I2C_Mem_Write(&hi2c4, 0xA0, addr_w, 2, data_w, 32, 1000) != HAL_OK)
	{
		uart2_puts("eeprom write error !! \r\n");
	}
	else
	{
		uart2_puts("eeprom write ok~ \r\n");
	}
	HAL_Delay(10);	// important !!

	// 1-page read
	addr_r = 0x0000;
	for(i=0; i<32; i++)
		data_r[i] = 0;
	if(HAL_I2C_Mem_Read(&hi2c4, 0xA1, addr_r, 2, data_r, 32, 1000) != HAL_OK)
	{
		uart2_puts("eeprom read error !! \r\n");
	}
	else
	{
		uart2_puts("eeprom read ok~ \r\n");
		for(i=0; i<32; i++)
		{
			uart2_puts_8d("i : ", i );
			uart2_puts_8hn(",  ", data_r[i]);
		}
	}

	for(i=0; i<32; i++)
	{
		if(data_w[i] != data_r[i])	error_num++;
	}
	uart2_puts_8dn("\r\n errors : ", error_num);
}
