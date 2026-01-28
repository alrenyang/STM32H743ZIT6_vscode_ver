#include "com_widget.h"
#include "widgets.h"

void table_build(ui_strobe_t * ui, lv_obj_t * parent);
lv_obj_t * table_cell_label(lv_obj_t * parent, const char * txt, lv_color_t col, lv_text_align_t align, lv_coord_t w);
void table_row_event_cb(lv_event_t * e);
lv_obj_t * table_btn_label(lv_obj_t * btn);
lv_obj_t * table_cell_btn(lv_obj_t * parent, const char * txt, lv_text_align_t align, lv_coord_t w);
void CH_open(ui_strobe_t * ui);
void CHPannel_mask_event_cb(lv_event_t * e);
void chk_save_all_event_cb(lv_event_t * e);
lv_obj_t * ch_make_item(ui_strobe_t * ui, lv_obj_t * parent, const char * name, int idx);
void ch_item_event_cb(lv_event_t * e);
void CHPannel_close_btn_cb(lv_event_t * e);
void ch_edit_timer_cb(lv_timer_t * t);
void ch_panel_refresh(ui_strobe_t * ui);
void CHPannel_close(ui_strobe_t * ui);
void ch_apply_to_all(ui_strobe_t *ui, uint16_t src);
void table_refresh_all(ui_strobe_t * ui);
int calc_encoder_step(int diff);
int  ch_get_focused_field(ui_strobe_t * ui);
st_trig_con * ui_get_active_trig_con(ui_strobe_t *ui);
char trig_mode_to_char(uint8_t m);
uint8_t trig_char_to_mode(char c);
int enc_abs(int x);