#include "ch_win.h"

void CH_open(ui_strobe_t * ui)
{
    if(!ui) return;
    if(ui->CH_panel_mask) return;

    lv_obj_t * scr = lv_screen_active();

    ui->CH_panel_mask = lv_obj_create(scr);
    lv_obj_remove_style_all(ui->CH_panel_mask);
    lv_obj_set_size(ui->CH_panel_mask, DISP_W, DISP_H);
    lv_obj_set_pos(ui->CH_panel_mask, 0, 0);
    lv_obj_set_style_bg_opa(ui->CH_panel_mask, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(ui->CH_panel_mask, lv_color_black(), 0);
    lv_obj_add_flag(ui->CH_panel_mask, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui->CH_panel_mask, CHPannel_mask_event_cb, LV_EVENT_CLICKED, ui);

    const lv_coord_t pw = (DISP_W * 2) / 3;
    const lv_coord_t ph = (DISP_H * 2) / 3;

    ui->CH_panel = lv_obj_create(ui->CH_panel_mask);
    lv_obj_remove_style_all(ui->CH_panel);
    lv_obj_set_size(ui->CH_panel, pw, ph);
    lv_obj_center(ui->CH_panel);

    lv_obj_set_style_bg_opa(ui->CH_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(ui->CH_panel, lv_color_hex(0x0B1118), 0);
    lv_obj_set_style_radius(ui->CH_panel, 10, 0);
    lv_obj_set_style_border_width(ui->CH_panel, 2, 0);
    lv_obj_set_style_border_color(ui->CH_panel, lv_color_hex(0x2DE0C7), 0);
    lv_obj_set_style_pad_all(ui->CH_panel, 12, 0);
    lv_obj_set_style_pad_row(ui->CH_panel, 8, 0);
    lv_obj_set_flex_flow(ui->CH_panel, LV_FLEX_FLOW_COLUMN);

    lv_obj_clear_flag(ui->CH_panel, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * header = lv_obj_create(ui->CH_panel);
    lv_obj_remove_style_all(header);
    lv_obj_set_width(header, LV_PCT(100));
    lv_obj_set_height(header, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_row(header, 6, 0);

    lv_obj_t * hdr_top = lv_obj_create(header);
    lv_obj_remove_style_all(hdr_top);
    lv_obj_set_width(hdr_top, LV_PCT(100));
    lv_obj_set_height(hdr_top, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(hdr_top, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hdr_top,
                          LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(hdr_top, LV_OPA_TRANSP, 0);

    ui->ch_title = lv_label_create(hdr_top);
    lv_label_set_text(ui->ch_title, "CH");
    lv_obj_set_style_text_color(ui->ch_title, lv_color_hex(0x2DE0C7), 0);
    lv_obj_set_style_text_font(ui->ch_title, &lv_font_montserrat_16, 0);

    ui->chk_save_all = lv_checkbox_create(hdr_top);
    lv_checkbox_set_text(ui->chk_save_all, "Save all");

    lv_obj_set_style_bg_opa(ui->chk_save_all, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui->chk_save_all, 0, 0);
    lv_obj_set_style_pad_all(ui->chk_save_all, 0, 0);
    lv_obj_set_style_pad_column(ui->chk_save_all, 6, 0);

    lv_obj_set_style_text_font(ui->chk_save_all, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(ui->chk_save_all, lv_color_hex(0xCFEFF0), 0);

    lv_obj_set_style_border_color(ui->chk_save_all, lv_color_hex(0x2DE0C7), LV_PART_INDICATOR);
    lv_obj_set_style_border_width(ui->chk_save_all, 2, LV_PART_INDICATOR);
    lv_obj_set_style_radius(ui->chk_save_all, 4, LV_PART_INDICATOR);

    lv_obj_set_style_bg_color(ui->chk_save_all, lv_color_hex(0x2DE0C7), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui->chk_save_all, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_set_style_outline_width(ui->chk_save_all, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_pad(ui->chk_save_all, 3, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(ui->chk_save_all, lv_color_hex(0x2DE0C7), LV_STATE_FOCUSED);
    lv_obj_set_style_outline_opa(ui->chk_save_all, LV_OPA_COVER, LV_STATE_FOCUSED);

    lv_obj_set_style_text_color(ui->chk_save_all, lv_color_hex(0xFFFFFF), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ui->chk_save_all, 3, LV_PART_INDICATOR | LV_STATE_FOCUSED);

    lv_obj_set_style_bg_opa(ui->chk_save_all, LV_OPA_20, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ui->chk_save_all, lv_color_hex(0x2DE0C7), LV_STATE_FOCUSED);

    /* 체크 상태는 유지하고 싶으면 여기 제거하고 ui->save_all을 기반으로 세팅 */
    lv_obj_clear_state(ui->chk_save_all, LV_STATE_CHECKED);
    ui->save_all = false;

    lv_obj_add_event_cb(ui->chk_save_all, chk_save_all_event_cb, LV_EVENT_VALUE_CHANGED, ui);

    lv_obj_t * hdr_cols = lv_obj_create(header);
    lv_obj_remove_style_all(hdr_cols);
    lv_obj_set_width(hdr_cols, LV_PCT(100));
    lv_obj_set_height(hdr_cols, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(hdr_cols, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_opa(hdr_cols, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_column(hdr_cols, 8, 0);

    static const char * titles[4] = {"On", "Delay", "Block", "Trg"};
    for(int i=0; i<4; i++){
        lv_obj_t * t = lv_label_create(hdr_cols);
        lv_label_set_text(t, titles[i]);
        lv_obj_set_style_text_color(t, lv_color_hex(0x2DE0C7), 0);
        lv_obj_set_style_text_font(t, &lv_font_montserrat_14, 0);
        lv_obj_set_flex_grow(t, 1);
        lv_obj_set_style_text_align(t, LV_TEXT_ALIGN_CENTER, 0);
    }

    lv_obj_t * list = lv_obj_create(ui->CH_panel);
    lv_obj_remove_style_all(list);
    lv_obj_set_width(list, LV_PCT(100));
    lv_obj_set_flex_grow(list, 1);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_column(list, 8, 0);

    lv_obj_t * b0 = ch_make_item(ui, list, "On",    CH_F_ON);
    lv_obj_t * b1 = ch_make_item(ui, list, "Delay", CH_F_DELAY);
    lv_obj_t * b2 = ch_make_item(ui, list, "Block", CH_F_BLOCK);
    lv_obj_t * b3 = ch_make_item(ui, list, "Trg",   CH_F_TRG);

    lv_obj_add_event_cb(b0, ch_item_event_cb, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(b1, ch_item_event_cb, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(b2, ch_item_event_cb, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(b3, ch_item_event_cb, LV_EVENT_ALL, ui);

    ui->CH_btn_close = lv_btn_create(ui->CH_panel);
    lv_obj_set_size(ui->CH_btn_close, LV_PCT(100), 34);

    lv_obj_set_style_bg_color(ui->CH_btn_close, lv_color_hex(0x1C2B34), 0);
    lv_obj_set_style_bg_opa(ui->CH_btn_close, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ui->CH_btn_close, 6, 0);
    lv_obj_set_style_border_width(ui->CH_btn_close, 1, 0);
    lv_obj_set_style_border_color(ui->CH_btn_close, lv_color_hex(0x2DE0C7), 0);

    lv_obj_add_event_cb(ui->CH_btn_close, CHPannel_close_btn_cb, LV_EVENT_CLICKED, ui);

    lv_obj_t * lbl = lv_label_create(ui->CH_btn_close);
    lv_label_set_text(lbl, "CLOSE");
    lv_obj_center(lbl);

    ui->ch_grp_prev = s_group;
    ui->ch_grp = lv_group_create();
    lv_group_set_wrap(ui->ch_grp, true);
    lv_group_set_editing(ui->ch_grp, false);

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();
    if(enc) lv_indev_set_group(enc, ui->ch_grp);
    if(kp)  lv_indev_set_group(kp,  ui->ch_grp);

    lv_group_add_obj(ui->ch_grp, ui->chk_save_all);
    lv_group_add_obj(ui->ch_grp, b0);
    lv_group_add_obj(ui->ch_grp, b1);
    lv_group_add_obj(ui->ch_grp, b2);
    lv_group_add_obj(ui->ch_grp, b3);
    lv_group_add_obj(ui->ch_grp, ui->CH_btn_close);

    lv_group_focus_obj(ui->chk_save_all);

    if(!ui->ch_timer) {
        ui->ch_timer = lv_timer_create(ch_edit_timer_cb, 5, ui);
    } else {
        lv_timer_resume(ui->ch_timer);
        lv_timer_set_period(ui->ch_timer, 5);
    }

    ch_panel_refresh(ui);
}

void table_build(ui_strobe_t * ui, lv_obj_t * parent)
{
    if(!ui || !parent) return;

    ui->tbl_root = parent;

    lv_obj_clean(parent);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(parent, 8, 0);
    lv_obj_set_style_pad_row(parent, 6, 0);

    ui->tbl_hdr = lv_obj_create(parent);
    lv_obj_remove_style_all(ui->tbl_hdr);
    lv_obj_set_width(ui->tbl_hdr, LV_PCT(100));
    lv_obj_set_height(ui->tbl_hdr, 20);
    lv_obj_set_style_bg_opa(ui->tbl_hdr, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_column(ui->tbl_hdr, 6, 0);
    lv_obj_set_flex_flow(ui->tbl_hdr, LV_FLEX_FLOW_ROW);

    const lv_coord_t w_ch=52, w_on=56, w_delay=72, w_block=72, w_trg=40;
    lv_coord_t widths[TBL_COLS] = {w_ch, w_on, w_delay, w_block, w_trg};

    table_cell_label(ui->tbl_hdr, "CH",    C_TEAL_TX, LV_TEXT_ALIGN_LEFT,   widths[0]);
    table_cell_label(ui->tbl_hdr, "On",    C_TEAL_TX, LV_TEXT_ALIGN_CENTER, widths[1]);
    table_cell_label(ui->tbl_hdr, "Delay", C_TEAL_TX, LV_TEXT_ALIGN_CENTER, widths[2]);
    table_cell_label(ui->tbl_hdr, "Block", C_TEAL_TX, LV_TEXT_ALIGN_CENTER, widths[3]);
    table_cell_label(ui->tbl_hdr, "Trg",   C_TEAL_TX, LV_TEXT_ALIGN_CENTER, widths[4]);

    ui->tbl_body = lv_obj_create(parent);
    lv_obj_remove_style_all(ui->tbl_body);
    lv_obj_set_width(ui->tbl_body, LV_PCT(100));
    lv_obj_set_height(ui->tbl_body, 0);
    lv_obj_set_flex_grow(ui->tbl_body, 1);
    lv_obj_set_style_bg_opa(ui->tbl_body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_row(ui->tbl_body, 4, 0);
    lv_obj_set_flex_flow(ui->tbl_body, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(ui->tbl_body, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(ui->tbl_body, LV_SCROLLBAR_MODE_AUTO);

    ui->tbl_cell_btn = (lv_obj_t ***)lv_malloc(sizeof(lv_obj_t **) * ui->ch_count);
    ui->tbl_cell_lbl = (lv_obj_t ***)lv_malloc(sizeof(lv_obj_t **) * ui->ch_count);

    ui->tbl_row = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *) * ui->ch_count);
    ui->sel_row = 0;

    for(uint16_t r=0; r<ui->ch_count; r++){
        ui->tbl_cell_btn[r] = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *) * TBL_COLS);
        ui->tbl_cell_lbl[r] = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *) * TBL_COLS);

        lv_obj_t * row = lv_obj_create(ui->tbl_body);
        lv_obj_remove_style_all(row);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_width(row, LV_PCT(100));

        /* row 높이는 cell(26)보다 크게 (클리핑 방지) */
        lv_obj_set_height(row, 28);

        lv_obj_set_style_pad_column(row, 6, 0);
        lv_obj_set_style_margin_top(row, 2, 0);
        lv_obj_set_style_pad_left(row, 2, 0);
        lv_obj_set_style_pad_right(row, 2, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);

        ui->tbl_row[r] = row;
        lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
        lv_obj_add_flag(row, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_event_cb(row, table_row_event_cb, LV_EVENT_ALL, ui);

        lv_obj_add_style(row, &g_st_row, 0);
        lv_obj_add_style(row, &g_st_row_focus, LV_STATE_FOCUSED);

        for(uint16_t c=0; c<TBL_COLS; c++){
            lv_obj_t * btn = table_cell_btn(row, "", (c==0)?LV_TEXT_ALIGN_LEFT:LV_TEXT_ALIGN_CENTER, widths[c]);
            lv_obj_t * lbl = table_btn_label(btn);

            lv_obj_set_style_text_color(lbl, (c==0) ? C_TEAL_TX : C_TX, 0);

            ui->tbl_cell_btn[r][c] = btn;
            ui->tbl_cell_lbl[r][c] = lbl;

            table_format_cell(ui, r, c);
        }
    }
}

lv_obj_t * table_cell_label(lv_obj_t * parent, const char * txt, lv_color_t col, lv_text_align_t align, lv_coord_t w)
{
    lv_obj_t * l = lv_label_create(parent);
    lv_label_set_text(l, txt);
    lv_obj_set_width(l, w);
    lv_obj_set_style_text_color(l, col, 0);
    lv_obj_set_style_text_align(l, align, 0);
    lv_label_set_long_mode(l, LV_LABEL_LONG_CLIP);
    return l;
}

void table_row_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    lv_obj_t * row = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(!ui) return;

    if(code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED || code == LV_EVENT_KEY) {
        for(uint16_t r=0; r<ui->ch_count; r++){
            if(ui->tbl_row[r] == row){
                ui->sel_row = r;
                break;
            }
        }
    }

    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_ENTER) {
            CH_open(ui);
        }
        return;
    }

    if(code == LV_EVENT_CLICKED) {
        uint32_t key = lv_event_get_key(e);
        if(key == 0 || key == LV_KEY_ENTER) {
            CH_open(ui);
        }
        return;
    }
}

lv_obj_t * table_btn_label(lv_obj_t * btn)
{
    return lv_obj_get_child(btn, 0);
}

lv_obj_t * table_cell_btn(lv_obj_t * parent, const char * txt, lv_text_align_t align, lv_coord_t w)
{
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, w, 26);
    lv_obj_set_style_radius(btn, 4, 0);
    lv_obj_set_style_pad_all(btn, 2, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_TRANSP, 0);

    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, txt);
    lv_obj_set_width(lbl, LV_PCT(100));
    lv_obj_set_style_text_align(lbl, align, 0);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_CLIP);
    lv_obj_center(lbl);

    return btn;
}

void CHPannel_mask_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        CHPannel_close(ui);
    }
}

void chk_save_all_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    lv_obj_t * chk = lv_event_get_target(e);
    ui->save_all = lv_obj_has_state(chk, LV_STATE_CHECKED);
}

lv_obj_t * ch_make_item(ui_strobe_t * ui, lv_obj_t * parent, const char * name, int idx)
{
    (void)name;

    lv_obj_t * btn = lv_btn_create(parent);

    lv_obj_set_flex_grow(btn, 1);
    lv_obj_set_height(btn, 32);

    lv_obj_set_style_radius(btn, 6, 0);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_20, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1C2B34), 0);

    lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x2DE0C7), LV_STATE_FOCUSED);

    lv_obj_t * l_val = lv_label_create(btn);
    lv_label_set_text(l_val, "-");
    lv_obj_set_style_text_color(l_val, lv_color_hex(0xDDE6EE), 0);
    lv_obj_center(l_val);

    ui->ch_item_btn[idx] = btn;
    ui->ch_item_val[idx] = l_val;

    return btn;
}

void ch_item_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(!ui || !ui->ch_grp) return;

    if(code == LV_EVENT_CLICKED) {
        bool ed = lv_group_get_editing(ui->ch_grp);
        lv_group_set_editing(ui->ch_grp, !ed);
        return;
    }

    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_ENTER) {
            bool ed = lv_group_get_editing(ui->ch_grp);
            lv_group_set_editing(ui->ch_grp, !ed);
        }
    }
}

