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

    /* ================= Row: User save ================= */
    lv_obj_t * user_save = lv_obj_create(ui->MEM_panel);
    lv_obj_remove_style_all(user_save);
    lv_obj_set_width(user_save, LV_PCT(100));
    lv_obj_set_height(user_save, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(user_save, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(user_save, 0, 0);
    lv_obj_set_style_pad_row(user_save, 6, 0);   // 라벨-버튼 간격
    lv_obj_set_style_pad_column(user_save, 6, 0);

    lv_obj_t * lbl_user = lv_label_create(user_save);
    lv_label_set_text(lbl_user, "User Save");
    lv_obj_set_style_text_color(lbl_user, lv_color_white(), 0);

    /* 버튼 라인 */
    lv_obj_t * save_line = lv_obj_create(user_save);
    lv_obj_remove_style_all(save_line);
    lv_obj_set_width(save_line, LV_PCT(100));
    lv_obj_set_height(save_line, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(save_line, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_row(save_line, 6, 0);
    lv_obj_set_style_pad_column(save_line, 8, 0);

    for(int i = 0; i < 3; i++) {
    }
}