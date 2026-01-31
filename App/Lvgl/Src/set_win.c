#include "set_win.h"

uint32_t s_baud_tbl[3] = { 9600, 57600, 115200 };

typedef struct {
    ui_strobe_t * ui;
    uint8_t      idx;   // 0..3
} ip_cell_ctx_t;

static ip_cell_ctx_t s_ip_ctx[4];

void baud_btn_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    lv_obj_t * btn = lv_event_get_target(e);

    if(!ui) return;
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    int idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    if(idx < 0 || idx >= 3) return;

    ui->baud_idx = (uint8_t)idx;
    g_rs232_baud = s_baud_tbl[idx];

    /* === 선택 상태 갱신 === */
    for(int i = 0; i < 3; i++) {
        if(i == idx) lv_obj_add_state(ui->baud_btn[i], LV_STATE_CHECKED);
        else         lv_obj_clear_state(ui->baud_btn[i], LV_STATE_CHECKED);
    }

    /* === ★ widgets.c의 통신 상태 라벨 갱신 ★ === */
    if(ui->lbl_comm) {
        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%lu", (unsigned long)g_rs232_baud);
        lv_label_set_text(ui->lbl_comm, buf);
    }
}

static void ip_apply_to_user_ip(ui_strobe_t * ui)
{
    for(int i = 0; i < 4; i++) user_ip[i] = ui->ip_cell[i].value;
}

void ip_cell_event_cb(lv_event_t * e)
{
    ip_cell_ctx_t * ctx = (ip_cell_ctx_t *)lv_event_get_user_data(e);
    if(!ctx || !ctx->ui) return;

    ui_strobe_t * ui = ctx->ui;
    uint8_t idx = ctx->idx;
    if(idx >= 4) return;

    lv_event_code_t code = lv_event_get_code(e);

    /* 1) 엔코더 누름(=CLICK/ENTER) : 편집 모드 토글 */
    if(code == LV_EVENT_CLICKED) {
        if(ui->grp_setting) {
            bool edit = lv_group_get_editing(ui->grp_setting);
            lv_group_set_editing(ui->grp_setting, !edit);

            /* 선택 상태 표시(원하면) */
            if(!edit) lv_obj_add_state(ui->ip_cell[idx].btn, LV_STATE_CHECKED);
            else      lv_obj_clear_state(ui->ip_cell[idx].btn, LV_STATE_CHECKED);
        }
        return;
    }

    /* 2) 엔코더 회전은 LV_EVENT_KEY로 들어옴 (편집 모드일 때만 처리) */
    if(code == LV_EVENT_KEY) {
        if(!ui->grp_setting) return;
        if(!lv_group_get_editing(ui->grp_setting)) return;  // 편집 모드 아닐 땐 포커스 이동 유지

        uint32_t key = lv_event_get_key(e);

        if(key == LV_KEY_RIGHT) {
            ui->ip_cell[idx].value = (uint8_t)((ui->ip_cell[idx].value + 1) & 0xFF);
        } else if(key == LV_KEY_LEFT) {
            ui->ip_cell[idx].value = (uint8_t)((ui->ip_cell[idx].value - 1) & 0xFF);
        } else {
            return;
        }

        char buf[4];
        lv_snprintf(buf, sizeof(buf), "%u", (unsigned)ui->ip_cell[idx].value);
        lv_label_set_text(ui->ip_cell[idx].lbl, buf);

        ip_apply_to_user_ip(ui);
        return;
    }
}


// void ip_cell_event_cb(lv_event_t * e)
// {
//     ip_cell_ctx_t * ctx = (ip_cell_ctx_t *)lv_event_get_user_data(e);
//     if(!ctx || !ctx->ui) return;

//     ui_strobe_t * ui = ctx->ui;
//     uint8_t idx = ctx->idx;
//     if(idx >= 4) return;

//     if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

//     /* 값 증가 */
//     ui->ip_cell[idx].value = (uint8_t)((ui->ip_cell[idx].value + 1) % 256);

//     char buf[4];
//     lv_snprintf(buf, sizeof(buf), "%u", (unsigned)ui->ip_cell[idx].value);
//     lv_label_set_text(ui->ip_cell[idx].lbl, buf);

