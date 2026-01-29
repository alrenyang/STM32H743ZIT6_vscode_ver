#include "widgets.h"
// #include "lv_port_indev.h"
#include "key_input.h"
#include "lvgl.h"

#include "com_widget.h"
#include "mod_win.h"
#include "ch_win.h"
#include "set_win.h"

/* Encoder focus group (footer buttons only).
 * Table cells are intentionally NOT focusable and do NOT handle LV_EVENT_KEY.
 */

// lv_group_t * g_widgets_group = NULL;
lv_group_t * s_group = NULL;


#define HEADER_H 36
#define FOOTER_H 36
#define GAP      6

/* forward declarations */

static void hotkey_poll_cb(lv_timer_t * t);

/* ===== Styles ===== */
static lv_style_t st_scr, st_hdr, st_ftr, st_panel;
static lv_style_t st_btn, st_btn_focus, st_btn_primary, st_tag, st_tag_bad;
// static lv_style_t st_row, st_row_focus;	//포커스 스타일
static bool s_style_inited = false;

// extern uint32_t g_rs232_baud;
// extern uint8_t  user_ip[4];


static void styles_init(void)
{
    if (s_style_inited) return;
    s_style_inited = true;

    lv_style_init(&st_scr);
    lv_style_set_bg_color(&st_scr, C_BG);
    lv_style_set_bg_opa(&st_scr, LV_OPA_COVER);
    lv_style_set_pad_all(&st_scr, 0);
    lv_style_set_border_width(&st_scr, 0);

    lv_style_init(&st_hdr);
    lv_style_set_bg_color(&st_hdr, lv_color_hex(0x0A1017));
    lv_style_set_bg_opa(&st_hdr, LV_OPA_COVER);
    lv_style_set_pad_left(&st_hdr, 10);
    lv_style_set_pad_right(&st_hdr, 10);
    lv_style_set_pad_top(&st_hdr, 6);
    lv_style_set_pad_bottom(&st_hdr, 6);

    lv_style_init(&st_ftr);
    lv_style_set_bg_color(&st_ftr, lv_color_hex(0x0A1017));
    lv_style_set_bg_opa(&st_ftr, LV_OPA_COVER);
    lv_style_set_pad_left(&st_ftr, 10);
    lv_style_set_pad_right(&st_ftr, 10);
    lv_style_set_pad_top(&st_ftr, 6);
    lv_style_set_pad_bottom(&st_ftr, 6);

    lv_style_init(&st_panel);
    lv_style_set_bg_color(&st_panel, C_PANEL);
    lv_style_set_bg_opa(&st_panel, LV_OPA_COVER);
    lv_style_set_border_color(&st_panel, C_LINE);
    lv_style_set_border_width(&st_panel, 1);
    lv_style_set_radius(&st_panel, 10);
    lv_style_set_pad_all(&st_panel, 8);

    lv_style_init(&st_btn);
    lv_style_set_radius(&st_btn, 10);
    lv_style_set_bg_color(&st_btn, lv_color_hex(0x0E1822));
    lv_style_set_bg_opa(&st_btn, LV_OPA_COVER);
    lv_style_set_border_color(&st_btn, C_LINE);
    lv_style_set_border_width(&st_btn, 1);
    lv_style_set_text_color(&st_btn, C_TEXT);
    lv_style_set_pad_all(&st_btn, 10);

    lv_style_init(&st_btn_focus);
    lv_style_set_outline_color(&st_btn_focus, C_TEAL);
    lv_style_set_outline_width(&st_btn_focus, 3);
    lv_style_set_outline_pad(&st_btn_focus, 2);

    lv_style_init(&st_btn_primary);
    lv_style_set_bg_color(&st_btn_primary, lv_color_hex(0x0F2430));
    lv_style_set_border_color(&st_btn_primary, C_TEAL);
    lv_style_set_border_width(&st_btn_primary, 1);
    lv_style_set_text_color(&st_btn_primary, C_TEAL_TX);

    lv_style_init(&st_tag);
    lv_style_set_radius(&st_tag, 999);
    lv_style_set_bg_color(&st_tag, lv_color_hex(0x0F2430));
    lv_style_set_bg_opa(&st_tag, LV_OPA_COVER);
    lv_style_set_border_color(&st_tag, C_LINE);
    lv_style_set_border_width(&st_tag, 1);
    lv_style_set_pad_left(&st_tag, 10);
    lv_style_set_pad_right(&st_tag, 10);
    lv_style_set_pad_top(&st_tag, 4);
    lv_style_set_pad_bottom(&st_tag, 4);
    lv_style_set_text_color(&st_tag, C_TEAL_TX);

    lv_style_init(&st_tag_bad);
    lv_style_set_bg_color(&st_tag_bad, lv_color_hex(0x2A1010));
    lv_style_set_border_color(&st_tag_bad, C_RED);
    lv_style_set_text_color(&st_tag_bad, lv_color_hex(0xFFD3D3));

    /* ===== Row focus styles (for encoder row navigation) ===== */
    lv_style_init(&g_st_row);
    lv_style_set_radius(&g_st_row, 10);
    lv_style_set_bg_opa(&g_st_row, LV_OPA_TRANSP);

    lv_style_init(&g_st_row_focus);
    lv_style_set_bg_color(&g_st_row_focus, lv_color_hex(0x0F2430));
    lv_style_set_bg_opa(&g_st_row_focus, LV_OPA_COVER);
    lv_style_set_outline_color(&g_st_row_focus, C_TEAL);
    lv_style_set_outline_width(&g_st_row_focus, 1);
    lv_style_set_outline_pad(&g_st_row_focus, 0);
    lv_style_set_radius(&g_st_row_focus, 10);
    lv_style_set_pad_left(&g_st_row_focus, 1);
    lv_style_set_pad_right(&g_st_row_focus, 1);
}


