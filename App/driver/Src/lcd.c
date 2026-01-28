/*
 * lcd.c
 *
 *  Created on: Dec 8, 2025
 *      Author: alrenm3
 */


#include "lcd.h"
#include "main.h"
#include "stm32h7xx_hal_ltdc.h"

extern LTDC_HandleTypeDef hltdc;
#include "lcd_fonts.h"
#include <stdio.h>
#include <string.h>
#include "stm32h7xx.h"


#include "stm32h7xx_hal.h"

#define LCD_OPT_DEF   __attribute__((optimize("O2")))

static void (*s_vsync_cb)(void) = NULL;
static volatile uint8_t s_use_line_irq = 0;

static uint16_t *p_draw_frame_buf = NULL;

//static void disHanFont(int x, int y, han_font_t *FontPtr, uint16_t textcolor);
static void disEngFont(int x, int y, char ch, lcd_font_t *font, uint16_t textcolor);

static lcd_font_t *font_tbl[LCD_FONT_MAX] = { &font_07x10, &font_11x18, &font_16x26, &font_hangul};
static LcdFont lcd_font = LCD_FONT_11x18;

void lcdPrintf(int x, int y, uint16_t color,  const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];
  int Size_Char;
  int i, x_Pre = x;

  uint8_t font_width;
  uint8_t font_height;


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  if (font_tbl[lcd_font]->data != NULL)
  {
    for( i=0; i<len; i+=Size_Char )
    {
      disEngFont(x, y, print_buffer[i], font_tbl[lcd_font], color);

      Size_Char = 1;
      font_width = font_tbl[lcd_font]->width;
      font_height = font_tbl[lcd_font]->height;
      x += font_width;

      if ((x+font_width) > HW_LCD_WIDTH)
      {
        x  = x_Pre;
        y += font_height;
      }
    }
  }
  else
  {

//    for( i=0; i<len; i+=Size_Char )
//    {
//      hanFontLoad( &print_buffer[i], &FontBuf );
//
//      disHanFont( x, y, &FontBuf, color);
//
//      Size_Char = FontBuf.Size_Char;
//      if (Size_Char >= 2)
//      {
//        font_width = 16;
//        x += 2*8;
//      }
//      else
//      {
//        font_width = 8;
//        x += 1*8;
//      }
//
//      if ((x+font_width) > HW_LCD_WIDTH)
//      {
//        x  = x_Pre;
//        y += 16;
//      }
//
//      if( FontBuf.Code_Type == PHAN_END_CODE ) break;
//    }

  }

}

void disEngFont(int x, int y, char ch, lcd_font_t *font, uint16_t textcolor)
{
  uint32_t i, b, j;


  // We gaan door het font
  for (i = 0; i < font->height; i++)
  {
    b = font->data[(ch - 32) * font->height + i];
    for (j = 0; j < font->width; j++)
    {
      if ((b << j) & 0x8000)
      {
        lcdDrawPixel(x + j, (y + i), textcolor);
      }
    }
  }
}

LCD_OPT_DEF inline void lcdDrawPixel(int16_t x_pos, int16_t y_pos, uint32_t rgb_code)
{
  #if HW_LCD_SWAP_RGB > 0
  uint16_t rgb_out;

  if (x_pos < 0 || x_pos >= LCD_WIDTH) return;
  if (y_pos < 0 || y_pos >= LCD_HEIGHT) return;

  rgb_out  = (rgb_code>>8) & 0x00FF;
  rgb_out |= (rgb_code<<8) & 0xFF00;

  p_draw_frame_buf[y_pos * LCD_WIDTH + x_pos] = rgb_code;
  #else
  if (x_pos < 0 || x_pos >= LCD_WIDTH) return;
  if (y_pos < 0 || y_pos >= LCD_HEIGHT) return;

  p_draw_frame_buf[y_pos * LCD_WIDTH + x_pos] = rgb_code;
  #endif
}

void lcdSetFrameBuffer(uint16_t *fb)
{
  p_draw_frame_buf = fb;
}

uint16_t *lcdGetFrameBuffer(void)
{
  return (uint16_t *)LCD_ST_ADDR;
}

void LCD_ShowLogo(void)
{
  memcpy(LCD_FB_ADDR, tri_vision_logo_480x272_rgb565, 480*272*2);

  // D-Cache ON이면 필수
//  SCB_CleanDCache_by_Addr((uint32_t*)LCD_FB_ADDR, 480*272*2);
}

void lcdRegisterVsyncCallback(void (*cb)(void))
{
  s_vsync_cb = cb;
}

void lcdUpdateDraw(void)
{
  /* If MPU makes SDRAM non-cacheable, this clean is harmless (just extra cycles). */
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /* LCD_FB_ADDR is (uint16_t*)LCD_ST_ADDR */
  SCB_CleanDCache_by_Addr((uint32_t *)LCD_FB_ADDR, (int32_t)LCD_FB_SIZE);
#endif

  /* For LTDC there is no "transfer"; the display reads framebuffer continuously.
   * So we can signal completion immediately.
   * If you later add tear-free VSYNC sync, call s_vsync_cb() from LTDC line/VSYNC interrupt instead.
   */
  if (s_vsync_cb)
  {
    if (!s_use_line_irq)
    {
      s_vsync_cb();
    }
    /* else: will be called from HAL_LTDC_LineEventCallback */
  }
}

void lcdEnableVsyncLineInterrupt(uint32_t line)
{
  s_use_line_irq = 1;

  /* Enable LTDC line interrupt and program line event */
  __HAL_LTDC_ENABLE_IT(&hltdc, LTDC_IT_LI);

  HAL_LTDC_ProgramLineEvent(&hltdc, line);
}

/* HAL weak callback: called on LTDC Line Interrupt */
void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hl)
{
  (void)hl;

  /* Re-arm the line event for continuous interrupts */
  HAL_LTDC_ProgramLineEvent(&hltdc, 0);

  if (s_vsync_cb)
  {
    s_vsync_cb();
  }
}


