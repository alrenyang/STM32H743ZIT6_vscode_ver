#include "ui_styles.h"

/* 실제 스타일 객체 (단 1번만 생성됨) */
lv_style_t g_st_row;
lv_style_t g_st_row_focus;

static bool s_inited = false;

void ui_styles_init(void)
{
    if(s_inited) return;
    s_inited = true;

    /* normal row */
    lv_style_init(&g_st_row);
    lv_style_set_radius(&g_st_row, 10);
    lv_style_set_bg_opa(&g_st_row, LV_OPA_TRANSP);

    /* focused row */
    lv_style_init(&g_st_row_focus);
    lv_style_set_bg_color(&g_st_row_focus, lv_color_hex(0x0F2430));
    lv_style_set_bg_opa(&g_st_row_focus, LV_OPA_COVER);
    lv_style_set_outline_width(&g_st_row_focus, 1);
    lv_style_set_outline_pad(&g_st_row_focus, 0);
}