/*-----------------------------------------------------------*/
// global 키 입력
/*-----------------------------------------------------------*/
extern volatile uint32_t g_hotkey;
static void hotkey_poll_cb(lv_timer_t * t)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_timer_get_user_data(t);
    uint32_t k = g_hotkey;
    if(!k) return;
    g_hotkey = 0;

    switch(k)
    {
        case USE_KEY_MODE:
            /* MODE 키: 다른 팝업만 닫고, MODE는 토글 */
            if(ui->CH_panel_mask) CHPannel_close(ui);
            if(ui->SETTING_mask)  Setting_window_close(ui);

            if(!ui->MODE_mask) Mode_window_open(ui);
            else               Mode_window_close(ui);
            break;

        case USE_KEY_SET:
            /* SET 키도 동일 토글 패턴 추천 */
            if(ui->CH_panel_mask) CHPannel_close(ui);
            if(ui->MODE_mask)     Mode_window_close(ui);

            if(!ui->SETTING_mask) Setting_window_open(ui);
            else                  Setting_window_close(ui);
            break;
        
        case USE_KEY_INTER:
            break;
        
        case USE_KEY_MEM:
            break;

        case USE_KEY_UP:
            break;

        case USE_KEY_DOWN:
            break;

        case USE_KEY_LOCK:
            break;

        default:
            /* 다른 키: 일괄 닫고 싶으면 여기서 처리 */
            if(ui->CH_panel_mask) CHPannel_close(ui);
            if(ui->SETTING_mask)  Setting_window_close(ui);
            if(ui->MODE_mask)     Mode_window_close(ui);
            break;
    }
}

/* helper */
static lv_obj_t * make_tag(lv_obj_t * parent, const char * txt, bool bad)
{
    lv_obj_t * l = lv_label_create(parent);
    lv_label_set_text(l, txt);
    lv_obj_add_style(l, &st_tag, 0);
    if (bad) lv_obj_add_style(l, &st_tag_bad, 0);
    return l;
}

