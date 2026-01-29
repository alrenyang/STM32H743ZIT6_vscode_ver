#include "com_widget.h"

const char * const mode_str_tbl[] = {
    "MODE: TRIGGER",
    "MODE: SEQUENCE",
    "MODE: RS232",
    "MODE: ETHERNET"
};




/*기본 버튼 스타일 공통 적용*/
lv_obj_t * mode_make_btn(lv_obj_t * parent, const char * txt)
{
    lv_obj_t * btn = lv_btn_create(parent);

    lv_obj_set_width(btn, LV_PCT(100));
    lv_obj_set_height(btn, 28);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1C2B34), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(btn, 6, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x2DE0C7), 0);

    lv_obj_set_style_pad_all(btn, 4, 0);

    lv_obj_set_style_outline_width(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(btn, lv_color_hex(0x2DE0C7), LV_STATE_FOCUSED);
    lv_obj_set_style_outline_pad(btn, 2, LV_STATE_FOCUSED);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x243844), LV_STATE_FOCUSED);

    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, txt);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xCFEFF0), 0);
    lv_obj_center(lbl);

    return btn;
}