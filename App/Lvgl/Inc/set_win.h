#include "com_widget.h"
#include "widgets.h"

void Setting_window_open(ui_strobe_t * ui);
void Setting_mask_event_cb(lv_event_t * e);
void setting_close_event_cb(lv_event_t * e);
void Setting_window_close(ui_strobe_t * ui);
uint32_t baud_from_idx(uint16_t idx);
bool parse_octet(const char *s, uint8_t *out);
void setting_apply_event_cb(lv_event_t * e);
void ip_cell_event_cb(lv_event_t * e);
void baud_btn_event_cb(lv_event_t * e);