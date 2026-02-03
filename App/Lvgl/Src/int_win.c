#include "int_win.h"
#include <stdint.h>

static const uint8_t s_int_tbl[5] = {1,2,4,6,8};

static int int_val_to_idx(uint8_t v)
{
    for(int i = 0; i < 5; i++) {
        if(s_int_tbl[i] == v) return i;
    }
    return 0;
}

static uint8_t int_idx_wrap(int idx)
{
    while(idx < 0) idx += 5;
    while(idx >= 5) idx -= 5;
    return (uint8_t)idx;
}


void int_target_btn_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);

    uint8_t is_start = (btn == ui->int_btn_start);
    uint8_t is_end   = (btn == ui->int_btn_end);
    if(!is_start && !is_end) return;

    if(code == LV_EVENT_FOCUSED) {
        ui->int_pick_target = is_start ? 0 : 1;
        return;
    }

    if(code == LV_EVENT_CLICKED) {
        ui->int_pick_target = is_start ? 0 : 1;

        if(ui->INT_grp) {
            bool ed = lv_group_get_editing(ui->INT_grp);

            /* 클릭할 때마다 editing 토글 */
            lv_group_set_editing(ui->INT_grp, !ed);

            /* 잔류 입력 제거(선택) */
            lv_indev_t * enc = lv_port_indev_get_encoder();
            if(enc) lv_indev_wait_release(enc);
        }

        return;
    }

    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);

        if(key == LV_KEY_ENTER) {
            if(ui->INT_grp) {
                bool ed = lv_group_get_editing(ui->INT_grp);
                lv_group_set_editing(ui->INT_grp, !ed);
            }
            return;
        }

        int step = 0;
        if(key == LV_KEY_LEFT || key == LV_KEY_PREV) step = -1;
        else if(key == LV_KEY_RIGHT || key == LV_KEY_NEXT) step = +1;
        else return;

        uint8_t cur = is_start ? ui->int_start_value : ui->int_end_value;
        int idx = int_val_to_idx(cur);
        idx = (int)int_idx_wrap(idx + step);
        uint8_t nv = s_int_tbl[idx];

        if(is_start) {
            ui->int_start_value = nv;
            if(ui->int_start_value > ui->int_end_value) ui->int_end_value = ui->int_start_value;
        } else {
            ui->int_end_value = nv;
            if(ui->int_end_value < ui->int_start_value) ui->int_start_value = ui->int_end_value;
        }

        int_set_btn_text(ui->int_btn_start, ui->int_start_value);
        int_set_btn_text(ui->int_btn_end,   ui->int_end_value);

        g_seq_con.start_page = ui->int_start_value;
        g_seq_con.end_page   = ui->int_end_value;

        /* START / END 라벨 갱신 */
        if(ui->int_str) {
            char buf[16];
            lv_snprintf(buf, sizeof(buf), "START:%u", (unsigned)ui->int_start_value);
            lv_label_set_text(ui->int_str, buf);
        }

        if(ui->int_end) {
            char buf[16];
            lv_snprintf(buf, sizeof(buf), "END:%u", (unsigned)ui->int_end_value);
            lv_label_set_text(ui->int_end, buf);
        }

        return;
    }
}


void Int_window_close(ui_strobe_t * ui)
{
    if(!ui) return;

    // if(ui->INT_timer) lv_timer_pause(ui->INT_timer);    
    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();

    lv_group_t * back = ui->INT_grp_prev ? ui->INT_grp_prev : s_group;
    if(enc) lv_indev_set_group(enc, back);
    if(kp)  lv_indev_set_group(kp,  back);

    if(ui->INT_grp) {
        lv_group_del(ui->INT_grp);
        ui->INT_grp = NULL;
    }
    ui->INT_grp_prev = NULL;
    if(ui->INT_mask) {
        lv_obj_del(ui->INT_mask);
        ui->INT_mask = NULL;
    }

    ui->INT_panel = NULL;
    ui->INT_btn_close = NULL;
    eeprom_save_sys();
}

