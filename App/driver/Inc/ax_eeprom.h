/*
 * ax_eeprom.h
 *
 *  Created on: Nov 29, 2025
 *      Author: alex
 */

#ifndef INC_AX_EEPROM_H_
#define INC_AX_EEPROM_H_

#include "main.h"
#include "user_def.h"

#define		EEP_ADDR_USER0		0x0
#define		EEP_ADDR_USER1		0x80
#define		EEP_ADDR_USER2		0x100
#define		EEP_ADDR_USER3		0x180

#define		EEP_ADDR_PAGE0		0x200
#define		EEP_ADDR_PAGE1		0x280
#define		EEP_ADDR_PAGE2		0x300
#define		EEP_ADDR_PAGE3		0x380
#define		EEP_ADDR_PAGE4		0x400
#define		EEP_ADDR_PAGE5		0x480
#define		EEP_ADDR_PAGE6		0x500
#define		EEP_ADDR_PAGE7		0x580

#define		EEP_ADDR_SYS			0x780

#define		EEP_ADDR_INTERLOCK0		0x780
#define		EEP_ADDR_INTERLOCK1		0x781
#define		EEP_ADDR_INTERLOCK2		0x782
#define		EEP_ADDR_INTERLOCK3		0x783

#define		EEP_ADDR_USER_DEFUALT	0x788
#define		EEP_ADDR_PAGE_START		0x789
#define		EEP_ADDR_PAGE_END		0x78A
#define		EEP_ADDR_OPER_MODE		0x78B


#define		EEP_ADDR_PAGE_REPEAT0	0x790
#define		EEP_ADDR_PAGE_REPEAT1	0x791
#define		EEP_ADDR_PAGE_REPEAT2	0x792
#define		EEP_ADDR_PAGE_REPEAT3	0x793
#define		EEP_ADDR_PAGE_REPEAT4	0x794
#define		EEP_ADDR_PAGE_REPEAT5	0x795
#define		EEP_ADDR_PAGE_REPEAT6	0x796
#define		EEP_ADDR_PAGE_REPEAT7	0x797




void eeprom_write_nbyte(uint16_t addr, uint8_t *p_para, uint16_t len);
void eeprom_read_nbyte(uint16_t addr, uint8_t *p_para, uint16_t len);
void eeprom_write_channel_con(uint16_t addr, pst_channel_con p_para);
void eeprom_read_channel_con(uint16_t addr, pst_channel_con p_para);

void eepram_test_1byte(void);
void eepram_test_page(void);


#endif /* INC_AX_EEPROM_H_ */
