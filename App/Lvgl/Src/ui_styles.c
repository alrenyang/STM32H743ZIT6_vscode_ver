#include "ui_styles.h"

/* 실제 스타일 객체 (단 1번만 생성됨) */
lv_style_t g_st_row;
lv_style_t g_st_row_focus;

static bool s_inited = false;

lv_style_t g_st_set_btn;
lv_style_t g_st_set_btn_focus;


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



void ui_styles_init_set_btn(void)
{
    if(s_inited) return;
    s_inited = true;

    /* 기본 버튼 */
    lv_style_init(&g_st_set_btn);
    lv_style_set_radius(&g_st_set_btn, 6);
    lv_style_set_border_width(&g_st_set_btn, 1);
    lv_style_set_border_color(&g_st_set_btn, lv_color_hex(0x2DE0C7));
    lv_style_set_bg_opa(&g_st_set_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&g_st_set_btn, lv_color_hex(0x0B1118));
    lv_style_set_text_color(&g_st_set_btn, lv_color_white());
    lv_style_set_pad_hor(&g_st_set_btn, 8);
    lv_style_set_pad_ver(&g_st_set_btn, 4);

    /* 포커스/선택(강조) */
    lv_style_init(&g_st_set_btn_focus);
    lv_style_set_border_width(&g_st_set_btn_focus, 2);
    lv_style_set_border_color(&g_st_set_btn_focus, lv_color_hex(0xFFD54A));
    lv_style_set_bg_opa(&g_st_set_btn_focus, LV_OPA_COVER);
    lv_style_set_bg_color(&g_st_set_btn_focus, lv_color_hex(0x12303A));
    lv_style_set_text_color(&g_st_set_btn_focus, lv_color_white());
}