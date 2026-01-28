/*
 * ax_mcu_if.h
 *
 *  Created on: Nov 22, 2025
 *      Author: User
 */

#ifndef INC_AX_MCU_IF_H_
#define INC_AX_MCU_IF_H_


#include "main.h"
#include "user_def.h"


void mcu_write16(uint16_t addr, uint16_t wdata16);
uint16_t mcu_read16(uint16_t addr);
void mcu_write_channel_con(uint16_t addr, pst_channel_con p_para);
void mcu_read_channel_con(uint16_t addr, pst_channel_con p_para);


#endif /* INC_AX_MCU_IF_H_ */