static lv_obj_t * make_btn(lv_obj_t * parent, const char * title, bool primary)
{
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_add_style(btn, &st_btn, 0);
    if (primary) lv_obj_add_style(btn, &st_btn_primary, 0);
    lv_obj_add_style(btn, &st_btn_focus, LV_STATE_FOCUSED);

    lv_obj_t * l = lv_label_create(btn);
    lv_label_set_text(l, title);
    lv_obj_center(l);

    return btn;
}

/* =========================
 * 테이블 셀 포맷: trig_mode는 0/1/2를 %c로 찍으면 안 보임!
 * 무조건 R/F/B로 변환해서 출력
 * ========================= */
void table_format_cell(ui_strobe_t * ui, uint16_t r, uint16_t c)
{
    if(!ui || !ui->tbl_cell_lbl || r>=ui->ch_count || c>=TBL_COLS) return;
    lv_obj_t * lbl = ui->tbl_cell_lbl[r][c];
    if(!lbl) return;

    st_trig_con * conf = ui_get_active_trig_con(ui);
    st_channel_con * ch = &conf->ch_con[r];

    char buf[24];

    switch(c){
    case 0: lv_snprintf(buf, sizeof(buf), "%u CH", (unsigned)(r+1)); break;
    case 1: lv_snprintf(buf, sizeof(buf), "%u us", (unsigned)ch->on); break;
    case 2: lv_snprintf(buf, sizeof(buf), "%u us", (unsigned)ch->delay); break;
    case 3: lv_snprintf(buf, sizeof(buf), "%u us", (unsigned)ch->block); break;
    case 4: lv_snprintf(buf, sizeof(buf), "%c", trig_mode_to_char(ch->trig_mode)); break;
    default: buf[0]=0; break;
    }

    lv_label_set_text(lbl, buf);
}

static void table_free(ui_strobe_t * ui)
{
    if(!ui) return;
    if(ui->tbl_cell_btn){
        for(uint16_t r=0; r<ui->ch_count; r++){
            if(ui->tbl_cell_btn[r]) lv_free(ui->tbl_cell_btn[r]);
        }
        lv_free(ui->tbl_cell_btn);
        ui->tbl_cell_btn = NULL;
    }
    if(ui->tbl_cell_lbl){
        for(uint16_t r=0; r<ui->ch_count; r++){
            if(ui->tbl_cell_lbl[r]) lv_free(ui->tbl_cell_lbl[r]);
        }
        lv_free(ui->tbl_cell_lbl);
        ui->tbl_cell_lbl = NULL;
    }
    if(ui->tbl_row){
        lv_free(ui->tbl_row);
        ui->tbl_row = NULL;
    }
}

/* rebuild encoder group objects in the intended order */
static void encoder_group_rebuild(ui_strobe_t * ui, lv_group_t * g)
{
    if(!ui || !g) return;
    lv_group_remove_all_objs(g);

    if(ui->tbl_row){
        for(uint16_t r = 0; r < ui->ch_count; r++){
            if(ui->tbl_row[r]) lv_group_add_obj(g, ui->tbl_row[r]);
        }
        if(ui->tbl_row[0]) lv_group_focus_obj(ui->tbl_row[0]);
    }

    if(ui->btn_start) lv_group_add_obj(g, ui->btn_start);
    if(ui->btn_stop)  lv_group_add_obj(g, ui->btn_stop);
    if(ui->btn_save)  lv_group_add_obj(g, ui->btn_save);
    if(ui->btn_load)  lv_group_add_obj(g, ui->btn_load);
}


