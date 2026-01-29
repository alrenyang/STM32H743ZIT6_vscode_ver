#include "lvgl.h"
#include "lv_port_indev.h"
#include "packet_task.h"
#include "ax_eeprom_task.h"
#include "ui_styles.h"
#include "key_input.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ===== Colors (macro) ===== */
#define C_BG      lv_color_hex(0x070C11)
#define C_PANEL   lv_color_hex(0x0B1118)
#define C_LINE    lv_color_hex(0x1C2B34)

#define C_TEXT    lv_color_hex(0xDDE6EE)
#define C_DIM     lv_color_hex(0x8AA3B5)

#define C_TEAL    lv_color_hex(0x2DE0C7)
#define C_YEL     lv_color_hex(0xD8C26A)
#define C_TEAL_TX lv_color_hex(0x9FF6E9)
#define C_TX      lv_color_hex(0xDDE6EE)
#define C_GOLD    lv_color_hex(0xD7B36A)
#define C_RED     lv_color_hex(0xFF4D4D)

#define DISP_W  480
#define DISP_H  272

extern const char * const mode_str_tbl[];
extern lv_group_t * s_group;
extern uint32_t g_rs232_baud;
extern uint8_t  user_ip[4];


// const char * mode_str_tbl[] = {
//     "MODE: TRIGGER",
//     "MODE: SEQUENCE",
//     "MODE: RS232",
//     "MODE: ETHERNET"
// };

lv_obj_t * mode_make_btn(lv_obj_t * parent, const char * txt);