void Int_mask_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_target(e) != ui->INT_mask) return;
    Int_window_close(ui);
}

void Int_window_open(ui_strobe_t * ui)
{
    if(!ui) return;
    if(ui->INT_mask) return;

    lv_obj_t * scr = lv_screen_active();

    /* ================= Mask ================= */
    ui->INT_mask = lv_obj_create(scr);
    lv_obj_remove_style_all(ui->INT_mask);
    lv_obj_set_size(ui->INT_mask, DISP_W, DISP_H);
    lv_obj_set_pos(ui->INT_mask, 0, 0);
    lv_obj_set_style_bg_opa(ui->INT_mask, LV_OPA_80, 0);
    lv_obj_set_style_bg_color(ui->INT_mask, lv_color_black(), 0);
    lv_obj_add_flag(ui->INT_mask, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui->INT_mask, Int_mask_event_cb, LV_EVENT_CLICKED, ui);

    ui_styles_init_set_btn();
    /* ★ 중요: 스크롤로 엔코더 회전이 소비되는 것 방지 */
    lv_obj_clear_flag(ui->INT_mask, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Panel ================= */
    const lv_coord_t pw = (DISP_W * 2) / 3;
    const lv_coord_t ph = (DISP_H * 2) / 3;

    ui->INT_panel = lv_obj_create(ui->INT_mask);
    lv_obj_remove_style_all(ui->INT_panel);
    lv_obj_set_size(ui->INT_panel, pw, ph);
    lv_obj_center(ui->INT_panel);

    lv_obj_set_style_bg_opa(ui->INT_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(ui->INT_panel, lv_color_hex(0x0B1118), 0);
    lv_obj_set_style_radius(ui->INT_panel, 10, 0);
    lv_obj_set_style_border_width(ui->INT_panel, 2, 0);
    lv_obj_set_style_border_color(ui->INT_panel, lv_color_hex(0x2DE0C7), 0);
    lv_obj_set_style_pad_all(ui->INT_panel, 12, 0);
    lv_obj_set_style_pad_row(ui->INT_panel, 8, 0);

    /* panel은 세로 흐름: title / grid / close */
    lv_obj_set_flex_flow(ui->INT_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(ui->INT_panel, LV_OBJ_FLAG_EVENT_BUBBLE);

    /* ★ 중요: panel/content 스크롤 금지 */
    lv_obj_clear_flag(ui->INT_panel, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Title ================= */
    lv_obj_t * title = lv_label_create(ui->INT_panel);
    lv_label_set_text(title, "INTER");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    /* ================= Content (Buttons row + Range Slider) ================= */
    lv_obj_t * content = lv_obj_create(ui->INT_panel);
    lv_obj_remove_style_all(content);
    lv_obj_set_width(content, LV_PCT(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);

    /* 바깥 content는 세로로 쌓기 */
    lv_obj_set_layout(content, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content,
                      LV_FLEX_ALIGN_START,    /* main (세로) */
                      LV_FLEX_ALIGN_START,    /* cross (가로) */
                      LV_FLEX_ALIGN_START);

    lv_obj_set_style_pad_row(content, 10, 0);     /* 버튼줄/슬라이더줄 간격 */
    lv_obj_set_style_pad_all(content, 0, 0);

    /* ★ content 스크롤 금지 */
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Row: Buttons (one horizontal line) ================= */
    lv_obj_t * row_btns = lv_obj_create(content);
    lv_obj_remove_style_all(row_btns);
    lv_obj_set_width(row_btns, LV_PCT(100));
    lv_obj_set_height(row_btns, 34); /* 버튼 줄 높이 (원하는 값으로 조절) */
    lv_obj_set_style_bg_opa(row_btns, LV_OPA_TRANSP, 0);

    lv_obj_set_layout(row_btns, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row_btns, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row_btns,
                        LV_FLEX_ALIGN_START,   /* 가로 정렬 시작 */
                        LV_FLEX_ALIGN_CENTER,  /* 세로 중앙 */
                        LV_FLEX_ALIGN_CENTER);

    /* 버튼 간격 */
    lv_obj_set_style_pad_column(row_btns, 6, 0);

    /* (선택) 버튼이 많아 한 줄에 다 안 들어가면:
    *  - 줄바꿈하고 싶으면: LV_FLEX_FLOW_ROW_WRAP
    *  - 한 줄 고정 + 좌우 스크롤 원하면: row_btns에 스크롤 허용 + 수평 스크롤만
    */

    /* ===== 한 줄 고정 + 자동 줄바꿈 "없음" ===== */
    lv_obj_set_flex_flow(row_btns, LV_FLEX_FLOW_ROW);

    /* ===== (옵션A) 줄바꿈 허용 ===== */
    // lv_obj_set_flex_flow(row_btns, LV_FLEX_FLOW_ROW_WRAP);

    /* ===== (옵션B) 한 줄 고정 + 가로 스크롤 ===== */
    // lv_obj_add_flag(row_btns, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_scroll_dir(row_btns, LV_DIR_HOR);
    // lv_obj_set_scrollbar_mode(row_btns, LV_SCROLLBAR_MODE_OFF);

    /* 버튼 생성 (부모를 content -> row_btns 로 변경) */
    lv_obj_t * b_mode1 = mode_make_btn(row_btns, "1");
    lv_obj_t * b_mode2 = mode_make_btn(row_btns, "2");
    lv_obj_t * b_mode3 = mode_make_btn(row_btns, "4");
    lv_obj_t * b_mode4 = mode_make_btn(row_btns, "6");
    lv_obj_t * b_mode5 = mode_make_btn(row_btns, "8");

    /* 버튼 스타일은 그대로 */
    lv_obj_add_style(b_mode1, &g_st_set_btn, 0);
    lv_obj_add_style(b_mode1, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode1, &g_st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_mode2, &g_st_set_btn, 0);
    lv_obj_add_style(b_mode2, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode2, &g_st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_mode3, &g_st_set_btn, 0);
    lv_obj_add_style(b_mode3, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode3, &g_st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_mode4, &g_st_set_btn, 0);
    lv_obj_add_style(b_mode4, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode4, &g_st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_mode5, &g_st_set_btn, 0);
    lv_obj_add_style(b_mode5, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode5, &g_st_set_btn_focus, LV_STATE_PRESSED);

    if(ui->int_sel_idx <= 0 || ui->int_sel_idx > 8){
        ui->int_sel_idx = 1;
    }
    if(ui->int_sel_idx == 1){
        lv_obj_add_state(b_mode1, LV_STATE_CHECKED);
    }
    if(ui->int_sel_idx == 2){
        lv_obj_add_state(b_mode2, LV_STATE_CHECKED);
    }
    if(ui->int_sel_idx == 4){
        lv_obj_add_state(b_mode3, LV_STATE_CHECKED);
    }
    if(ui->int_sel_idx == 6){
        lv_obj_add_state(b_mode4, LV_STATE_CHECKED);
    }
    if(ui->int_sel_idx == 8){
        lv_obj_add_state(b_mode5, LV_STATE_CHECKED);
    }

    /* “작게” 만들기: 고정 크기 주기 */
    lv_obj_set_size(b_mode1, 46, 30);
    lv_obj_set_size(b_mode2, 46, 30);
    lv_obj_set_size(b_mode3, 46, 30);
    lv_obj_set_size(b_mode4, 46, 30);
    lv_obj_set_size(b_mode5, 46, 30);

    /* user_data / event_cb는 그대로 */
    lv_obj_set_user_data(b_mode1, (void*)(uintptr_t)0);
    lv_obj_set_user_data(b_mode2, (void*)(uintptr_t)1);
    lv_obj_set_user_data(b_mode3, (void*)(uintptr_t)2);
    lv_obj_set_user_data(b_mode4, (void*)(uintptr_t)3);
    lv_obj_set_user_data(b_mode5, (void*)(uintptr_t)4);

    lv_obj_add_event_cb(b_mode1, int_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_mode2, int_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_mode3, int_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_mode4, int_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_mode5, int_btn_event_cb, LV_EVENT_CLICKED, ui);

    /* ================= Row: START + END (same line) ================= */
    lv_obj_t * row_se = lv_obj_create(content);
    lv_obj_remove_style_all(row_se);
    lv_obj_set_width(row_se, LV_PCT(100));
    lv_obj_set_height(row_se, 34);
    lv_obj_set_style_bg_opa(row_se, LV_OPA_TRANSP, 0);

    lv_obj_set_layout(row_se, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row_se, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row_se,
                        LV_FLEX_ALIGN_SPACE_BETWEEN, /* 좌/우로 벌려 배치 */
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

    /* 왼쪽: START */
    lv_obj_t * left = lv_obj_create(row_se);
    lv_obj_remove_style_all(left);
    lv_obj_set_layout(left, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(left, 6, 0);

    lv_obj_t * lbl_s = lv_label_create(left);
    lv_label_set_text(lbl_s, "START");
    lv_obj_set_style_text_color(lbl_s, lv_color_white(), 0);

    ui->int_btn_start = mode_make_btn(left, "1");

    uint8_t start = g_seq_con.start_page;
    /* 값이 없거나 범위 밖이면 기본값 1 */
    if(start == 0 || start > 8) {
        start = 1;
    }
    ui->int_start_value = start;
    int_set_btn_text(ui->int_btn_start, start);

    lv_obj_set_size(ui->int_btn_start, 60, 30);
    lv_obj_add_style(ui->int_btn_start, &g_st_set_btn, 0);
    lv_obj_add_style(ui->int_btn_start, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(ui->int_btn_start, &g_st_set_btn_focus, LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui->int_btn_start, int_target_btn_event_cb, LV_EVENT_ALL, ui);

    /* 오른쪽: END */
    lv_obj_t * right = lv_obj_create(row_se);
    lv_obj_remove_style_all(right);
    lv_obj_set_layout(right, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(right, 6, 0);

    lv_obj_t * lbl_e = lv_label_create(right);
    lv_label_set_text(lbl_e, "END");
    lv_obj_set_style_text_color(lbl_e, lv_color_white(), 0);

    ui->int_btn_end = mode_make_btn(right, "8");

    uint8_t end = g_seq_con.end_page;

    /* 값이 없거나 범위 밖이면 기본값 8 */
    if(end == 0 || end > 8) {
        end = 8;
    }

    ui->int_end_value = end;
    int_set_btn_text(ui->int_btn_end, end);

    lv_obj_set_size(ui->int_btn_end, 60, 30);
    lv_obj_add_style(ui->int_btn_end, &g_st_set_btn, 0);
    lv_obj_add_style(ui->int_btn_end, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(ui->int_btn_end, &g_st_set_btn_focus, LV_STATE_PRESSED);
    lv_obj_add_event_cb(ui->int_btn_end, int_target_btn_event_cb, LV_EVENT_ALL, ui);

    /* 초기값 */
    ui->int_pick_target = 0;  // 기본: START 선택
    // ui->int_start_value = 1;
    // ui->int_end_value   = 8;
    int_set_btn_text(ui->int_btn_start, ui->int_start_value);
    int_set_btn_text(ui->int_btn_end,   ui->int_end_value);

    /* ================= Close Button ================= */
    ui->INT_btn_close = lv_btn_create(ui->INT_panel);
    lv_obj_set_size(ui->INT_btn_close, LV_PCT(100), 34);

    lv_obj_set_style_bg_color(ui->INT_btn_close, lv_color_hex(0x1C2B34), 0);
    lv_obj_set_style_bg_opa(ui->INT_btn_close, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ui->INT_btn_close, 6, 0);
    lv_obj_set_style_border_width(ui->INT_btn_close, 1, 0);
    lv_obj_set_style_border_color(ui->INT_btn_close, lv_color_hex(0x2DE0C7), 0);

    lv_obj_add_event_cb(ui->INT_btn_close, Int_close_btn_cb, LV_EVENT_CLICKED, ui);

    lv_obj_t * lbl = lv_label_create(ui->INT_btn_close);
    lv_label_set_text(lbl, "CLOSE");
    lv_obj_center(lbl);

    /* Setting 창과 동일한 CLOSE 톤/강조 */
    lv_obj_add_style(ui->INT_btn_close, &g_st_set_btn, 0);
    lv_obj_add_style(ui->INT_btn_close, &g_st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(ui->INT_btn_close, &g_st_set_btn_focus, LV_STATE_PRESSED);

    /* ================= Group (Encoder/Keypad) ================= */
    ui->INT_grp_prev = s_group;
    ui->INT_grp = lv_group_create();
    lv_group_set_wrap(ui->INT_grp, true);
    lv_group_set_editing(ui->INT_grp, false);

    /*default group 지정(다른 곳에서 키 처리가 꼬이는 것 방지) */
    lv_group_set_default(ui->INT_grp);

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();
    if(enc) lv_indev_set_group(enc, ui->INT_grp);
    if(kp)  lv_indev_set_group(kp,  ui->INT_grp);

    //잔류 입력 제거
    if(enc) lv_indev_wait_release(enc);
    if(kp)  lv_indev_wait_release(kp);

    lv_group_add_obj(ui->INT_grp, b_mode1);
    lv_group_add_obj(ui->INT_grp, b_mode2);
    lv_group_add_obj(ui->INT_grp, b_mode3);
    lv_group_add_obj(ui->INT_grp, b_mode4);
    lv_group_add_obj(ui->INT_grp, b_mode5);
    lv_group_add_obj(ui->INT_grp, ui->int_btn_start);
    lv_group_add_obj(ui->INT_grp, ui->int_btn_end);
    lv_group_add_obj(ui->INT_grp, ui->INT_btn_close);

    lv_group_focus_obj(b_mode1);
}

void int_btn_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    lv_obj_t * btn = lv_event_get_target(e);
    uint32_t m = (uint32_t)(uintptr_t)lv_obj_get_user_data(btn);

    uint8_t n_vlue = 0;
    if(m == 0){
        n_vlue = 1;
        ui->int_sel_idx = 1;
    }else if(m == 1){
        n_vlue = 2;
        ui->int_sel_idx = 2;
    }else if(m == 2){
        n_vlue = 4;
        ui->int_sel_idx = 4;
    }else if(m == 3){
        n_vlue = 6;
        ui->int_sel_idx = 6;
    }else if(m == 4){
        n_vlue = 8;
        ui->int_sel_idx = 8;
    }

    /* 이전 선택 버튼 체크 해제 */
    if(ui->int_btn_selected && ui->int_btn_selected != btn) {
        lv_obj_clear_state(ui->int_btn_selected, LV_STATE_CHECKED);
    }

    /* 현재 버튼 체크 */
    lv_obj_add_state(btn, LV_STATE_CHECKED);

    /*현재 버튼을 "선택됨"으로 기억 */
    ui->int_btn_selected = btn;

    /* INT 라벨 갱신 */
    if(ui->lbl_interlock) {
        char buf[16];
        // snprintf(buf, sizeof(buf), "INT: %u", m);
        snprintf(buf, sizeof(buf), "INT: %lu", (unsigned long)n_vlue);
        lv_label_set_text(ui->lbl_interlock, buf);
    }

    // Int_window_close(ui);
}

void Int_close_btn_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Int_window_close(ui);
    }
}

void int_set_btn_text(lv_obj_t * btn, uint8_t v)
{
    if(!btn) return;

    /* mode_make_btn()가
       [lv_btn]
         └─ [lv_label]  구조라고 가정 */
    lv_obj_t * lbl = lv_obj_get_child(btn, 0);
    if(!lbl) return;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%u", (unsigned)v);
    lv_label_set_text(lbl, buf);
}