void CHPannel_close_btn_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    if(!ui) return;

    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if(ui->save_all) {
        ch_apply_to_all(ui, ui->sel_row);
        table_refresh_all(ui);
        ch_panel_refresh(ui);
    } else {
        for(uint16_t c = 0; c < TBL_COLS; c++){
            table_format_cell(ui, ui->sel_row, c);
        }
    }

    CHPannel_close(ui);
}

void ch_edit_timer_cb(lv_timer_t * t)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_timer_get_user_data(t);
    if(!ui || !ui->CH_panel_mask) return;
    if(!ui->ch_grp || !lv_group_get_editing(ui->ch_grp)) return;

    int diff = (int)KeyInput_EncoderGetDiffAndClear();
    if(diff == 0) return;

    int dir = (diff > 0) ? +1 : -1;
    int step = calc_encoder_step(diff);

    int field = ch_get_focused_field(ui);
    if(field < 0) return;

    uint16_t r = ui->sel_row;
    if(r >= ui->ch_count) return;

    st_trig_con * conf = ui_get_active_trig_con(ui);
    st_channel_con * ch = &conf->ch_con[r];

    switch(field) {
    case 0: { /* ON */
        int v = (int)ch->on + dir * step;
        if(v < ON_MIN) v = ON_MIN;
        if(v > ON_MAX) v = ON_MAX;
        ch->on = (uint16_t)v;
    } break;

    case 1: { /* DELAY */
        int v = (int)ch->delay + dir * step;
        if(v < DELAY_MIN) v = DELAY_MIN;
        if(v > DELAY_MAX) v = DELAY_MAX;
        ch->delay = (uint16_t)v;
    } break;

    case 2: { /* BLOCK */
        int v = (int)ch->block + dir * step;
        if(v < BLOCK_MIN) v = BLOCK_MIN;
        if(v > BLOCK_MAX) v = BLOCK_MAX;
        ch->block = (uint16_t)v;
    } break;

    case 3: { /* TRG (0/1/2 <-> R/F/B) */
        char t2 = trig_mode_to_char(ch->trig_mode);

        if(dir > 0) t2 = (t2=='F')?'R':(t2=='R')?'B':'F';
        else        t2 = (t2=='F')?'B':(t2=='B')?'R':'F';

        ch->trig_mode = trig_char_to_mode(t2);
    } break;
    }

    ch_panel_refresh(ui);

    /* 선택 row만 갱신 */
    for(uint16_t c=0; c<TBL_COLS; c++) {
        table_format_cell(ui, r, c);
    }

    g_trig_con = * conf;
    eeprom_save_sys();
    lv_obj_invalidate(ui->tbl_body); /* 디버그/확실한 리프레시용 */
}

