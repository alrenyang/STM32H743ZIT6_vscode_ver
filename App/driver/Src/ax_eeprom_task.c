/*
 * ax_eeprom_task.c
 *
 *  Created on: Dec 8, 2025
 *      Author: alex
 */

#include "ax_eeprom_task.h"
#include "ax_eeprom.h"
#include "ax_uart2.h"
#include "reg_addr.h"
#include "ax_mcu_if.h"
#include "packet_task.h"


void eeprom_test1(void)
{
	g_user_default = 0;
	g_seq_con.start_page = 0;
	g_seq_con.end_page = 7;
	g_oper_mode = 0;

	g_seq_con.repeat_page[0] = 1;
	g_seq_con.repeat_page[1] = 1;
	g_seq_con.repeat_page[2] = 1;
	g_seq_con.repeat_page[3] = 1;
	g_seq_con.repeat_page[4] = 1;
	g_seq_con.repeat_page[5] = 1;
	g_seq_con.repeat_page[6] = 1;
	g_seq_con.repeat_page[7] = 1;


	eeprom_save_sys();
	eeprom_load_sys();

}

void eeprom_save_facotry(void)
{
	uint16_t addr;
	uint16_t i, j;

	// 1) user factory data
	for(i=0; i<16; i++)
	{
		g_trig_con.ch_con[i].delay = 10;
		g_trig_con.ch_con[i].on = 20;
		g_trig_con.ch_con[i].block = 30;
		g_trig_con.ch_con[i].trig_mode = 0;
	}

	// 2) save user data
	for(j=0; j<4; j++)
	{
		addr = j*0x80;
		for(i=0; i<16; i++)
		{
			eeprom_write_channel_con(addr+i*8, &g_trig_con.ch_con[i]);
		}
	}

	// 3) page factory data
	for(j=0; j<8; j++)
	{
		for(i=0; i<16; i++)
		{
			g_seq_con.page_con[j].ch_con[i].delay = 990;
			g_seq_con.page_con[j].ch_con[i].on = 330;
			g_seq_con.page_con[j].ch_con[i].block = 440;
			g_seq_con.page_con[j].ch_con[i].trig_mode = 0;
		}
	}

	// 4) save page data
	for(j=0; j<8; j++)
	{
		addr = EEP_ADDR_PAGE0 + j*0x80;
		for(i=0; i<16; i++)
		{
			eeprom_write_channel_con(addr + i*8, &g_seq_con.page_con[j].ch_con[i]);
		}
	}

	// 5) system factory data
	g_user_default = 0;
	g_seq_con.start_page = 0;
	g_seq_con.end_page = 7;
	g_oper_mode = 0;

	g_seq_con.repeat_page[0] = 1;
	g_seq_con.repeat_page[1] = 1;
	g_seq_con.repeat_page[2] = 1;
	g_seq_con.repeat_page[3] = 1;
	g_seq_con.repeat_page[4] = 1;
	g_seq_con.repeat_page[5] = 1;
	g_seq_con.repeat_page[6] = 1;
	g_seq_con.repeat_page[7] = 1;

	// 6) save system data
	eeprom_save_sys();
}

void eeprom_load_factory(void)
{

}


void eeprom_save_sys(void)
{
	uint8_t wdata[26];
	uint16_t i;

	wdata[8] = g_user_default;
	wdata[9] = g_seq_con.start_page;
	wdata[10] = g_seq_con.end_page;
	wdata[11] = g_oper_mode;

	wdata[16] = g_seq_con.repeat_page[0];
	wdata[17] = g_seq_con.repeat_page[1];
	wdata[18] = g_seq_con.repeat_page[2];
	wdata[19] = g_seq_con.repeat_page[3];
	wdata[20] = g_seq_con.repeat_page[4];
	wdata[21] = g_seq_con.repeat_page[5];
	wdata[22] = g_seq_con.repeat_page[6];
	wdata[23] = g_seq_con.repeat_page[7];

	eeprom_write_nbyte(EEP_ADDR_SYS, &wdata[0], 24);

	// fpga write
	mcu_write16(rPAGE_SEL, (uint16_t)g_user_default);
	mcu_write16(rPAGE_START, (uint16_t)g_seq_con.start_page);
	mcu_write16(rPAGE_END, (uint16_t)g_seq_con.end_page);
	mcu_write16(rOPER_MODE, (uint16_t)g_oper_mode);

	for(i=0; i<8; i++)
	{
		mcu_write16(rPAGE_REPEAT0+i, (uint16_t)g_seq_con.repeat_page[i]);
	}

}

