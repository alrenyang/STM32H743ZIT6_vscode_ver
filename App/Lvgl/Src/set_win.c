#include "set_win.h"



/*-----------------------------------------------------------*/
// setting window 모드키 누르면 팝업창 생성
/*-----------------------------------------------------------*/

void Setting_window_open(ui_strobe_t * ui)
{
    if(!ui) return;
    if(ui->SETTING_mask) return;

    lv_obj_t * scr = lv_screen_active();

    /* ================= Mask ================= */
    ui->SETTING_mask = lv_obj_create(scr);
    lv_obj_remove_style_all(ui->SETTING_mask);
    lv_obj_set_size(ui->SETTING_mask, DISP_W, DISP_H);
    lv_obj_set_pos(ui->SETTING_mask, 0, 0);
    lv_obj_set_style_bg_opa(ui->SETTING_mask, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(ui->SETTING_mask, lv_color_black(), 0);
    lv_obj_add_flag(ui->SETTING_mask, LV_OBJ_FLAG_CLICKABLE);

    /* ★ 중요: SETTING 전용 마스크 콜백 */
    lv_obj_add_event_cb(ui->SETTING_mask, Setting_mask_event_cb, LV_EVENT_CLICKED, ui);

    /* ★ 중요: 스크롤 금지 */
    lv_obj_clear_flag(ui->SETTING_mask, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Panel ================= */
    const lv_coord_t pw = (DISP_W * 2) / 3;
    const lv_coord_t ph = (DISP_H * 2) / 3;

    ui->SETTING_panel = lv_obj_create(ui->SETTING_mask);
    lv_obj_remove_style_all(ui->SETTING_panel);
    lv_obj_set_size(ui->SETTING_panel, pw, ph);
    lv_obj_center(ui->SETTING_panel);

    lv_obj_set_style_bg_opa(ui->SETTING_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(ui->SETTING_panel, lv_color_hex(0x0B1118), 0);
    lv_obj_set_style_radius(ui->SETTING_panel, 10, 0);
    lv_obj_set_style_border_width(ui->SETTING_panel, 2, 0);
    lv_obj_set_style_border_color(ui->SETTING_panel, lv_color_hex(0x2DE0C7), 0);
    lv_obj_set_style_pad_all(ui->SETTING_panel, 12, 0);
    lv_obj_set_style_pad_row(ui->SETTING_panel, 8, 0);

    lv_obj_set_flex_flow(ui->SETTING_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(ui->SETTING_panel, LV_OBJ_FLAG_EVENT_BUBBLE);

    /* ★ 중요: panel 스크롤 금지 */
    lv_obj_clear_flag(ui->SETTING_panel, LV_OBJ_FLAG_SCROLLABLE);

    /* ===== Title ===== */
    lv_obj_t * title = lv_label_create(ui->SETTING_panel);
    lv_label_set_text(title, "SETTING");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);

    /* ===== Row: RS232 Baudrate ===== */
    lv_obj_t * row_baud = lv_obj_create(ui->SETTING_panel);
    lv_obj_remove_style_all(row_baud);
    lv_obj_set_width(row_baud, LV_PCT(100));
    lv_obj_set_flex_flow(row_baud, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(row_baud, 10, 0);

    lv_obj_t * lbl_baud = lv_label_create(row_baud);
    lv_label_set_text(lbl_baud, "RS232 Baud");
    lv_obj_set_style_text_color(lbl_baud, lv_color_white(), 0);

    ui->dd_baud = lv_dropdown_create(row_baud);
    lv_dropdown_set_options(ui->dd_baud,
        "9600\n"
        "57600\n"
        "115200\n"
    );

    switch(g_rs232_baud){
    case 9600:   lv_dropdown_set_selected(ui->dd_baud, 0); break;
    case 57600:  lv_dropdown_set_selected(ui->dd_baud, 1); break;
    case 115200: lv_dropdown_set_selected(ui->dd_baud, 2); break;
    default:     lv_dropdown_set_selected(ui->dd_baud, 2); break;
    }

    /* ===== Row: TCP/IP Address ===== */
    lv_obj_t * row_ip = lv_obj_create(ui->SETTING_panel);
    lv_obj_remove_style_all(row_ip);
    lv_obj_set_width(row_ip, LV_PCT(100));
    lv_obj_set_flex_flow(row_ip, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(row_ip, 6, 0);

    lv_obj_t * lbl_ip = lv_label_create(row_ip);
    lv_label_set_text(lbl_ip, "TCP/IP Address");
    lv_obj_set_style_text_color(lbl_ip, lv_color_white(), 0);

    lv_obj_t * ip_line = lv_obj_create(row_ip);
    lv_obj_remove_style_all(ip_line);
    lv_obj_set_width(ip_line, LV_PCT(100));
    lv_obj_set_flex_flow(ip_line, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(ip_line, 6, 0);

    for(int i=0;i<4;i++){
        ui->ta_ip[i] = lv_textarea_create(ip_line);
        lv_obj_set_size(ui->ta_ip[i], 48, 26);
        lv_textarea_set_one_line(ui->ta_ip[i], true);
        lv_textarea_set_max_length(ui->ta_ip[i], 3);
        lv_textarea_set_accepted_chars(ui->ta_ip[i], "0123456789");
        lv_textarea_set_text(ui->ta_ip[i], "0");
        lv_textarea_set_cursor_click_pos(ui->ta_ip[i], false);

        if(i != 3){
            lv_obj_t * dot = lv_label_create(ip_line);
            lv_label_set_text(dot, ".");
            lv_obj_set_style_text_color(dot, lv_color_white(), 0);
        }
    }

    char tmp[4];
    for(int i=0;i<4;i++){
        lv_snprintf(tmp, sizeof(tmp), "%u", (unsigned)user_ip[i]);
        lv_textarea_set_text(ui->ta_ip[i], tmp);
    }

    /* ===== Buttons (Apply / Close) ===== */
    lv_obj_t * row_btn = lv_obj_create(ui->SETTING_panel);
    lv_obj_remove_style_all(row_btn);
    lv_obj_set_width(row_btn, LV_PCT(100));
    lv_obj_set_flex_flow(row_btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(row_btn, 10, 0);

    ui->btn_apply = mode_make_btn(row_btn, "APPLY");
    ui->btn_close = mode_make_btn(row_btn, "CLOSE");

    lv_obj_add_event_cb(ui->btn_apply, setting_apply_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->btn_close, setting_close_event_cb, LV_EVENT_CLICKED, ui);

    /* ================= Group (Encoder/Keypad) =================
     * 중요: 위젯 생성이 끝난 "마지막"에 그룹 생성/연결/포커스
     */
    ui->grp_setting_prev = s_group;          /* (헤더에 추가했거나, 없다면 이 줄 제거) */
    ui->grp_setting = lv_group_create();
    lv_group_set_wrap(ui->grp_setting, true);
    lv_group_set_editing(ui->grp_setting, false);

    lv_group_set_default(ui->grp_setting);

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();
    if(enc) lv_indev_set_group(enc, ui->grp_setting);
    if(kp)  lv_indev_set_group(kp,  ui->grp_setting);

    if(enc) lv_indev_wait_release(enc);
    if(kp)  lv_indev_wait_release(kp);

    lv_group_add_obj(ui->grp_setting, ui->dd_baud);
    lv_group_add_obj(ui->grp_setting, ui->ta_ip[0]);
    lv_group_add_obj(ui->grp_setting, ui->ta_ip[1]);
    lv_group_add_obj(ui->grp_setting, ui->ta_ip[2]);
    lv_group_add_obj(ui->grp_setting, ui->ta_ip[3]);
    lv_group_add_obj(ui->grp_setting, ui->btn_apply);
    lv_group_add_obj(ui->grp_setting, ui->btn_close);

    lv_group_focus_obj(ui->dd_baud);
}

/*-----------------------------------------------------------*/
// SETTING mask click -> close
/*-----------------------------------------------------------*/
void Setting_mask_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_target(e) != ui->SETTING_mask) return;

    Setting_window_close(ui);
}


void setting_apply_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    /* 1) Baud 반영 */
    uint16_t sel = lv_dropdown_get_selected(ui->dd_baud);

    // extern uint32_t g_rs232_baud;
    g_rs232_baud = baud_from_idx(sel);

    /* TODO: 실제 UART 재설정 함수 호출 */
    // RS232_Reinit(g_rs232_baud);

    /* 2) IP 반영(검증) */
    uint8_t new_ip[4];
    for(int i=0;i<4;i++){
        if(!parse_octet(lv_textarea_get_text(ui->ta_ip[i]), &new_ip[i])){
            /* 값이 이상하면 여기서 에러 표시(토스트/라벨 등) */
            // show_toast("Invalid IP");
            return;
        }
    }

    for(int i=0;i<4;i++) user_ip[i] = new_ip[i];

    /* TODO: 실제 TCP/IP 스택 재설정/저장 */
    // Net_SetIP(user_ip);
    // Config_Save();

    /* 필요하면 창 닫기 */
    // Setting_window_close(ui);
}

void setting_close_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    /* 닫기 함수가 있다면 호출 */
    Setting_window_close(ui);
}

void Setting_window_close(ui_strobe_t * ui)
{
    if(!ui) return;

    /* 타이머가 있으면 정지 (있을 때만) */
    // if(ui->setting_timer) {
    //     lv_timer_pause(ui->setting_timer);
    // }

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();

    /* 잔류 입력 제거(중요: 닫자마자 아래 위젯 클릭/재오픈 방지) */
    if(enc) lv_indev_wait_release(enc);
    if(kp)  lv_indev_wait_release(kp);

    /* group 복구 */
    lv_group_t * back = ui->grp_setting_prev ? ui->grp_setting_prev : s_group;
    if(enc) lv_indev_set_group(enc, back);
    if(kp)  lv_indev_set_group(kp,  back);

    /* setting group 삭제 */
    if(ui->grp_setting) {
        lv_group_del(ui->grp_setting);
        ui->grp_setting = NULL;
    }
    ui->grp_setting_prev = NULL;

    /* SETTING 마스크/패널 삭제 */
    if(ui->SETTING_mask) {
        lv_obj_del(ui->SETTING_mask);
        ui->SETTING_mask = NULL;
    }

    /* 포인터 정리 */
    ui->SETTING_panel   = NULL;
    ui->SETTING_btn_close = NULL;

    ui->dd_baud = NULL;
    for(int i=0;i<4;i++) ui->ta_ip[i] = NULL;

    ui->btn_apply = NULL;
    ui->btn_close = NULL;
}

uint32_t baud_from_idx(uint16_t idx)
{
    static const uint32_t tbl[] = { 9600, 57600, 115200};
    if(idx >= (sizeof(tbl)/sizeof(tbl[0]))) return 115200;
    return tbl[idx];
}

bool parse_octet(const char *s, uint8_t *out)
{
    if(!s || !*s) return false;
    int v = atoi(s);
    if(v < 0 || v > 255) return false;
    *out = (uint8_t)v;
    return true;
}
