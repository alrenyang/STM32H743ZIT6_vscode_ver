
/**
 * @file lv_port_indev_templ.h
 *
 */

/*Copy this file as "lv_port_indev.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_INDEV_TEMPL_H
#define LV_PORT_INDEV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    USE_KEY_MODE     = 1,  /*0x11*/
    USE_KEY_SET      = 2,  /*0x12*/
    USE_KEY_INTER    = 3,  /*0x13*/
    USE_KEY_MEM      = 4,  /*0x14*/
    USE_KEY_UP       = 5,  /*0x1B*/
    USE_KEY_DOWN     = 6, /*0x7F*/
    USE_KEY_LOCK     = 7, /*0x7F*/
}user_key_map;
/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_indev_init(void);

void lv_port_indev_poll_5ms(void);

lv_indev_t * lv_port_indev_get_encoder(void);
lv_indev_t * lv_port_indev_get_keypad(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_INDEV_TEMPL_H*/

#endif /*Disable/Enable content*/