void eeprom_load_sys(void)
{
	uint8_t rdata[26];
	uint16_t i;

	eeprom_read_nbyte(EEP_ADDR_SYS, &rdata[0], 24);

	g_user_default       = rdata[8] ;
	g_seq_con.start_page = rdata[9] ;
	g_seq_con.end_page   = rdata[10];
	g_oper_mode          = rdata[11];

	g_seq_con.repeat_page[0] = rdata[16];
	g_seq_con.repeat_page[1] = rdata[17];
	g_seq_con.repeat_page[2] = rdata[18];
	g_seq_con.repeat_page[3] = rdata[19];
	g_seq_con.repeat_page[4] = rdata[20];
	g_seq_con.repeat_page[5] = rdata[21];
	g_seq_con.repeat_page[6] = rdata[22];
	g_seq_con.repeat_page[7] = rdata[23];

	// fpga write
	mcu_write16(rPAGE_SEL, (uint16_t)g_user_default);
	mcu_write16(rPAGE_START, (uint16_t)g_seq_con.start_page);
	mcu_write16(rPAGE_END, (uint16_t)g_seq_con.end_page);
	mcu_write16(rOPER_MODE, (uint16_t)g_oper_mode);

	for(i=0; i<8; i++)
	{
		mcu_write16(rPAGE_REPEAT0+i, (uint16_t)g_seq_con.repeat_page[i]);
	}


#ifdef	UART2_DEBUG_L2_ON
	uart2_puts_8dn("g_user_default : ", g_user_default);
	uart2_puts_8dn("start_page : ", g_seq_con.start_page);
	uart2_puts_8dn("end_page : ", g_seq_con.end_page);
	uart2_puts_8dn("g_oper_mode : ", g_oper_mode);

	uart2_puts_8d("repeat : ", g_seq_con.repeat_page[0]);
	uart2_puts_8d("  ", g_seq_con.repeat_page[1]);
	uart2_puts_8d("  ", g_seq_con.repeat_page[2]);
	uart2_puts_8d("  ", g_seq_con.repeat_page[3]);
	uart2_puts_8d("  ", g_seq_con.repeat_page[4]);
	uart2_puts_8d("  ", g_seq_con.repeat_page[5]);
	uart2_puts_8d("  ", g_seq_con.repeat_page[6]);
	uart2_puts_8dn("  ", g_seq_con.repeat_page[7]);
#endif

}

void eeprom_save_user(uint16_t user_num)
{
	uint16_t addr;
	uint16_t i;

	addr = user_num * 0x80;
	for(i=0; i<16; i++)
	{
		eeprom_write_channel_con(addr+i*8, &g_trig_con.ch_con[i]);
	}
}

void eeprom_load_user(uint16_t user_num)
{
	uint16_t addr;
	uint16_t i;
	st_trig_con  rd_trig_con;

	addr = user_num * 0x80;
	for(i=0; i<16; i++)
	{
		eeprom_read_channel_con(addr+i*8, &g_trig_con.ch_con[i]);

#ifdef	UART2_DEBUG_L2_ON
		// for debug
		uart2_puts_16d(" user ch : ", i+1);
		uart2_puts_16d("  ", g_trig_con.ch_con[i].on);
		uart2_puts_16d("  ", g_trig_con.ch_con[i].delay);
		uart2_puts_16d("  ", g_trig_con.ch_con[i].block);
		uart2_puts_8dn("  ", g_trig_con.ch_con[i].trig_mode);
#endif
	}

	// write fpga reg
	for(i=0; i<16; i++)
	{
		mcu_write_channel_con(rTRIG_SEL00_PX + i*4, &g_trig_con.ch_con[i]);
	}
	uart2_putsn("fpga write ok");

	uart2_putsn("fpga read ..");
	for(i=0; i<16; i++)
	{
		mcu_read_channel_con(rTRIG_SEL00_PX + i*4, &rd_trig_con.ch_con[i]);
	}


#ifdef	UART2_DEBUG_L2_ON
	for(i=0; i<16; i++)
	{
		uart2_puts_16d("delay : ", rd_trig_con.ch_con[i].delay);
		uart2_puts_16d(",  on : ", rd_trig_con.ch_con[i].on);
		uart2_puts_16d(",  block : ", rd_trig_con.ch_con[i].block);
		uart2_puts_16dn(",  trig : ", rd_trig_con.ch_con[i].trig_mode);
	}
#endif


}

void eeprom_load_page(uint16_t page_num)
{
	uint16_t i;

	// eeprom load
	for(i=0; i<16; i++)
	{
		eeprom_read_channel_con(EEP_ADDR_PAGE0 + page_num*0x80 + i*8, &g_seq_con.page_con[page_num].ch_con[i]);


#ifdef	UART2_DEBUG_L2_ON
		// for debug
		uart2_puts_16d(" ch : ", i+1);
		uart2_puts_16d("  ", g_seq_con.page_con[page_num].ch_con[i].on);
		uart2_puts_16d("  ", g_seq_con.page_con[page_num].ch_con[i].delay);
		uart2_puts_16d("  ", g_seq_con.page_con[page_num].ch_con[i].block);
		uart2_puts_8dn("  ", g_seq_con.page_con[page_num].ch_con[i].trig_mode);
#endif

	}
	uart2_puts_16dn("eeprom read page : ", page_num);

	// write fpga reg
	for(i=0; i<16; i++)
	{
		mcu_write16(rON_TIME00_P0    + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].on);
		mcu_write16(rDELAY_TIME00_P0 + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].delay);
		mcu_write16(rBLOCK_TIME00_P0 + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].block);
		mcu_write16(rTRIG_SEL00_P0   + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].trig_mode);
	}
	uart2_puts_16dn("fpga write : ", page_num);
}

void eeprom_load_page_all(void)
{
	uint16_t i;

	for(i=0; i<8; i++)
		eeprom_load_page(i);
}