//     /* 실제 user_ip[]도 즉시 반영 */
//     for(int i = 0; i < 4; i++) {
//         user_ip[i] = ui->ip_cell[i].value;
//     }
// }
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
    lv_obj_set_style_bg_opa(ui->SETTING_mask, LV_OPA_80, 0);
    lv_obj_set_style_bg_color(ui->SETTING_mask, lv_color_black(), 0);
    lv_obj_add_flag(ui->SETTING_mask, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui->SETTING_mask, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_add_event_cb(ui->SETTING_mask,
    //                     Setting_mask_event_cb, LV_EVENT_CLICKED, ui);

    ui_styles_init_set_btn();
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
    lv_obj_set_style_border_color(ui->SETTING_panel,
                                  lv_color_hex(0x2DE0C7), 0);

    lv_obj_set_style_pad_all(ui->SETTING_panel, 8, 0);
    lv_obj_set_style_pad_row(ui->SETTING_panel, 6, 0);
    lv_obj_set_flex_flow(ui->SETTING_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(ui->SETTING_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(ui->SETTING_panel, LV_OBJ_FLAG_EVENT_BUBBLE);

    /* ================= Title ================= */
    lv_obj_t * title = lv_label_create(ui->SETTING_panel);
    lv_label_set_text(title, "SETTING");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_line_space(title, 0, 0);

   /* ================= Row: RS232 Baud ================= */
    lv_obj_t * row_baud = lv_obj_create(ui->SETTING_panel);
    lv_obj_remove_style_all(row_baud);
    lv_obj_set_width(row_baud, LV_PCT(100));
    lv_obj_set_height(row_baud, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row_baud, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(row_baud, 0, 0);
    lv_obj_set_style_pad_row(row_baud, 6, 0);   // 라벨-버튼 간격
    lv_obj_set_style_pad_column(row_baud, 6, 0);

    lv_obj_t * lbl_baud = lv_label_create(row_baud);
    lv_label_set_text(lbl_baud, "RS232 Baud");
    lv_obj_set_style_text_color(lbl_baud, lv_color_white(), 0);

    /* 버튼 라인 */
    lv_obj_t * baud_line = lv_obj_create(row_baud);
    lv_obj_remove_style_all(baud_line);
    lv_obj_set_width(baud_line, LV_PCT(100));
    lv_obj_set_height(baud_line, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(baud_line, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_row(baud_line, 6, 0);
    lv_obj_set_style_pad_column(baud_line, 8, 0);

    /* 초기 선택값(예: g_rs232_baud 기반) */
    ui->baud_idx = 2; // 기본 115200
    for(int i=0;i<3;i++){
        if(g_rs232_baud == s_baud_tbl[i]) { ui->baud_idx = (uint8_t)i; break; }
    }

    for(int i = 0; i < 3; i++) {
        ui->baud_btn[i] = lv_btn_create(baud_line);
        lv_obj_set_size(ui->baud_btn[i], 72, 30);

        /* 스타일만 적용 (동작/레이아웃 변경 없음) */
        lv_obj_add_style(ui->baud_btn[i], &g_st_set_btn, 0);
        lv_obj_add_style(ui->baud_btn[i], &g_st_set_btn_focus, LV_STATE_FOCUSED);
        lv_obj_add_style(ui->baud_btn[i], &g_st_set_btn_focus, LV_STATE_CHECKED);

        /* checkable로 만들어서 선택 표시 가능 */
        lv_obj_add_flag(ui->baud_btn[i], LV_OBJ_FLAG_CHECKABLE);

        /* 버튼 user_data에 index 저장 */
        lv_obj_set_user_data(ui->baud_btn[i], (void *)(intptr_t)i);

        ui->baud_lbl[i] = lv_label_create(ui->baud_btn[i]);
        char t[8];
        lv_snprintf(t, sizeof(t), "%lu", (unsigned long)s_baud_tbl[i]);
        lv_label_set_text(ui->baud_lbl[i], t);
        lv_obj_center(ui->baud_lbl[i]);

        lv_obj_add_event_cb(ui->baud_btn[i], baud_btn_event_cb, LV_EVENT_CLICKED, ui);
    }

    /* 초기 체크 반영 */
    for(int i = 0; i < 3; i++) {
        if(i == ui->baud_idx) lv_obj_add_state(ui->baud_btn[i], LV_STATE_CHECKED);
        else                  lv_obj_clear_state(ui->baud_btn[i], LV_STATE_CHECKED);
    }

    /* ================= Row: TCP/IP Address ================= */
    lv_obj_t * row_ip = lv_obj_create(ui->SETTING_panel);
    lv_obj_remove_style_all(row_ip);
    lv_obj_set_width(row_ip, LV_PCT(100));
    lv_obj_set_height(row_ip, LV_SIZE_CONTENT);

    lv_obj_set_flex_flow(row_ip, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(row_ip, 0, 0);
    lv_obj_set_style_pad_row(row_ip, 6, 0);

    lv_obj_t * lbl_ip = lv_label_create(row_ip);
    lv_label_set_text(lbl_ip, "TCP/IP Address");
    lv_obj_set_style_text_color(lbl_ip, lv_color_white(), 0);
    lv_obj_set_style_text_line_space(lbl_ip, 0, 0);

    lv_obj_t * ip_line = lv_obj_create(row_ip);
    lv_obj_remove_style_all(ip_line);
    lv_obj_set_width(ip_line, LV_PCT(100));
    lv_obj_set_height(ip_line, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ip_line, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(ip_line, 2, 0);
    lv_obj_set_style_pad_column(ip_line, 6, 0);

    for(int i = 0; i < 4; i++) {

        /* 버튼 */
        ui->ip_cell[i].btn = lv_btn_create(ip_line);
        lv_obj_set_size(ui->ip_cell[i].btn, 56, 30);

        /* 스타일만 적용 (동작/레이아웃 변경 없음) */
        lv_obj_add_style(ui->ip_cell[i].btn, &g_st_set_btn, 0);
        lv_obj_add_style(ui->ip_cell[i].btn, &g_st_set_btn_focus, LV_STATE_FOCUSED);
        lv_obj_add_style(ui->ip_cell[i].btn, &g_st_set_btn_focus, LV_STATE_PRESSED);

        /* 라벨 */
        ui->ip_cell[i].lbl = lv_label_create(ui->ip_cell[i].btn);

        /* 초기 값 */
        ui->ip_cell[i].value = (uint8_t)user_ip[i];

        char tmp[4];
        lv_snprintf(tmp, sizeof(tmp), "%u", (unsigned)ui->ip_cell[i].value);
        lv_label_set_text(ui->ip_cell[i].lbl, tmp);
        lv_obj_center(ui->ip_cell[i].lbl);


        s_ip_ctx[i].ui  = ui;
        s_ip_ctx[i].idx = (uint8_t)i;

        lv_obj_add_event_cb(ui->ip_cell[i].btn, ip_cell_event_cb, LV_EVENT_CLICKED, &s_ip_ctx[i]);
        lv_obj_add_event_cb(ui->ip_cell[i].btn, ip_cell_event_cb, LV_EVENT_KEY,     &s_ip_ctx[i]);


        /* 점(.) */
        if(i < 3) {
            ui->ip_dot[i] = lv_label_create(ip_line);
            lv_label_set_text(ui->ip_dot[i], ".");
            lv_obj_set_style_text_color(ui->ip_dot[i], lv_color_white(), 0);
        } else {
            /* 마지막은 dot 없음 */
        }
    }

    /* ================= Button: CLOSE only ================= */
    lv_obj_t * row_btn = lv_obj_create(ui->SETTING_panel);
    lv_obj_remove_style_all(row_btn);
    lv_obj_set_width(row_btn, LV_PCT(100));
    lv_obj_set_height(row_btn, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row_btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row_btn, 0, 0);

    ui->btn_close = mode_make_btn(row_btn, "CLOSE");
    lv_obj_add_event_cb(ui->btn_close,setting_close_event_cb, LV_EVENT_CLICKED, ui);

    lv_obj_add_style(ui->btn_close, &g_st_set_btn, 0);
    lv_obj_add_style(ui->btn_close, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(ui->btn_close, &g_st_set_btn_focus, LV_STATE_PRESSED);

    /* ================= Group (Encoder / Keypad) ================= */
    ui->grp_setting_prev = s_group;
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

    lv_group_add_obj(ui->grp_setting, ui->baud_btn[0]);
    lv_group_add_obj(ui->grp_setting, ui->baud_btn[1]);
    lv_group_add_obj(ui->grp_setting, ui->baud_btn[2]);
    lv_group_add_obj(ui->grp_setting, ui->ip_cell[0].btn);
    lv_group_add_obj(ui->grp_setting, ui->ip_cell[1].btn);
    lv_group_add_obj(ui->grp_setting, ui->ip_cell[2].btn);
    lv_group_add_obj(ui->grp_setting, ui->ip_cell[3].btn);
    lv_group_add_obj(ui->grp_setting, ui->btn_close);

    lv_group_focus_obj(ui->ip_cell[0].btn);

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

void setting_close_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    /* 닫기 함수가 있다면 호출 */
    widgets_update_ip_label(ui);
    Setting_window_close(ui);
}

void Setting_window_close(ui_strobe_t * ui)
{
    if(!ui) return;

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

    /* SETTING 마스크/패널 삭제 (mask 삭제하면 자식(panel 포함)도 같이 삭제됨) */
    if(ui->SETTING_mask) {
        lv_obj_del(ui->SETTING_mask);
        ui->SETTING_mask = NULL;
    }

    /* 포인터 정리 */
    ui->SETTING_panel     = NULL;
    ui->SETTING_btn_close = NULL;

    /* ip_cell / dot 정리 (구조체는 NULL 불가 → 멤버만 NULL) */
    for(int i = 0; i < 4; i++) {
        ui->ip_cell[i].btn   = NULL;
        ui->ip_cell[i].lbl   = NULL;
        ui->ip_cell[i].value = 0;
    }
    for(int i = 0; i < 3; i++) {
        ui->ip_dot[i] = NULL;
    }

    ui->btn_close = NULL;
}


// uint32_t baud_from_idx(uint16_t idx)
// {
//     static const uint32_t tbl[] = { 9600, 57600, 115200};
//     if(idx >= (sizeof(tbl)/sizeof(tbl[0]))) return 115200;
//     return tbl[idx];
// }

// bool parse_octet(const char *s, uint8_t *out)
// {
//     if(!s || !*s) return false;
//     int v = atoi(s);
//     if(v < 0 || v > 255) return false;
//     *out = (uint8_t)v;
//     return true;
// }
