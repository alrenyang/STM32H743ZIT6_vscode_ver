#include "com_widget.h"
#include "widgets.h"

void Int_window_open(ui_strobe_t * ui);
void Int_mask_event_cb(lv_event_t * e);
void Int_window_close(ui_strobe_t * ui);
void int_btn_event_cb(lv_event_t * e);
void Int_close_btn_cb(lv_event_t * e);

void int_set_btn_text(lv_obj_t * btn, uint8_t v);
void int_target_btn_event_cb(lv_event_t * e);
