/*
 * packet_task.h
 *
 *  Created on: Sep 20, 2025
 *      Author: alex
 */

#ifndef INC_PACKET_TASK_H_
#define INC_PACKET_TASK_H_

#include "main.h"
#include "user_def.h"

void packet_init(void);
int packet_rcv(uint8_t rdata);
void load_user_param(uint16_t user_num);

extern st_trig_con      g_trig_con;		// trig mode
extern st_seq_con       g_seq_con;		// sequence mode
extern st_rcv_packet    g_rcv_packet;
extern uint8_t          g_interlock[USER_MAX];
extern uint8_t          g_user_default;
extern uint8_t			g_sys_rst_flag;
extern uint8_t			g_factory_rst_flag;
extern uint8_t          g_oper_mode ;


#endif /* INC_PACKET_TASK_H_ */
