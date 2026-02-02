#include "int_win.h"
#include <stdint.h>

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

    /* ================= Content (GRID 2x2) ================= */
    lv_obj_t * content = lv_obj_create(ui->INT_panel);
    lv_obj_remove_style_all(content);
    lv_obj_set_width(content, LV_PCT(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_row(content, 8, 0);
    lv_obj_set_style_pad_column(content, 8, 0);

    /* ★ 중요: content 스크롤 금지 */
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

     /* 2 columns, 2 rows */
    static lv_coord_t col_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static lv_coord_t row_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST };

    lv_obj_set_layout(content, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(content, col_dsc, row_dsc);

    /* ================= Buttons ================= */
    lv_obj_t * b_mode1 = mode_make_btn(content, "1");
    lv_obj_t * b_mode2 = mode_make_btn(content, "2");
    lv_obj_t * b_mode3 = mode_make_btn(content, "4");
    lv_obj_t * b_mode4 = mode_make_btn(content, "6");
    lv_obj_t * b_mode5 = mode_make_btn(content, "8");

    /* Setting 창과 동일한 버튼 톤/강조 */
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

    lv_obj_set_size(b_mode1, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(b_mode2, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(b_mode3, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(b_mode4, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(b_mode5, LV_PCT(100), LV_PCT(100));

    lv_obj_set_grid_cell(b_mode1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1); // (0,0)
    lv_obj_set_grid_cell(b_mode2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1); // (1,0)
    lv_obj_set_grid_cell(b_mode3, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1); // (0,1)
    lv_obj_set_grid_cell(b_mode4, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1); // (1,1)
    lv_obj_set_grid_cell(b_mode5, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1); // (0,2)

    /* user_data */
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
    }else if(m == 1){
        n_vlue = 2;
    }else if(m == 2){
        n_vlue = 4;
    }else if(m == 3){
        n_vlue = 6;
    }else if(m == 4){
        n_vlue = 8;
    }

    /* INT 라벨 갱신 */
    if(ui->lbl_interlock) {
        char buf[16];
        // snprintf(buf, sizeof(buf), "INT: %u", m);
        snprintf(buf, sizeof(buf), "INT: %lu", (unsigned long)n_vlue);
        lv_label_set_text(ui->lbl_interlock, buf);
    }


    Int_window_close(ui);
}

void Int_close_btn_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Int_window_close(ui);
    }
}