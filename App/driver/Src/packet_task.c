/*
 * packet_task.c
 *
 *  Created on: Sep 20, 2025
 *      Author: alex
 */

#include <stdio.h>
#include <string.h>
#include "user_def.h"
#include "packet_task.h"
#include "ax_uart2.h"
#include "ax_eeprom.h"
#include "reg_addr.h"
#include "ax_mcu_if.h"
#include "ax_eeprom_task.h"

st_trig_con     g_trig_con;		// trig mode
st_seq_con      g_seq_con;		// sequence mode
st_rcv_packet 	g_rcv_packet;
uint8_t         g_interlock[USER_MAX];
uint8_t         g_user_default;
uint8_t         g_oper_mode ;
uint8_t			g_sys_rst_flag;
uint8_t			g_factory_rst_flag;


int packet_ontime(void);
int packet_task(void);
int pk_c2i(uint8_t ch);
int pk_str2int(uint8_t *buf, int len);
void pk_int2str(uint8_t *buf, int num, int len);
int packet_ontime(void);
int packet_delay(void);
int packet_block(void);
int packet_trig_mode(void);
int packet_user_save(void);
int packet_user_load(void);
int packet_user_default(void);
int packet_sequence(void);
int packet_seq_repeat(void);
int packet_seq_start(void);
int packet_seq_end(void);
int packet_inter_lock(void);
int packet_network_ip(void);
int packet_version(void);
int packet_sys_rst(void);
int packet_factory_rst(void);
int packet_sw_trigger(void);
int packet_operate_mode(void);
int packet_sw_sequence(void);


void packet_init(void)
{
	memset(&g_rcv_packet, 0, sizeof(g_rcv_packet));
}

// rval
//    0 : ok
//    1 : command buffer overflow error
//	  2 : data buffer overflow error
//	  3 : unknown command error
//	  4 : channel number error
//	  5 : data length error

int packet_rcv(uint8_t rdata)
{
	int rval = 0;

//	uart2_putc(rdata);

	switch(g_rcv_packet.state)
	{
	case PK_START :
		if(rdata == PK_STX)
		{
			g_rcv_packet.cmd_index = 0;
			memset(&g_rcv_packet.cmd_buf, 0, sizeof(g_rcv_packet.cmd_buf));
			g_rcv_packet.state = PK_CMD ;
		}
		break;

	case PK_CMD :
		if( (rdata==PK_CTX) || (rdata==PK_RTX) )
		{
			g_rcv_packet.data_index = 0;
			memset(&g_rcv_packet.data_buf, 0, sizeof(g_rcv_packet.data_buf));
			g_rcv_packet.state = PK_DATA ;
			if(rdata==PK_CTX)	g_rcv_packet.type = PK_SET ;
			else				g_rcv_packet.type = PK_GET ;
		}
		else
		{
			g_rcv_packet.cmd_buf[g_rcv_packet.cmd_index++] = rdata;
			if(g_rcv_packet.cmd_index >= PK_CMD_MAX)
			{
				packet_init();
				return (1);
			}
		}
		break;

	case PK_DATA :
		if(rdata == PK_ETX)
		{
			g_rcv_packet.state = PK_END ;
			rval = packet_task();
			packet_init();
		}
		else
		{
			g_rcv_packet.data_buf[g_rcv_packet.data_index++] = rdata;
			if(g_rcv_packet.data_index >= PK_DATA_MAX)
			{
				packet_init();
				return (2);
			}
		}
		break;

	default:
		break;
	}

	return (rval);
}


int packet_task(void)
{
	int rval = 0;

	if(strcmp((const char*)g_rcv_packet.cmd_buf, "ONT")==0)
	{
		rval = packet_ontime();
		uart2_puts(" packet_ontime .. \r\n");
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "DLT")==0)
	{
		rval = packet_delay();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "BLT")==0)
	{
		rval = packet_block();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "TGT")==0)
	{
		rval = packet_trig_mode();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SAVE")==0)
	{
		rval = packet_user_save();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "LOAD")==0)
	{
		rval = packet_user_load();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "INIT")==0)
	{
		rval = packet_user_default();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "INTL")==0)
	{
		rval = packet_inter_lock();
	}


	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SEQ")==0)
	{
		rval = packet_sequence();
	}

	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SPR")==0)
	{
		rval = packet_seq_repeat();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SEQ_SRT")==0)
	{
		rval = packet_seq_start();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SEQ_END")==0)
	{
		rval = packet_seq_end();
	}


	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "IP")==0)
	{
		rval = packet_network_ip();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "REV")==0)
	{
		rval = packet_version();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SRST")==0)
	{
		rval = packet_sys_rst();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "FRST")==0)
	{
		rval = packet_factory_rst();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SWT")==0)
	{
		rval = packet_sw_trigger();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "OPM")==0)
	{
		rval = packet_operate_mode();
	}
	else if(strcmp((const char*)g_rcv_packet.cmd_buf, "SWS")==0)
	{
		rval = packet_sw_sequence();
	}
	else
	{
		rval = 3;
	}

	return (rval);
}

