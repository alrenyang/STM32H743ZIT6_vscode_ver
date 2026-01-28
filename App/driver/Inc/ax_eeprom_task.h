/*
 * ax_eeprom_task.h
 *
 *  Created on: Dec 8, 2025
 *      Author: alex
 */

#ifndef INC_AX_EEPROM_TASK_H_
#define INC_AX_EEPROM_TASK_H_

#include "main.h"


void eeprom_test1(void);
void eeprom_save_facotry(void);

void eeprom_save_sys(void);
void eeprom_load_sys(void);
void eeprom_save_user(uint16_t user_num);
void eeprom_load_user(uint16_t user_num);
void eeprom_load_page(uint16_t page_num);
void eeprom_load_page_all(void);

#endif /* INC_AX_EEPROM_TASK_H_ */
