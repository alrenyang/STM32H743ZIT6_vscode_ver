/*
 * user_def.h
 *
 *  Created on: Sep 20, 2025
 *      Author: alex
 */

#ifndef INC_USER_DEF_H_
#define INC_USER_DEF_H_

#include "main.h"

#define		UART2_DEBUG_ON			// level 0 debug on

#define		UART2_DEBUG_L2_ON		// level 2 debug on


#define		ax_VERSION		":FTS3-16CH Ver 1.0;"

#define	CH_MAX		16
#define PAGE_MAX	8
#define	USER_MAX	4

#define	PK_CMD_MAX		20
#define	PK_DATA_MAX		120
#define PK_SEND_MAX		120

#define	PK_STX		':'		// start
#define	PK_ETX		';'		// end
#define	PK_CTX		'!'		// command
#define	PK_RTX		'?'		// request
#define	PK_COMMA	','


/*
typedef enum {
	RISING  ,
	FALLING ,
	BOTH
} en_tirg_type ;
*/

typedef enum {
	PK_START,
	PK_CMD,
	PK_DATA,
	PK_END
} en_state;

typedef enum {
	PK_SET  ,
	PK_GET
} en_packet_type ;


// 개벌 제어 parameter
typedef struct {
	uint16_t delay;
	uint16_t on;
	uint16_t block;
	uint8_t  trig_mode;	// 0 : rising, 1 : falling, 2 : both
} st_channel_con, *pst_channel_con;


// trig mode 제어 Parameter
typedef struct {
	st_channel_con ch_con[CH_MAX];
	int interlock;		// 0 : 1ch, 1 : 2chs, 2 : 4chs, 3 : 8chs, 4 : 16chs
} st_trig_con, *pst_trig_con;


// Sequence mode 제어 Parameter : Page 제어 + start_page + end_page
// Sequence mode에서는 interlock은 사용하지 않음.
typedef struct {
	st_trig_con page_con[PAGE_MAX];
	uint8_t repeat_page[PAGE_MAX];
	uint8_t start_page;
	uint8_t end_page;
} st_seq_con, *pst_seq_con;


typedef struct {
	en_state state;
	en_packet_type type;
	uint8_t cmd_buf[PK_CMD_MAX];
	int cmd_index;
	uint8_t data_buf[PK_DATA_MAX];
	int data_index;
} st_rcv_packet, *pst_rcv_packet;



#endif /* INC_USER_DEF_H_ */