void ch_panel_refresh(ui_strobe_t * ui)
{
    if(!ui) return;
    uint16_t r = ui->sel_row;
    if(r >= ui->ch_count) r = 0;

    st_trig_con * conf = ui_get_active_trig_con(ui);
    st_channel_con * ch = &conf->ch_con[r];

    if(ui->ch_title) {
        char t[24];
        if(g_oper_mode == 0) lv_snprintf(t, sizeof(t), "CH %02u", (unsigned)(r+1));
        else                 lv_snprintf(t, sizeof(t), "P%u CH %02u", (unsigned)(ui->seq_page+1), (unsigned)(r+1));
        lv_label_set_text(ui->ch_title, t);
    }

    if(ui->ch_item_val[0]) { char s[24]; lv_snprintf(s,sizeof(s),"%u", (unsigned)ch->on);    lv_label_set_text(ui->ch_item_val[0], s); }
    if(ui->ch_item_val[1]) { char s[24]; lv_snprintf(s,sizeof(s),"%u", (unsigned)ch->delay); lv_label_set_text(ui->ch_item_val[1], s); }
    if(ui->ch_item_val[2]) { char s[24]; lv_snprintf(s,sizeof(s),"%u", (unsigned)ch->block); lv_label_set_text(ui->ch_item_val[2], s); }
    if(ui->ch_item_val[3]) { char s[8];  lv_snprintf(s,sizeof(s),"%c", trig_mode_to_char(ch->trig_mode)); lv_label_set_text(ui->ch_item_val[3], s); }
}

