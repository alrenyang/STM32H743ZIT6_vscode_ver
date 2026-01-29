#include "mod_win.h"


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


void mode_start_btn_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    Mode_window_open(ui);
}

void Mode_window_open(ui_strobe_t * ui)
{
    if(!ui) return;
    if(ui->MODE_mask) return;

    lv_obj_t * scr = lv_screen_active();

    /* ================= Mask ================= */
    ui->MODE_mask = lv_obj_create(scr);
    lv_obj_remove_style_all(ui->MODE_mask);
    lv_obj_set_size(ui->MODE_mask, DISP_W, DISP_H);
    lv_obj_set_pos(ui->MODE_mask, 0, 0);
    lv_obj_set_style_bg_opa(ui->MODE_mask, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(ui->MODE_mask, lv_color_black(), 0);
    lv_obj_add_flag(ui->MODE_mask, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui->MODE_mask, Mode_mask_event_cb, LV_EVENT_CLICKED, ui);

    setwin_init_styles();
    /* ★ 중요: 스크롤로 엔코더 회전이 소비되는 것 방지 */
    lv_obj_clear_flag(ui->MODE_mask, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Panel ================= */
    const lv_coord_t pw = (DISP_W * 2) / 3;
    const lv_coord_t ph = (DISP_H * 2) / 3;

    ui->MODE_panel = lv_obj_create(ui->MODE_mask);
    lv_obj_remove_style_all(ui->MODE_panel);
    lv_obj_set_size(ui->MODE_panel, pw, ph);
    lv_obj_center(ui->MODE_panel);

    lv_obj_set_style_bg_opa(ui->MODE_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(ui->MODE_panel, lv_color_hex(0x0B1118), 0);
    lv_obj_set_style_radius(ui->MODE_panel, 10, 0);
    lv_obj_set_style_border_width(ui->MODE_panel, 2, 0);
    lv_obj_set_style_border_color(ui->MODE_panel, lv_color_hex(0x2DE0C7), 0);
    lv_obj_set_style_pad_all(ui->MODE_panel, 12, 0);
    lv_obj_set_style_pad_row(ui->MODE_panel, 8, 0);

    /* panel은 세로 흐름: title / grid / close */
    lv_obj_set_flex_flow(ui->MODE_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(ui->MODE_panel, LV_OBJ_FLAG_EVENT_BUBBLE);

    /* ★ 중요: panel/content 스크롤 금지 */
    lv_obj_clear_flag(ui->MODE_panel, LV_OBJ_FLAG_SCROLLABLE);

    /* ================= Title ================= */
    lv_obj_t * title = lv_label_create(ui->MODE_panel);
    lv_label_set_text(title, "MODE");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);

    /* ================= Content (GRID 2x2) ================= */
    lv_obj_t * content = lv_obj_create(ui->MODE_panel);
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
    static lv_coord_t row_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

    lv_obj_set_layout(content, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(content, col_dsc, row_dsc);

    /* ================= Buttons ================= */
    lv_obj_t * b_mode1 = mode_make_btn(content, "Trigger");
    lv_obj_t * b_mode2 = mode_make_btn(content, "Sequence");
    lv_obj_t * b_mode3 = mode_make_btn(content, "RS232");
    lv_obj_t * b_mode4 = mode_make_btn(content, "Ethernet");

    /* Setting 창과 동일한 버튼 톤/강조 */
    lv_obj_add_style(b_mode1, &st_set_btn, 0);
    lv_obj_add_style(b_mode1, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode1, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_mode2, &st_set_btn, 0);
    lv_obj_add_style(b_mode2, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode2, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_mode3, &st_set_btn, 0);
    lv_obj_add_style(b_mode3, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode3, &st_set_btn_focus, LV_STATE_PRESSED);

    lv_obj_add_style(b_mode4, &st_set_btn, 0);
    lv_obj_add_style(b_mode4, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(b_mode4, &st_set_btn_focus, LV_STATE_PRESSED);

    /* grid 셀에 맞게 강제(※ mode_make_btn 안에서 height=28 고정이라 덮어씀) */
    lv_obj_set_size(b_mode1, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(b_mode2, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(b_mode3, LV_PCT(100), LV_PCT(100));
    lv_obj_set_size(b_mode4, LV_PCT(100), LV_PCT(100));

    lv_obj_set_grid_cell(b_mode1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1); // (0,0)
    lv_obj_set_grid_cell(b_mode2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1); // (1,0)
    lv_obj_set_grid_cell(b_mode3, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1); // (0,1)
    lv_obj_set_grid_cell(b_mode4, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1); // (1,1)

    /* user_data */
    lv_obj_set_user_data(b_mode1, (void*)(uintptr_t)0);
    lv_obj_set_user_data(b_mode2, (void*)(uintptr_t)1);
    lv_obj_set_user_data(b_mode3, (void*)(uintptr_t)2);
    lv_obj_set_user_data(b_mode4, (void*)(uintptr_t)3);

    lv_obj_add_event_cb(b_mode1, mode_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_mode2, mode_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_mode3, mode_btn_event_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(b_mode4, mode_btn_event_cb, LV_EVENT_CLICKED, ui);

    /* ================= Close Button ================= */
    ui->MODE_btn_close = lv_btn_create(ui->MODE_panel);
    lv_obj_set_size(ui->MODE_btn_close, LV_PCT(100), 34);

    lv_obj_set_style_bg_color(ui->MODE_btn_close, lv_color_hex(0x1C2B34), 0);
    lv_obj_set_style_bg_opa(ui->MODE_btn_close, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ui->MODE_btn_close, 6, 0);
    lv_obj_set_style_border_width(ui->MODE_btn_close, 1, 0);
    lv_obj_set_style_border_color(ui->MODE_btn_close, lv_color_hex(0x2DE0C7), 0);

    lv_obj_add_event_cb(ui->MODE_btn_close, Mode_close_btn_cb, LV_EVENT_CLICKED, ui);

    lv_obj_t * lbl = lv_label_create(ui->MODE_btn_close);
    lv_label_set_text(lbl, "CLOSE");
    lv_obj_center(lbl);

    /* Setting 창과 동일한 CLOSE 톤/강조 */
    lv_obj_add_style(ui->MODE_btn_close, &st_set_btn, 0);
    lv_obj_add_style(ui->MODE_btn_close, &st_set_btn_focus, LV_STATE_FOCUSED);
    lv_obj_add_style(ui->MODE_btn_close, &st_set_btn_focus, LV_STATE_PRESSED);

    /* ================= Group (Encoder/Keypad) ================= */
    ui->mode_grp_prev = s_group;
    ui->mode_grp = lv_group_create();
    lv_group_set_wrap(ui->mode_grp, true);
    lv_group_set_editing(ui->mode_grp, false);

    /*default group 지정(다른 곳에서 키 처리가 꼬이는 것 방지) */
    lv_group_set_default(ui->mode_grp);

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();
    if(enc) lv_indev_set_group(enc, ui->mode_grp);
    if(kp)  lv_indev_set_group(kp,  ui->mode_grp);

    //잔류 입력 제거
    if(enc) lv_indev_wait_release(enc);
    if(kp)  lv_indev_wait_release(kp);

    lv_group_add_obj(ui->mode_grp, b_mode1);
    lv_group_add_obj(ui->mode_grp, b_mode2);
    lv_group_add_obj(ui->mode_grp, b_mode3);
    lv_group_add_obj(ui->mode_grp, b_mode4);
    lv_group_add_obj(ui->mode_grp, ui->MODE_btn_close);

    /* 포커스 초기화 */
    if(g_oper_mode == 0){
        lv_group_focus_obj(b_mode1);
    }else if(g_oper_mode == 1){
        lv_group_focus_obj(b_mode2);
    }else if (g_oper_mode == 2) {
        lv_group_focus_obj(b_mode3);
    }else if (g_oper_mode == 3) {
        lv_group_focus_obj(b_mode4);
    }
    
    
}

void Mode_mask_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_target(e) != ui->MODE_mask) return;
    Mode_window_close(ui);
}

void Mode_window_close(ui_strobe_t * ui)
{
    if(!ui) return;

    if(ui->mode_timer) lv_timer_pause(ui->mode_timer);

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();
    lv_group_t * back = ui->mode_grp_prev ? ui->mode_grp_prev : s_group;

    if(enc) lv_indev_set_group(enc, back);
    if(kp)  lv_indev_set_group(kp,  back);

    if(ui->mode_grp) {
        lv_group_del(ui->mode_grp);
        ui->mode_grp = NULL;
    }
    ui->mode_grp_prev = NULL;

    if(ui->MODE_mask) {
        lv_obj_del(ui->MODE_mask);
        ui->MODE_mask = NULL;
    }

    ui->MODE_panel = NULL;
    ui->MODE_btn_close = NULL;
}

void mode_btn_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    lv_obj_t * btn = lv_event_get_target(e);
    uint32_t m = (uint32_t)(uintptr_t)lv_obj_get_user_data(btn);
    // g_oper_mode = (int)m;

    // if(m < LV_ARRAY_SIZE(mode_str_tbl)) {
        g_oper_mode = (int)m;
        lv_label_set_text(ui->lbl_mode, mode_str_tbl[m]);
    // }


    Mode_window_close(ui);
}

void Mode_close_btn_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Mode_window_close(ui);
    }
}
