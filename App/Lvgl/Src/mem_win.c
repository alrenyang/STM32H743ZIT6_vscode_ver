#include "mem_win.h"


static lv_style_t st_set_btn;        // 기본 버튼
static lv_style_t st_set_btn_focus;  // 포커스/선택 강조
static bool st_set_btn_inited = false;

static void setwin_init_styles(void)
{
    if(st_set_btn_inited) return;
    st_set_btn_inited = true;

    /* 기본 버튼 */
    lv_style_init(&st_set_btn);
    lv_style_set_radius(&st_set_btn, 6);
    lv_style_set_border_width(&st_set_btn, 1);
    lv_style_set_border_color(&st_set_btn, lv_color_hex(0x2DE0C7));
    lv_style_set_bg_opa(&st_set_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&st_set_btn, lv_color_hex(0x0B1118));
    lv_style_set_text_color(&st_set_btn, lv_color_white());
    lv_style_set_pad_hor(&st_set_btn, 8);
    lv_style_set_pad_ver(&st_set_btn, 4);

    /* 포커스/선택(강조) */
    lv_style_init(&st_set_btn_focus);
    lv_style_set_border_width(&st_set_btn_focus, 2);
    lv_style_set_border_color(&st_set_btn_focus, lv_color_hex(0xFFD54A)); /* 강조색 */
    lv_style_set_bg_opa(&st_set_btn_focus, LV_OPA_COVER);
    lv_style_set_bg_color(&st_set_btn_focus, lv_color_hex(0x12303A));     /* 살짝 밝게 */
    lv_style_set_text_color(&st_set_btn_focus, lv_color_white());
}

void Mem_window_close(ui_strobe_t * ui)
{
    if(!ui) return;

    // if(ui->INT_timer) lv_timer_pause(ui->INT_timer);    
    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();

    lv_group_t * back = ui->MEM_grp_prev ? ui->MEM_grp_prev : s_group;
    if(enc) lv_indev_set_group(enc, back);
    if(kp)  lv_indev_set_group(kp,  back);

    if(ui->MEM_grp) {
        lv_group_del(ui->MEM_grp);
        ui->MEM_grp = NULL;
    }
    ui->MEM_grp_prev = NULL;
    if(ui->MEM_mask) {
        lv_obj_del(ui->MEM_mask);
        ui->MEM_mask = NULL;
    }

    ui->MEM_panel = NULL;
    ui->MEM_btn_close = NULL;
}

void Mem_mask_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_target(e) != ui->MEM_mask) return;
    Mem_window_close(ui);
}