ui_strobe_t * widgets_create_strobe_screen(void)
{
    styles_init();

    ui_strobe_t * ui = (ui_strobe_t *)calloc(1, sizeof(ui_strobe_t));
    if (!ui) return NULL;

    const int w = DISP_W;
    const int h = DISP_H;

    /* seq_page 기본값(Sequence모드에서 page_con[0]부터 보여줌) */
    ui->seq_page = 0;

    ui->scr = lv_obj_create(NULL);
    lv_obj_add_style(ui->scr, &st_scr, 0);
    lv_obj_clear_flag(ui->scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_size(ui->scr, w, h);

    const int body_y = HEADER_H;
    const int body_h = h - HEADER_H - FOOTER_H;

    ui->body = lv_obj_create(ui->scr);
    lv_obj_remove_style_all(ui->body);
    lv_obj_clear_flag(ui->body, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(ui->body, 0, body_y);
    lv_obj_set_size(ui->body, w, body_h);
    lv_obj_set_style_pad_all(ui->body, GAP, 0);
    lv_obj_set_style_bg_opa(ui->body, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(ui->body, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(ui->body, GAP, 0);

    const int info_w = 120;
    const int grid_w = w - (GAP*2) - GAP - info_w;

    lv_obj_t * p_grid = lv_obj_create(ui->body);
    lv_obj_add_style(p_grid, &st_panel, 0);
    lv_obj_set_size(p_grid, grid_w, body_h - (GAP * 2));
    lv_obj_clear_flag(p_grid, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * p_info = lv_obj_create(ui->body);
    lv_obj_add_style(p_info, &st_panel, 0);
    lv_obj_set_size(p_info, info_w, body_h - (GAP * 2));
    lv_obj_clear_flag(p_info, LV_OBJ_FLAG_SCROLLABLE);

    ui->ch_count = CH_MAX;
    table_build(ui, p_grid);

    lv_obj_set_flex_flow(p_info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(p_info, 8, 0);

    lv_obj_t * info_title = lv_label_create(p_info);
    lv_label_set_text(info_title, "STATUS");
    lv_obj_set_style_text_color(info_title, C_TEAL_TX, 0);

    make_tag(p_info, "SLOT 2/8", false);
    make_tag(p_info, "REPEAT (5)", false);
    make_tag(p_info, "COMM: OK", false);

    lv_obj_t * hint = lv_label_create(p_info);
    lv_label_set_text(hint, "ROT: Navigate\n(footer only)");
    lv_obj_set_style_text_color(hint, C_DIM, 0);

    ui->header = lv_obj_create(ui->scr);
    lv_obj_remove_style_all(ui->header);
    lv_obj_clear_flag(ui->header, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(ui->header, 0, 0);
    lv_obj_set_size(ui->header, w, HEADER_H);
    lv_obj_set_style_bg_color(ui->header, lv_color_hex(0x0A1017), 0);
    lv_obj_set_style_bg_opa(ui->header, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(ui->header, 6, 0);

    lv_obj_t * title = lv_label_create(ui->header);
    lv_label_set_text(title, "TRI vision");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title, C_GOLD, 0);
    lv_obj_set_pos(title, 8, 6);

    ui->lbl_mode = lv_label_create(ui->header);
    lv_label_set_text(ui->lbl_mode, "MODE: TRIGGER");
    lv_obj_add_style(ui->lbl_mode, &st_tag, 0);
    lv_obj_set_style_text_font(ui->lbl_mode, &lv_font_montserrat_12, 0);
    lv_obj_align(ui->lbl_mode, LV_ALIGN_TOP_RIGHT, -250, 2);

    ui->lbl_comm = lv_label_create(ui->header);
    lv_label_set_text(ui->lbl_comm, "19200");
    lv_obj_add_style(ui->lbl_comm, &st_tag, 0);
    lv_obj_set_style_text_font(ui->lbl_comm, &lv_font_montserrat_12, 0);
    lv_obj_align(ui->lbl_comm, LV_ALIGN_TOP_RIGHT, -150, 2);

    ui->lbl_interlock = lv_label_create(ui->header);
    lv_label_set_text(ui->lbl_interlock, "INT: 1");
    lv_obj_add_style(ui->lbl_interlock, &st_tag, 0);
    lv_obj_set_style_text_font(ui->lbl_interlock, &lv_font_montserrat_12, 0);
    lv_obj_align(ui->lbl_interlock, LV_ALIGN_TOP_RIGHT, -85, 2);

    ui->lbl_mem = lv_label_create(ui->header);
    lv_label_set_text(ui->lbl_mem, "MEM: 0");
    lv_obj_add_style(ui->lbl_mem, &st_tag, 0);
    lv_obj_set_style_text_font(ui->lbl_mem, &lv_font_montserrat_12, 0);
    lv_obj_align(ui->lbl_mem, LV_ALIGN_TOP_RIGHT, -6, 2);

    ui->footer = lv_obj_create(ui->scr);
    lv_obj_remove_style_all(ui->footer);
    lv_obj_clear_flag(ui->footer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(ui->footer, 0, h - FOOTER_H);
    lv_obj_set_size(ui->footer, w, FOOTER_H);
    lv_obj_set_style_bg_color(ui->footer, lv_color_hex(0x0A1017), 0);
    lv_obj_set_style_bg_opa(ui->footer, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(ui->footer, 6, 0);

    ui->btn_start = make_btn(ui->footer, "MODE", true);
    lv_obj_set_size(ui->btn_start, 92, 25);
    lv_obj_set_pos(ui->btn_start, 6, 0);
    /* 클릭 이벤트 등록 */
    lv_obj_add_event_cb(
        ui->btn_start,
        mode_start_btn_event_cb,
        LV_EVENT_CLICKED,
        ui
    );

    ui->btn_stop  = make_btn(ui->footer, "SET", false);
    lv_obj_set_size(ui->btn_stop, 92, 25);
    lv_obj_set_pos(ui->btn_stop, 6 + 92 + 8, 0);

    ui->btn_save = make_btn(ui->footer, "INT", false);
    lv_obj_set_size(ui->btn_save, 92, 25);
    lv_obj_set_pos(ui->btn_save, w - (92*2 + 8 + 6), 0);

    ui->btn_load = make_btn(ui->footer, "MEMORY", false);
    lv_obj_set_size(ui->btn_load, 92, 25);
    lv_obj_set_pos(ui->btn_load, w - (92 + 6), 0);

    lv_obj_move_foreground(ui->header);
    lv_obj_move_foreground(ui->footer);

    ui->hotkey_timer = lv_timer_create(hotkey_poll_cb, 1, ui);

    return ui;
}

void widgets_bind_encoder(ui_strobe_t * ui, lv_indev_t * indev_encoder)
{
    if (!ui) return;

    if (!s_group) {
        s_group = lv_group_create();
        lv_group_set_wrap(s_group, true);
    }
    lv_group_t * g = s_group;

    if (indev_encoder) {
        lv_indev_set_group(indev_encoder, g);
    }

    lv_indev_t * kp = lv_port_indev_get_keypad();
    if (kp) {
        lv_indev_set_group(kp, g);
    }

    encoder_group_rebuild(ui, g);
}

void widgets_table_set_channel_count(ui_strobe_t * ui, uint16_t new_count)
{
    if(!ui) return;
    if(new_count == 0) new_count = 1;
    if(new_count > UI_MAX_CH) new_count = UI_MAX_CH;

    table_free(ui);
    ui->ch_count = new_count;

    if(ui->tbl_root) {
        table_build(ui, ui->tbl_root);
    }

    if(s_group) encoder_group_rebuild(ui, s_group);
}

void widgets_table_set_cell(ui_strobe_t * ui, uint16_t ch0, uint16_t col, const char * txt)
{
    if(!ui || !ui->tbl_cell_lbl) return;
    if(ch0 >= ui->ch_count) return;
    if(col >= TBL_COLS) return;
    if(!ui->tbl_cell_lbl[ch0][col]) return;
    lv_label_set_text(ui->tbl_cell_lbl[ch0][col], txt);
}

void widgets_destroy_strobe_screen(ui_strobe_t * ui)
{
    if(!ui) return;

    if(ui->hotkey_timer) {
        lv_timer_del(ui->hotkey_timer);
        ui->hotkey_timer = NULL;
    }

    if(ui->scr) {
        lv_obj_del(ui->scr);
        ui->scr = NULL;
    }

    lv_free(ui);
}
