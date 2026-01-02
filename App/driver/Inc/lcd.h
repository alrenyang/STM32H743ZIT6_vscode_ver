/*
 * lcd.h
 *
 *  Created on: Dec 8, 2025
 *      Author: alrenm3
 */

#ifndef SRC_LCD_H_
#define SRC_LCD_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "tri_vision_logo_480x272_rgb565.h"

#define HW_LCD_WIDTH           480
#define HW_LCD_HEIGHT          272

#define LCD_WIDTH         HW_LCD_WIDTH
#define LCD_HEIGHT        HW_LCD_HEIGHT

//#define LCD_FB_ADDR ((uint16_t*)0xC0000000)
//#if 1
//#define LCD_ST_ADDR	0xC0000000UL
//#else
#define LCD_ST_ADDR	0x24040400UL
//#endif
#define LCD_FB_ADDR ((uint16_t*)LCD_ST_ADDR)

#define LCD_FB_SIZE (LCD_WIDTH * LCD_HEIGHT * 2)



#define GETR(c) (((uint16_t)(c)) >> 11)
#define GETG(c) (((c) & 0x07E0)>>5)
#define GETB(c) ((c) & 0x1F)
#define RGB2COLOR(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

enum class_color {
 white     = 0xFFFF,
 gray      = 0x8410,
 darkgray  = 0xAD55,
 black     = 0x0000,
 purple    = 0x8010,
 pink      = 0xFE19,
 red       = 0xF800,
 orange    = 0xFD20,
 brown     = 0xA145,
 beige     = 0xF7BB,
 yellow    = 0xFFE0,
 lightgreen= 0x9772,
 green     = 0x07E0,
 darkblue  = 0x0011,
 blue      = 0x001F,
 lightblue = 0xAEDC,
};

typedef enum
{
  LCD_FONT_07x10,
  LCD_FONT_11x18,
  LCD_FONT_16x26,
  LCD_FONT_HAN,
  LCD_FONT_MAX
} LcdFont;

typedef struct lcd_driver_t_ lcd_driver_t;

typedef struct lcd_driver_t_
{
  bool     (*init)(void);
  bool     (*reset)(void);
  void     (*setWindow)(int32_t x, int32_t y, int32_t w, int32_t h);
  void     (*setRotation)(uint8_t mode);
  uint16_t (*getWidth)(void);
  uint16_t (*getHeight)(void);
  bool     (*setCallBack)(void (*p_func)(void));
  bool     (*sendBuffer)(uint8_t *p_data, uint32_t length, uint32_t timeout_ms);

} lcd_driver_t;

uint16_t *lcdGetFrameBuffer(void);
void lcdSetFrameBuffer(uint16_t *fb);
void lcdDrawPixel(int16_t x_pos, int16_t y_pos, uint32_t rgb_code);
void lcdPrintf(int x, int y, uint16_t color,  const char *fmt, ...);
void LCD_ShowLogo(void);
void lcdUpdateDraw(void);
void lcdRegisterVsyncCallback(void (*cb)(void));
void lcdEnableVsyncLineInterrupt(uint32_t line);

#endif /* SRC_LCD_H_ */
