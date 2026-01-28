#pragma once
#include "lvgl.h"
#include "com_widget.h"

/* Row focus styles (shared) */
extern lv_style_t g_st_row;
extern lv_style_t g_st_row_focus;

/* 초기화 함수 */
void ui_styles_init(void);