void CHPannel_close(ui_strobe_t * ui)
{
    if(!ui) return;

    if(ui->ch_timer) lv_timer_pause(ui->ch_timer);

    lv_indev_t * enc = lv_port_indev_get_encoder();
    lv_indev_t * kp  = lv_port_indev_get_keypad();

    /* ★ 잔류 입력 제거(중요) */
    if(enc) lv_indev_wait_release(enc);
    if(kp)  lv_indev_wait_release(kp);

    lv_group_t * back = ui->ch_grp_prev ? ui->ch_grp_prev : s_group;
    if(enc) lv_indev_set_group(enc, back);
    if(kp)  lv_indev_set_group(kp,  back);

    if(ui->ch_grp) {
        lv_group_del(ui->ch_grp);
        ui->ch_grp = NULL;
    }
    ui->ch_grp_prev = NULL;

    if(ui->CH_panel_mask) {
        lv_obj_del(ui->CH_panel_mask);
        ui->CH_panel_mask = NULL;
    }

    ui->CH_panel = NULL;
    ui->CH_btn_close = NULL;
    ui->ch_title = NULL;
    ui->chk_save_all = NULL;

    for(int i=0;i<4;i++){
        ui->ch_item_btn[i] = NULL;
        ui->ch_item_val[i] = NULL;
    }

}