int pk_c2i(uint8_t ch)
{
	int rval = 0;

	if( (ch >= '0') && (ch <= '9') )
		rval = ch - '0';

	return (rval);
}


int pk_str2int(uint8_t *buf, int len)
{
	int rval = 0;
	int i;

	for(i=0; i<len; i++)
	{
		rval *= 10;
		rval += pk_c2i(buf[i]);
	}

	return (rval);
}

void pk_int2str(uint8_t *buf, int num, int len)
{
	switch(len)
	{
	case 5 :
		buf[0] = num/10000 + '0';
		buf[1] = (num%10000)/1000 + '0';
		buf[2] = (num%1000)/100 + '0';
		buf[3] = (num%100)/10 + '0';
		buf[4] = num%10 + '0';
		break;
	case 4 :
		buf[0] = num/1000 + '0';
		buf[1] = (num%1000)/100 + '0';
		buf[2] = (num%100)/10 + '0';
		buf[3] = num%10 + '0';
		break;
	case 3 :
		buf[0] = num/100 + '0';
		buf[1] = (num%100)/10 + '0';
		buf[2] = num%10 + '0';
		break;
	case 2 :
		buf[0] = num/10 + '0';
		buf[1] = num%10 + '0';
		break;
	case 1 :
		buf[0] = num%10 + '0';
		break;
	default :
		break;
	}
}