void Mem_window_open(ui_strobe_t * ui)
{
    if(!ui) return;
    if(ui->MEM_mask) return;

    lv_obj_t * scr = lv_screen_active();

    /* ================= Mask ================= */
    ui->MEM_mask = lv_obj_create(scr);
    lv_obj_remove_style_all(ui->MEM_mask);
    lv_obj_set_size(ui->MEM_mask, DISP_W, DISP_H);
    lv_obj_set_pos(ui->MEM_mask, 0, 0);
    lv_obj_set_style_bg_opa(ui->MEM_mask, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(ui->MEM_mask, lv_color_black(), 0);
    lv_obj_add_flag(ui->MEM_mask, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui->MEM_mask, Mem_mask_event_cb, LV_EVENT_CLICKED, ui);

    setwin_init_styles();
    /* ★ 중요: 스크롤로 엔코더 회전이 소비되는 것 방지 */
    lv_obj_clear_flag(ui->MEM_mask, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Panel ================= */
    const lv_coord_t pw = (DISP_W * 2) / 3;
    const lv_coord_t ph = (DISP_H * 2) / 3;

    ui->MEM_panel = lv_obj_create(ui->MEM_mask);
    lv_obj_remove_style_all(ui->MEM_panel);
    lv_obj_set_size(ui->MEM_panel, pw, ph);
    lv_obj_center(ui->MEM_panel);

    lv_obj_set_style_bg_opa(ui->MEM_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(ui->MEM_panel, lv_color_hex(0x0B1118), 0);
    lv_obj_set_style_radius(ui->MEM_panel, 10, 0);
    lv_obj_set_style_border_width(ui->MEM_panel, 2, 0);
    lv_obj_set_style_border_color(ui->MEM_panel, lv_color_hex(0x2DE0C7), 0);
    lv_obj_set_style_pad_all(ui->MEM_panel, 12, 0);
    lv_obj_set_style_pad_row(ui->MEM_panel, 8, 0);

    /* panel은 세로 흐름: title / grid / close */
    lv_obj_set_flex_flow(ui->MEM_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(ui->MEM_panel, LV_OBJ_FLAG_EVENT_BUBBLE);

    /* ★ 중요: panel/content 스크롤 금지 */
    lv_obj_clear_flag(ui->MEM_panel, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Title ================= */
    lv_obj_t * title = lv_label_create(ui->MEM_panel);
    lv_label_set_text(title, "MEMORY");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    // lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    /* ================= User block (2 rows) ================= */
    lv_obj_t * user_block = lv_obj_create(ui->MEM_panel);
    lv_obj_remove_style_all(user_block);
    lv_obj_set_width(user_block, LV_PCT(100));
    lv_obj_set_height(user_block, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(user_block, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(user_block, 10, 0);     // Save/Load 줄 간격

    /* ---------- Row 1: User Save ---------- */
    lv_obj_t * row_save = lv_obj_create(user_block);
    lv_obj_remove_style_all(row_save);
    lv_obj_set_width(row_save, LV_PCT(100));
    lv_obj_set_height(row_save, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row_save, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row_save,
                        LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row_save, 12, 0);    // 라벨-버튼 간격

    lv_obj_t * lbl_save = lv_label_create(row_save);
    lv_label_set_text(lbl_save, "User Save");
    lv_obj_set_style_text_color(lbl_save, lv_color_white(), 0);
    /* 라벨 폭을 고정하면 버튼 정렬이 깔끔해짐(권장) */
    lv_obj_set_width(lbl_save, 90);

    lv_obj_t * line_save = lv_obj_create(row_save);
    lv_obj_remove_style_all(line_save);
    lv_obj_set_height(line_save, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(line_save, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(line_save, 8, 0);
    lv_obj_set_flex_grow(line_save, 1);

    lv_obj_t * b_save1 = mode_make_btn(line_save, "0");
    lv_obj_t * b_save2 = mode_make_btn(line_save, "1");
    lv_obj_t * b_save3 = mode_make_btn(line_save, "2");

    lv_obj_set_width(b_save1, 48);
    lv_obj_set_width(b_save2, 48);
    lv_obj_set_width(b_save3, 48);

    lv_obj_add_style(b_save1, &st_set_btn, 0);
    lv_obj_add_style(b_save1, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_save1, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_save2, &st_set_btn, 0);
    lv_obj_add_style(b_save2, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_save2, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_save3, &st_set_btn, 0);
    lv_obj_add_style(b_save3, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_save3, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_set_user_data(b_save1, (void *)(intptr_t)0);
    lv_obj_set_user_data(b_save2, (void *)(intptr_t)1);
    lv_obj_set_user_data(b_save3, (void *)(intptr_t)2);

    lv_obj_add_event_cb(b_save1, mem_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_save2, mem_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_save3, mem_btn_event_cb, LV_EVENT_CLICKED, ui);


    /* ---------- Row 2: User Load ---------- */
    lv_obj_t * row_load = lv_obj_create(user_block);
    lv_obj_remove_style_all(row_load);
    lv_obj_set_width(row_load, LV_PCT(100));
    lv_obj_set_height(row_load, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row_load, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row_load,
                        LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row_load, 12, 0);

    lv_obj_t * lbl_load = lv_label_create(row_load);
    lv_label_set_text(lbl_load, "User Load");
    lv_obj_set_style_text_color(lbl_load, lv_color_white(), 0);
    lv_obj_set_width(lbl_load, 90);   // Save와 동일 폭

    lv_obj_t * line_load = lv_obj_create(row_load);
    lv_obj_remove_style_all(line_load);
    lv_obj_set_height(line_load, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(line_load, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(line_load, 8, 0);
    lv_obj_set_flex_grow(line_load, 1);

    lv_obj_t * b_load1 = mode_make_btn(line_load, "0");
    lv_obj_t * b_load2 = mode_make_btn(line_load, "1");
    lv_obj_t * b_load3 = mode_make_btn(line_load, "2");

    lv_obj_set_width(b_load1, 48);
    lv_obj_set_width(b_load2, 48);
    lv_obj_set_width(b_load3, 48);

    lv_obj_add_style(b_load1, &st_set_btn, 0);
    lv_obj_add_style(b_load1, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_load1, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_load2, &st_set_btn, 0);
    lv_obj_add_style(b_load2, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_load2, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_load3, &st_set_btn, 0);
    lv_obj_add_style(b_load3, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_load3, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_set_user_data(b_load1, (void *)(intptr_t)3);
    lv_obj_set_user_data(b_load2, (void *)(intptr_t)4);
    lv_obj_set_user_data(b_load3, (void *)(intptr_t)5);

    lv_obj_add_event_cb(b_load1, mem_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_load2, mem_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_load3, mem_btn_event_cb, LV_EVENT_CLICKED, ui);

    /* ================= Button: CLOSE only ================= */
    lv_obj_t * row_btn = lv_obj_create(ui->MEM_panel);
    lv_obj_remove_style_all(row_btn);
    lv_obj_set_width(row_btn, LV_PCT(100));
    lv_obj_set_height(row_btn, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row_btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row_btn, 0, 0);
    lv_obj_set_style_margin_top(row_btn, 25, 0);   // 숫자 조절

    ui->btn_close = mode_make_btn(row_btn, "CLOSE");
    lv_obj_add_event_cb(ui->btn_close,mem_close_event_cb, LV_EVENT_CLICKED, ui);

    lv_obj_add_style(ui->btn_close, &st_set_btn, 0);
    lv_obj_add_style(ui->btn_close, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(ui->btn_close, &st_set_btn_focus, LV_STATE_PRESSED);

    /* ================= Group (Encoder / Keypad) ================= */
    ui->MEM_grp_prev = s_group;
    ui->MEM_grp = lv_group_create();
    lv_group_set_wrap(ui->MEM_grp, true);
    lv_group_set_editing(ui->MEM_grp, false);
    lv_group_set_default(ui->MEM_grp);

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();
    if(enc) lv_indev_set_group(enc, ui->MEM_grp);
    if(kp)  lv_indev_set_group(kp,  ui->MEM_grp);
    if(enc) lv_indev_wait_release(enc);
    if(kp)  lv_indev_wait_release(kp);

    lv_group_add_obj(ui->MEM_grp, b_save1);
    lv_group_add_obj(ui->MEM_grp, b_save2);
    lv_group_add_obj(ui->MEM_grp, b_save3);
    lv_group_add_obj(ui->MEM_grp, b_load1);
    lv_group_add_obj(ui->MEM_grp, b_load2);
    lv_group_add_obj(ui->MEM_grp, b_load3);
    lv_group_add_obj(ui->MEM_grp, ui->btn_close);

    lv_group_focus_obj(b_save1);
}

void mem_close_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    /* 닫기 함수가 있다면 호출 */
    // widgets_update_ip_label(ui);
    Mem_window_close(ui);
}

void mem_btn_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    lv_obj_t * btn = lv_event_get_target(e);
    uint32_t m = (uint32_t)(uintptr_t)lv_obj_get_user_data(btn);

    uint8_t n_vlue = 0;
    if(m == 0){
        n_vlue = 0;
    }else if(m == 1){
        n_vlue = 1;
    }else if(m == 2){
        n_vlue = 2;
    }else if(m == 3){
        n_vlue = 0;
    }else if(m == 4){
        n_vlue = 1;
    }else if(m == 5){
        n_vlue = 2;
    }

    /* MEM 라벨 갱신 */
    if(ui->lbl_mem) {
        char buf[16];
        snprintf(buf, sizeof(buf), "MEM: %u", n_vlue);
        lv_label_set_text(ui->lbl_mem, buf);
    }

    // lv_label_set_text(ui->lbl_mem, "MEM: 0");
    // /* INT 라벨 갱신 */
    // if(ui->lbl_interlock) {
    //     char buf[16];
    //     // snprintf(buf, sizeof(buf), "INT: %u", m);
    //     snprintf(buf, sizeof(buf), "INT: %lu", (unsigned long)n_vlue);
    //     lv_label_set_text(ui->lbl_interlock, buf);
    // }


    Mem_window_close(ui);
}