void ch_apply_to_all(ui_strobe_t *ui, uint16_t src)
{
    if(!ui) return;
    if(src >= ui->ch_count) return;

    st_trig_con * conf = ui_get_active_trig_con(ui);
    st_channel_con v = conf->ch_con[src];

    for(uint16_t i = 0; i < ui->ch_count; i++){
        conf->ch_con[i] = v;
    }
}

void table_refresh_all(ui_strobe_t * ui)
{
    if(!ui) return;
    if(!ui->tbl_cell_lbl || !ui->tbl_cell_btn) return;

    uint16_t rows = ui->ch_count;
    if(rows == 0) return;

    for(uint16_t r = 0; r < rows; r++){
        for(uint16_t c = 0; c < TBL_COLS; c++){
            table_format_cell(ui, r, c);
        }
    }
}

int enc_abs(int x) 
{ 
    return (x < 0) ? -x : x; 
}

int calc_encoder_step(int diff)
{
    int base = enc_abs(diff);
    if(base < 1) base = 1;

    static uint32_t last_ms = 0;
    uint32_t now = HAL_GetTick();
    uint32_t dt  = (last_ms == 0) ? 9999u : (now - last_ms);
    last_ms = now;

    int step;
    if      (dt <= 200)  step = 1000;
    else if (dt <= 400)  step = 100;
    else                 step = 1;

    step *= base;

    if(step > 5000) step = 5000;

    return step;
}

int ch_get_focused_field(ui_strobe_t * ui)
{
    if(!ui || !ui->ch_grp) return -1;
    lv_obj_t * f = lv_group_get_focused(ui->ch_grp);
    for(int i=0;i<4;i++){
        if(ui->ch_item_btn[i] == f) return i;
    }
    return -1;
}

st_trig_con * ui_get_active_trig_con(ui_strobe_t *ui)
{
    if(!ui) return &g_trig_con;

    if(g_oper_mode == 0) { /* Trigger */
        return &g_trig_con;
    } else { /* Sequence */
        uint8_t p = ui->seq_page;
        if(p >= PAGE_MAX) p = 0;
        return &g_seq_con.page_con[p];
    }
}

char trig_mode_to_char(uint8_t m)
{
    return (m == 0) ? 'R' : (m == 1) ? 'F' : (m == 2) ? 'B' : 'R';
}

uint8_t trig_char_to_mode(char c)
{
    return (c == 'R') ? 0 : (c == 'F') ? 1 : (c == 'B') ? 2 : 0;
}