int packet_ontime(void)
{
	int data_len, i;
	int rval = 0;
	int ch_num;
	uint8_t tx_buf[PK_SEND_MAX];

	data_len = strlen((const char *)g_rcv_packet.data_buf);
	if(g_rcv_packet.type == PK_SET)
	{
		ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
		if( (ch_num >= 1) && (ch_num <= 16) )
			ch_num -= 1;
		else	return (4);

		switch(data_len)
		{
		case 6 :
			for(i=0; i<1; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 11 :
			for(i=0; i<2; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 16 :
			for(i=0; i<3; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 21 :
			for(i=0; i<4; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 26 :
			for(i=0; i<5; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 31 :
			for(i=0; i<6; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 36 :
			for(i=0; i<7; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 41 :
			for(i=0; i<8; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 46 :
			for(i=0; i<9; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 51 :
			for(i=0; i<10; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 56 :
			for(i=0; i<11; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 61 :
			for(i=0; i<12; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 66 :
			for(i=0; i<13; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 71 :
			for(i=0; i<14; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 76 :
			for(i=0; i<15; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 81 :
			for(i=0; i<16; i++)
				g_trig_con.ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		default :
			return (5);
		}

		// write fpga reg
		for(i=0; i<16; i++)
		{
			mcu_write16(rON_TIME00_PX + i*4, g_trig_con.ch_con[i].on);
		}
		uart2_putsn("fpga write ok, trig_con.ch_con.on");
		for(i=0; i<16; i++)		uart2_puts_16dn("on : ", mcu_read16(rON_TIME00_PX + i*4));

		// for debug.
		for(i=0; i<16; i++)
		{
			uart2_puts_16d(" i : ", i+1);
			uart2_puts_16dn(",   ", g_trig_con.ch_con[i].on);
		}

	}

	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		if(data_len==0)
		{
			tx_buf[0] = PK_STX;
			for(i=0; i<16; i++)
			{
				pk_int2str(&tx_buf[1+5*i], g_trig_con.ch_con[i].on, 4);
				if(i==15)	tx_buf[5+5*i] = PK_ETX;
				else		tx_buf[5+5*i] = PK_COMMA;
			}
			printf((char *)tx_buf);
		}
		else
		{
			ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			tx_buf[0] = PK_STX;
			pk_int2str(&tx_buf[1], g_trig_con.ch_con[ch_num].on, 4);
			tx_buf[5] = PK_ETX;
			printf((char *)tx_buf);
		}
	}

	return (rval);
}

int packet_delay(void)
{
	int data_len, i;
	int rval = 0;
	int ch_num;
	uint8_t tx_buf[PK_SEND_MAX];

	data_len = strlen((const char *)g_rcv_packet.data_buf);
	if(g_rcv_packet.type == PK_SET)
	{
		ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
		if( (ch_num >= 1) && (ch_num <= 16) )
			ch_num -= 1;
		else	return (4);

		switch(data_len)
		{
		case 6 :
			for(i=0; i<1; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 11 :
			for(i=0; i<2; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 16 :
			for(i=0; i<3; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 21 :
			for(i=0; i<4; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 26 :
			for(i=0; i<5; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 31 :
			for(i=0; i<6; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 36 :
			for(i=0; i<7; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 41 :
			for(i=0; i<8; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 46 :
			for(i=0; i<9; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 51 :
			for(i=0; i<10; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 56 :
			for(i=0; i<11; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 61 :
			for(i=0; i<12; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 66 :
			for(i=0; i<13; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 71 :
			for(i=0; i<14; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 76 :
			for(i=0; i<15; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 81 :
			for(i=0; i<16; i++)
				g_trig_con.ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		default :
			return (5);
		}

		// write fpga reg
		for(i=0; i<16; i++)
		{
//			mcu_write_channel_con(rTRIG_SEL00_PX + i*4, &g_trig_con.ch_con[i]);
			mcu_write16(rDELAY_TIME00_PX + i*4, g_trig_con.ch_con[i].delay);
		}
		uart2_putsn("fpga write ok : trig_con.ch_con.delay");
		for(i=0; i<16; i++)		uart2_puts_16dn("delay : ", mcu_read16(rON_TIME00_PX + i*4));

		// for debug.
		for(i=0; i<16; i++)
		{
			uart2_puts_16d(" i : ", i+1);
			uart2_puts_16dn(",   ", g_trig_con.ch_con[i].delay);
		}

	}

	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		if(data_len==0)
		{
			tx_buf[0] = PK_STX;
			for(i=0; i<16; i++)
			{
				pk_int2str(&tx_buf[1+5*i], g_trig_con.ch_con[i].delay, 4);
				if(i==15)	tx_buf[5+5*i] = PK_ETX;
				else		tx_buf[5+5*i] = PK_COMMA;
			}
			// printf(tx_buf);
		}
		else
		{
			ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			tx_buf[0] = PK_STX;
			pk_int2str(&tx_buf[1], g_trig_con.ch_con[ch_num].delay, 4);
			tx_buf[5] = PK_ETX;
			// printf(tx_buf);
		}
	}

	return (rval);
}

int packet_block(void)
{
	int data_len, i;
	int rval = 0;
	int ch_num;
	uint8_t tx_buf[PK_SEND_MAX];

	data_len = strlen((const char *)g_rcv_packet.data_buf);
	if(g_rcv_packet.type == PK_SET)
	{
		ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
		if( (ch_num >= 1) && (ch_num <= 16) )
			ch_num -= 1;
		else	return (4);

		switch(data_len)
		{
		case 6 :
			for(i=0; i<1; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 11 :
			for(i=0; i<2; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 16 :
			for(i=0; i<3; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 21 :
			for(i=0; i<4; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 26 :
			for(i=0; i<5; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 31 :
			for(i=0; i<6; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 36 :
			for(i=0; i<7; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 41 :
			for(i=0; i<8; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 46 :
			for(i=0; i<9; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 51 :
			for(i=0; i<10; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 56 :
			for(i=0; i<11; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 61 :
			for(i=0; i<12; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 66 :
			for(i=0; i<13; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 71 :
			for(i=0; i<14; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 76 :
			for(i=0; i<15; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 81 :
			for(i=0; i<16; i++)
				g_trig_con.ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		default :
			return (5);
		}

		// write fpga reg
		for(i=0; i<16; i++)
		{
//			mcu_write_channel_con(rTRIG_SEL00_PX + i*4, &g_trig_con.ch_con[i]);
			mcu_write16(rBLOCK_TIME00_PX + i*4, g_trig_con.ch_con[i].block);
		}
		uart2_putsn("fpga write ok, trig_con.ch_con.block");
		for(i=0; i<16; i++)		uart2_puts_16dn("block : ", mcu_read16(rBLOCK_TIME00_PX + i*4));

		// for debug.
		for(i=0; i<16; i++)
		{
			uart2_puts_16d(" i : ", i+1);
			uart2_puts_16dn(",   ", g_trig_con.ch_con[i].block);
		}

	}

	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		if(data_len==0)
		{
			tx_buf[0] = PK_STX;
			for(i=0; i<16; i++)
			{
				pk_int2str(&tx_buf[1+5*i], g_trig_con.ch_con[i].block, 4);
				if(i==15)	tx_buf[5+5*i] = PK_ETX;
				else		tx_buf[5+5*i] = PK_COMMA;
			}
			// printf(tx_buf);
		}
		else
		{
			ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			tx_buf[0] = PK_STX;
			pk_int2str(&tx_buf[1], g_trig_con.ch_con[ch_num].block, 4);
			tx_buf[5] = PK_ETX;
			// printf(tx_buf);
		}
	}

	return (rval);
}

int packet_trig_mode(void)
{
	int data_len, i;
	int rval = 0;
	int ch_num;
	uint8_t tx_buf[PK_SEND_MAX];

	data_len = strlen((const char *)g_rcv_packet.data_buf);
	if(g_rcv_packet.type == PK_SET)
	{
		ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
		if( (ch_num >= 1) && (ch_num <= 16) )
			ch_num -= 1;
		else	return (4);

		switch(data_len)
		{
		case 6 :
			for(i=0; i<1; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 11 :
			for(i=0; i<2; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 16 :
			for(i=0; i<3; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 21 :
			for(i=0; i<4; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 26 :
			for(i=0; i<5; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 31 :
			for(i=0; i<6; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 36 :
			for(i=0; i<7; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 41 :
			for(i=0; i<8; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 46 :
			for(i=0; i<9; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 51 :
			for(i=0; i<10; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 56 :
			for(i=0; i<11; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 61 :
			for(i=0; i<12; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 66 :
			for(i=0; i<13; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 71 :
			for(i=0; i<14; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 76 :
			for(i=0; i<15; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		case 81 :
			for(i=0; i<16; i++)
				g_trig_con.ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[2+5*i], 4);
			break;
		default :
			return (5);
		}

		// write fpga reg
		for(i=0; i<16; i++)
		{
//			mcu_write_channel_con(rTRIG_SEL00_PX + i*4, &g_trig_con.ch_con[i]);
			mcu_write16(rTRIG_SEL00_PX + i*4, g_trig_con.ch_con[i].trig_mode);
		}

		uart2_putsn("fpga write ok, trig_con.ch_con.trig_mode");
		for(i=0; i<16; i++)		uart2_puts_16dn("trig_mode : ", mcu_read16(rTRIG_SEL00_PX + i*4));


		// for debug.
		for(i=0; i<16; i++)
		{
			uart2_puts_16d(" i : ", i+1);
			uart2_puts_16dn(",   ", g_trig_con.ch_con[i].trig_mode);
		}
	}

	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		if(data_len==0)
		{
			tx_buf[0] = PK_STX;
			for(i=0; i<16; i++)
			{
				pk_int2str(&tx_buf[1+5*i], g_trig_con.ch_con[i].trig_mode, 5);
				if(i==15)	tx_buf[5+5*i] = PK_ETX;
				else		tx_buf[5+5*i] = PK_COMMA;
			}
			// printf(tx_buf);
		}
		else
		{
			ch_num = pk_str2int(&g_rcv_packet.data_buf[0], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			tx_buf[0] = PK_STX;
			pk_int2str(&tx_buf[1], g_trig_con.ch_con[ch_num].trig_mode, 5);
			tx_buf[5] = PK_ETX;
			// printf(tx_buf);
		}
	}

	return (rval);
}

int packet_user_save(void)
{
	int rval = 0;
	uint16_t user_num;

	user_num = (uint16_t)pk_str2int(&g_rcv_packet.data_buf[0], 1);
	eeprom_save_user(user_num);
	uart2_puts_8dn("user_save ok : ", (uint8_t)user_num);

	return (rval);
}


void load_user_param(uint16_t user_num)
{
	//uint16_t i;

	eeprom_load_user(user_num);
	uart2_putsn("user_load ok, eeprom read data ..");
}


int packet_user_load(void)
{
	int rval = 0;
	uint16_t user_num;

	user_num = (uint16_t)pk_str2int(&(g_rcv_packet.data_buf[0]), 1);
	load_user_param(user_num);

	return (rval);
}

int packet_user_default(void)
{
	int rval = 0;
	uint8_t tx_buf[PK_SEND_MAX];

	if(g_rcv_packet.type == PK_SET)
	{
		g_user_default = (uint8_t)pk_str2int(&(g_rcv_packet.data_buf[0]), 1);
		eeprom_save_sys();
		uart2_puts_8dn("user_default : ", g_user_default);
		load_user_param((uint16_t)g_user_default);
	}
	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		tx_buf[0] = PK_STX;
		tx_buf[1] = 'I';
		tx_buf[2] = 'N';
		tx_buf[3] = 'I';
		tx_buf[4] = 'T';
		pk_int2str(&tx_buf[5], g_user_default, 1);;
		tx_buf[6] = PK_ETX;
		printf((char *)tx_buf);
	}

	return (rval);
}


int packet_inter_lock(void)
{
	int rval = 0;
	uint8_t tx_buf[PK_SEND_MAX];

	if(g_rcv_packet.type == PK_SET)
	{
		g_trig_con.interlock  = pk_str2int(&(g_rcv_packet.data_buf[0]), 1);
		uart2_puts_8dn("interlock : ", g_trig_con.interlock);

		// fpga write
		mcu_write16(rINTER_LOCK, (uint16_t)g_trig_con.interlock);
		uart2_puts_16dn("read interlock : ", mcu_read16(rINTER_LOCK));
	}
	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		tx_buf[0] = PK_STX;
		tx_buf[1] = 'I';
		tx_buf[2] = 'N';
		tx_buf[3] = 'T';
		tx_buf[4] = 'L';
		pk_int2str(&tx_buf[5], g_trig_con.interlock, 1);;
		tx_buf[6] = PK_ETX;
		printf((char *)tx_buf);
	}

	return (rval);
}


int packet_seq_repeat(void)
{
	int rval = 0;
	int i;
	uint8_t tx_buf[PK_SEND_MAX];

	if(g_rcv_packet.type == PK_SET)
	{
		for(i=0; i<8; i++)
		{
			g_seq_con.repeat_page[i] = (uint8_t)pk_str2int(&(g_rcv_packet.data_buf[i]), 1);
		}
		//eeprom_write_nbyte(0x7810, &g_seq_con.repeat_page[0], 8);
		eeprom_save_sys();
		uart2_puts("page repeat ok..\r\n");

		// fpga write
//		for(i=0; i<8; i++)
//		{
//			mcu_write16(rPAGE_REPEAT0+i, (uint16_t)g_seq_con.repeat_page[i]);
//		}

	}
	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		tx_buf[0] = PK_STX;
		pk_int2str(&tx_buf[1], g_seq_con.repeat_page[0], 1);
		pk_int2str(&tx_buf[2], g_seq_con.repeat_page[1], 1);
		pk_int2str(&tx_buf[3], g_seq_con.repeat_page[2], 1);
		pk_int2str(&tx_buf[4], g_seq_con.repeat_page[3], 1);
		pk_int2str(&tx_buf[5], g_seq_con.repeat_page[4], 1);
		pk_int2str(&tx_buf[6], g_seq_con.repeat_page[5], 1);
		pk_int2str(&tx_buf[7], g_seq_con.repeat_page[6], 1);
		pk_int2str(&tx_buf[8], g_seq_con.repeat_page[7], 1);
		tx_buf[9] = PK_ETX;
		printf((char *)tx_buf);
	}

	return (rval);
}


int packet_seq_start(void)
{
	int rval = 0;
	uint8_t tx_buf[PK_SEND_MAX];

	if(g_rcv_packet.type == PK_SET)
	{
		g_seq_con.start_page  = pk_str2int(&(g_rcv_packet.data_buf[0]), 1);
		uart2_puts_8dn("start_page : ", g_seq_con.start_page);

		// fpga write
		//mcu_write16(rPAGE_START, (uint16_t)g_seq_con.start_page);
		eeprom_save_sys();
	}
	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		tx_buf[0] = PK_STX;
		pk_int2str(&tx_buf[1], g_seq_con.start_page, 1);
		tx_buf[2] = PK_ETX;
		printf((char *)tx_buf);
	}

	return (rval);
}

int packet_seq_end(void)
{
	int rval = 0;
	uint8_t tx_buf[PK_SEND_MAX];

	if(g_rcv_packet.type == PK_SET)
	{
		g_seq_con.end_page  = pk_str2int(&(g_rcv_packet.data_buf[0]), 1);
		uart2_puts_8dn("end_page : ", g_seq_con.end_page);

		// fpga write
		//mcu_write16(rPAGE_END, (uint16_t)g_seq_con.end_page);
		eeprom_save_sys();
	}
	else
	{
		memset(tx_buf, 0, sizeof(tx_buf));
		tx_buf[0] = PK_STX;
		pk_int2str(&tx_buf[1], g_seq_con.end_page, 1);
		tx_buf[2] = PK_ETX;
		printf((char *)tx_buf);
	}

	return (rval);
}

int packet_network_ip(void)
{
	int rval = 0;

	return (rval);
}


int packet_version(void)
{
	int rval = 0;

	printf(ax_VERSION);

	return (rval);
}

int packet_sys_rst(void)
{
	int rval = 0;

	g_sys_rst_flag  = 1;
	uart2_puts("system reset .. \r\n");

	return (rval);
}

int packet_factory_rst(void)
{
	int rval = 0;

	g_factory_rst_flag  = 1;
	uart2_puts("factory reset .. \r\n");

	return (rval);
}

int packet_sw_trigger(void)
{
	int rval = 0;
	uint16_t sw_trig = 0;
	int i;

	for(i=0; i<16; i++)
	{
		sw_trig = sw_trig<<1;
		if(pk_str2int(&g_rcv_packet.data_buf[15-i], 1)==1)
			sw_trig |= 0x01;
	}
	mcu_write16(rSW_TRIG, sw_trig);
	uart2_puts_16hn("sw_trigger : 0x", sw_trig);

//	mcu_write16(rSTROBE_EN, 0xffff);
//	uart2_putsn("strobe_enable..");

	return (rval);
}

int packet_operate_mode(void)
{
	int rval = 0;

	g_oper_mode = (uint16_t) pk_str2int(&g_rcv_packet.data_buf[0], 1);

	uart2_puts_16dn("oper_mode : ", g_oper_mode);
	mcu_write16(rOPER_MODE, g_oper_mode);
	eeprom_save_sys();

	return (rval);
}


int packet_sw_sequence(void)
{
	int rval = 0;

	mcu_write16(rSW_SEQUENCE, 1);

	uart2_puts_16dn("sw_sequence,  cur : ", mcu_read16(rPAGE_SEL));

/*
	uart2_putsn("sw_sequence");
	uart2_puts_16dn("    cur_page : ", mcu_read16(rPAGE_SEL));
	uart2_puts_16dn("    start_page : ", mcu_read16(rPAGE_START));
	uart2_puts_16dn("    end_page : ", mcu_read16(rPAGE_END));
	uart2_puts_16dn("    repeat0 : ", mcu_read16(rPAGE_REPEAT0));
	uart2_puts_16dn("    repeat1 : ", mcu_read16(rPAGE_REPEAT1));
	uart2_puts_16dn("    repeat2 : ", mcu_read16(rPAGE_REPEAT2));
	uart2_puts_16dn("    repeat3 : ", mcu_read16(rPAGE_REPEAT3));
	uart2_puts_16dn("    repeat4 : ", mcu_read16(rPAGE_REPEAT4));
	uart2_puts_16dn("    repeat5 : ", mcu_read16(rPAGE_REPEAT5));
	uart2_puts_16dn("    repeat6 : ", mcu_read16(rPAGE_REPEAT6));
	uart2_puts_16dn("    repeat7 : ", mcu_read16(rPAGE_REPEAT7));
*/

	return (rval);
}



int packet_sequence(void)
{
	int data_len, i;
	int rval = 0;
	int ch_num, page_num;
	uint8_t tx_buf[PK_SEND_MAX];

	if(g_rcv_packet.type == PK_SET)
	{
		page_num = pk_str2int(&g_rcv_packet.data_buf[0], 1);
		if( (page_num < 0) || (page_num > 7) )
		{
			uart2_puts("Error : page_num overflow !! \r\n");
			return (6);
		}

		if ( (g_rcv_packet.data_buf[1]=='O') && (g_rcv_packet.data_buf[2]=='N') && (g_rcv_packet.data_buf[3]=='T') )
		{
			data_len = strlen((char *)&g_rcv_packet.data_buf[4]);
			ch_num = pk_str2int(&g_rcv_packet.data_buf[4], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			switch(data_len)
			{
			case 6 :
				for(i=0; i<1; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 11 :
				for(i=0; i<2; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 16 :
				for(i=0; i<3; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 21 :
				for(i=0; i<4; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 26 :
				for(i=0; i<5; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 31 :
				for(i=0; i<6; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 36 :
				for(i=0; i<7; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 41 :
				for(i=0; i<8; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 46 :
				for(i=0; i<9; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 51 :
				for(i=0; i<10; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 56 :
				for(i=0; i<11; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 61 :
				for(i=0; i<12; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 66 :
				for(i=0; i<13; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 71 :
				for(i=0; i<14; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 76 :
				for(i=0; i<15; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 81 :
				for(i=0; i<16; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].on = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			default :
				return (5);
			}

			// write fpga reg
			for(i=0; i<16; i++)
			{
				//mcu_write_channel_con(rTRIG_SEL00_P0 + page_num*0x40, &g_seq_con.page_con[page_num].ch_con[i]);
				mcu_write16(rON_TIME00_P0 + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].on);
			}
			uart2_puts_16dn("fpga write on_time, page : ", page_num);

			// write eeprom
			for(i=0; i<16; i++)
			{
				eeprom_write_channel_con(EEP_ADDR_PAGE0 + page_num*0x80 + i*8, &g_seq_con.page_con[page_num].ch_con[i]);
			}
			uart2_puts_16dn("eeprom write page : ", page_num);
		}

		else if ( (g_rcv_packet.data_buf[1]=='D') && (g_rcv_packet.data_buf[2]=='L') && (g_rcv_packet.data_buf[3]=='T') )
		{
			data_len = strlen((char *)&g_rcv_packet.data_buf[4]);
			ch_num = pk_str2int(&g_rcv_packet.data_buf[4], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			switch(data_len)
			{
			case 6 :
				for(i=0; i<1; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 11 :
				for(i=0; i<2; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 16 :
				for(i=0; i<3; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 21 :
				for(i=0; i<4; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 26 :
				for(i=0; i<5; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 31 :
				for(i=0; i<6; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 36 :
				for(i=0; i<7; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 41 :
				for(i=0; i<8; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 46 :
				for(i=0; i<9; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 51 :
				for(i=0; i<10; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 56 :
				for(i=0; i<11; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 61 :
				for(i=0; i<12; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 66 :
				for(i=0; i<13; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 71 :
				for(i=0; i<14; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 76 :
				for(i=0; i<15; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 81 :
				for(i=0; i<16; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].delay = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			default :
				return (5);
			}

			// write fpga reg
			for(i=0; i<16; i++)
			{
				mcu_write16(rDELAY_TIME00_P0 + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].delay);
			}
			uart2_puts_16dn("fpga write delay_time, page : ", page_num);

			// write eeprom
			for(i=0; i<16; i++)
			{
				eeprom_write_channel_con(EEP_ADDR_PAGE0 + page_num*0x80 + i*8, &g_seq_con.page_con[page_num].ch_con[i]);
			}
			uart2_puts_16dn("eeprom write page : ", page_num);
		}

		else if ( (g_rcv_packet.data_buf[1]=='B') && (g_rcv_packet.data_buf[2]=='L') && (g_rcv_packet.data_buf[3]=='T') )
		{
			data_len = strlen((char *)&g_rcv_packet.data_buf[4]);
			ch_num = pk_str2int(&g_rcv_packet.data_buf[4], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			switch(data_len)
			{
			case 6 :
				for(i=0; i<1; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 11 :
				for(i=0; i<2; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 16 :
				for(i=0; i<3; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 21 :
				for(i=0; i<4; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 26 :
				for(i=0; i<5; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 31 :
				for(i=0; i<6; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 36 :
				for(i=0; i<7; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 41 :
				for(i=0; i<8; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 46 :
				for(i=0; i<9; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 51 :
				for(i=0; i<10; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 56 :
				for(i=0; i<11; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 61 :
				for(i=0; i<12; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 66 :
				for(i=0; i<13; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 71 :
				for(i=0; i<14; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 76 :
				for(i=0; i<15; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			case 81 :
				for(i=0; i<16; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].block = pk_str2int(&g_rcv_packet.data_buf[6+5*i], 4);
				break;
			default :
				return (5);
			}

			// write fpga reg
			for(i=0; i<16; i++)
			{
				mcu_write16(rBLOCK_TIME00_P0 + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].block);
			}
			uart2_puts_16dn("fpga write block_time, page : ", page_num);

			// write eeprom
			for(i=0; i<16; i++)
			{
				eeprom_write_channel_con(EEP_ADDR_PAGE0 + page_num*0x80 + i*8, &g_seq_con.page_con[page_num].ch_con[i]);
			}
			uart2_puts_16dn("eeprom write page : ", page_num);
		}

		else if ( (g_rcv_packet.data_buf[1]=='T') && (g_rcv_packet.data_buf[2]=='G') && (g_rcv_packet.data_buf[3]=='T') )
		{
			data_len = strlen((char *)&g_rcv_packet.data_buf[4]);
			ch_num = pk_str2int(&g_rcv_packet.data_buf[4], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else	return (4);

			switch(data_len)
			{
			case 6 :
				for(i=0; i<1; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 11 :
				for(i=0; i<2; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 16 :
				for(i=0; i<3; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 21 :
				for(i=0; i<4; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 26 :
				for(i=0; i<5; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 31 :
				for(i=0; i<6; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 36 :
				for(i=0; i<7; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 41 :
				for(i=0; i<8; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 46 :
				for(i=0; i<9; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 51 :
				for(i=0; i<10; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 56 :
				for(i=0; i<11; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 61 :
				for(i=0; i<12; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 66 :
				for(i=0; i<13; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 71 :
				for(i=0; i<14; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 76 :
				for(i=0; i<15; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			case 81 :
				for(i=0; i<16; i++)
					g_seq_con.page_con[page_num].ch_con[ch_num+i].trig_mode = pk_str2int(&g_rcv_packet.data_buf[9+5*i], 1);
				break;
			default :
				return (5);
			}

			// write fpga reg
			for(i=0; i<16; i++)
			{
				mcu_write16(rTRIG_SEL00_P0 + page_num*0x40 + i*4, g_seq_con.page_con[page_num].ch_con[i].trig_mode);
			}
			uart2_puts_16dn("fpga write trig_mode, page : ", page_num);

			// write eeprom
			for(i=0; i<16; i++)
			{
				eeprom_write_channel_con(EEP_ADDR_PAGE0 + page_num*0x80 + i*8, &g_seq_con.page_con[page_num].ch_con[i]);
			}
			uart2_puts_16dn("eeprom write page : ", page_num);
		}
		else
		{
			uart2_puts("Error : seq unknown command !! \r\n");
			return (7);
		}
	}

	else		// request
	{
		memset(tx_buf, 0, sizeof(tx_buf));

		data_len = strlen((char *)&g_rcv_packet.data_buf[0]);

		if(data_len==4)
		{
			page_num = pk_str2int(&g_rcv_packet.data_buf[0], 1) - 1;
			if( (page_num < 0) || (page_num > 7) )
			{
				uart2_puts("Error : page_num overflow !! \r\n");
				return (6);
			}

			tx_buf[0] = PK_STX;
			if( (g_rcv_packet.data_buf[1]=='O') && (g_rcv_packet.data_buf[2]=='N') && (g_rcv_packet.data_buf[3]=='T') )
			{
				for(i=0; i<16; i++)
				{
					pk_int2str(&tx_buf[1+5*i], g_seq_con.page_con[page_num].ch_con[i].on, 4);
					if(i==15)	tx_buf[5+5*i] = PK_ETX;
					else		tx_buf[5+5*i] = PK_COMMA;
				}
				printf((char *)tx_buf);
			}
			else if( (g_rcv_packet.data_buf[1]=='D') && (g_rcv_packet.data_buf[2]=='L') && (g_rcv_packet.data_buf[3]=='T') )
			{
				for(i=0; i<16; i++)
				{
					pk_int2str(&tx_buf[1+5*i], g_seq_con.page_con[page_num].ch_con[i].delay, 4);
					if(i==15)	tx_buf[5+5*i] = PK_ETX;
					else		tx_buf[5+5*i] = PK_COMMA;
				}
				printf((char *)tx_buf);
			}
			else if( (g_rcv_packet.data_buf[1]=='B') && (g_rcv_packet.data_buf[2]=='L') && (g_rcv_packet.data_buf[3]=='T') )
			{
				for(i=0; i<16; i++)
				{
					pk_int2str(&tx_buf[1+5*i], g_seq_con.page_con[page_num].ch_con[i].block, 4);
					if(i==15)	tx_buf[5+5*i] = PK_ETX;
					else		tx_buf[5+5*i] = PK_COMMA;
				}
				printf((char *)tx_buf);
			}
			else if( (g_rcv_packet.data_buf[1]=='T') && (g_rcv_packet.data_buf[2]=='G') && (g_rcv_packet.data_buf[3]=='T') )
			{
				for(i=0; i<16; i++)
				{
					pk_int2str(&tx_buf[1+5*i], g_seq_con.page_con[page_num].ch_con[i].trig_mode, 4);
					if(i==15)	tx_buf[5+5*i] = PK_ETX;
					else		tx_buf[5+5*i] = PK_COMMA;
				}
				printf((char *)tx_buf);
			}
			else
			{
				uart2_puts("Error : seq_req unknown command !! \r\n");
				return (8);
			}
		}
		else
		{
			page_num = pk_str2int(&g_rcv_packet.data_buf[0], 1) - 1;
			if( (page_num < 0) || (page_num > 7) )
			{
				uart2_puts("Error : page_num overflow !! \r\n");
				return (6);
			}

			ch_num = pk_str2int(&g_rcv_packet.data_buf[4], 2);
			if( (ch_num >= 1) && (ch_num <= 16) )
				ch_num -= 1;
			else
			{
				uart2_puts("Error : channel_num overflow !! \r\n");
				return (4);
			}

			tx_buf[0] = PK_STX;
			if( (g_rcv_packet.data_buf[1]=='O') && (g_rcv_packet.data_buf[2]=='N') && (g_rcv_packet.data_buf[3]=='T') )
			{
				pk_int2str(&tx_buf[1], g_seq_con.page_con[page_num].ch_con[ch_num].on, 4);
			}
			else if( (g_rcv_packet.data_buf[1]=='D') && (g_rcv_packet.data_buf[2]=='L') && (g_rcv_packet.data_buf[3]=='T') )
			{
				pk_int2str(&tx_buf[1], g_seq_con.page_con[page_num].ch_con[ch_num].delay, 4);
			}
			else if( (g_rcv_packet.data_buf[1]=='B') && (g_rcv_packet.data_buf[2]=='L') && (g_rcv_packet.data_buf[3]=='T') )
			{
				pk_int2str(&tx_buf[1], g_seq_con.page_con[page_num].ch_con[ch_num].block, 4);
			}
			else if( (g_rcv_packet.data_buf[1]=='T') && (g_rcv_packet.data_buf[2]=='G') && (g_rcv_packet.data_buf[3]=='T') )
			{
				pk_int2str(&tx_buf[1], g_seq_con.page_con[page_num].ch_con[ch_num].trig_mode, 4);
			}
			else
			{
				uart2_puts("Error : seq_req unknown command !! \r\n");
				return (8);
			}
			tx_buf[5] = PK_ETX;
			printf((char *)tx_buf);
		}
	}

	return (rval);
}



