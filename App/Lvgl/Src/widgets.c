#include "widgets.h"
#include "lv_port_indev.h"
#include <stdlib.h>
#include <stdio.h>
/* Encoder focus group (footer buttons only).
 * Table cells are intentionally NOT focusable and do NOT handle LV_EVENT_KEY.
 */
static lv_group_t * s_group = NULL;


/* ===== 480x272 fixed layout ===== */
#define LCD_W 480
#define LCD_H 272

static inline int DISP_W(void) {
    lv_display_t * d = lv_display_get_default();
    return d ? (int)lv_display_get_horizontal_resolution(d) : 480;
}
static inline int DISP_H(void) {
    lv_display_t * d = lv_display_get_default();
    return d ? (int)lv_display_get_vertical_resolution(d) : 272;
}

#define HEADER_H 36
#define FOOTER_H 36
#define GAP      6

/* ===== Colors (macro) ===== */
#define C_BG      lv_color_hex(0x070C11)
#define C_PANEL   lv_color_hex(0x0B1118)
#define C_LINE    lv_color_hex(0x1C2B34)

#define C_TEXT    lv_color_hex(0xDDE6EE)
#define C_DIM     lv_color_hex(0x8AA3B5)

#define C_TEAL    lv_color_hex(0x2DE0C7)
#define C_YEL     lv_color_hex(0xD8C26A)
#define C_TEAL_TX lv_color_hex(0x9FF6E9)
#define C_TX      lv_color_hex(0xDDE6EE)
#define C_GOLD    lv_color_hex(0xD7B36A)
#define C_RED     lv_color_hex(0xFF4D4D)

static void row_event_cb(lv_event_t * e);
static void blank_focus_event_cb(lv_event_t * e);
static void ui_open_blank_screen(ui_strobe_t * ui);
static void ui_close_blank_screen(ui_strobe_t * ui);



/* ===== Styles ===== */
static lv_style_t st_scr, st_hdr, st_ftr, st_panel;
static lv_style_t st_btn, st_btn_focus, st_btn_primary, st_tag, st_tag_bad;
static lv_style_t st_row, st_row_focus;
static bool s_style_inited = false;

static void table_cell_clicked_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(!ui || !btn) return;

    /* r,c를 user_data에 packed 해둔 값을 꺼냄 */
    uintptr_t pack = (uintptr_t)lv_obj_get_user_data(btn);
    uint16_t r = (uint16_t)((pack >> 8) & 0xFF);
    uint16_t c = (uint16_t)(pack & 0xFF);

    /* ✅ 여기서 “셀을 눌렀을 때(엔터/PUSH 포함)” 원하는 동작 구현 */
    /* 예) 디버그 표시 */
    printf("[CELL CLICK] r=%u c=%u\n", (unsigned)r, (unsigned)c);

    /* 예) 셀 라벨을 임시로 바꿔서 동작 확인 */
    lv_obj_t * lbl = lv_obj_get_child(btn, 0);
    if(lbl) {
        char buf[32];
        lv_snprintf(buf, sizeof(buf), "R%uC%u", (unsigned)(r+1), (unsigned)(c+1));
        lv_label_set_text(lbl, buf);
        lv_obj_center(lbl);
    }
}


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
    lv_style_init(&st_row);
    lv_style_set_radius(&st_row, 6);
    lv_style_set_bg_opa(&st_row, LV_OPA_TRANSP);

    lv_style_init(&st_row_focus);
    lv_style_set_bg_color(&st_row_focus, lv_color_hex(0x0F2430));
    lv_style_set_bg_opa(&st_row_focus, LV_OPA_COVER);
    lv_style_set_outline_color(&st_row_focus, C_TEAL);
    lv_style_set_outline_width(&st_row_focus, 2);
    lv_style_set_outline_pad(&st_row_focus, 1);
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

/* ===== Table / Grid (CH x 5 columns) ===== */
#define TBL_COLS 5

static lv_obj_t * table_cell_label(lv_obj_t * parent, const char * txt, lv_color_t col, lv_text_align_t align, lv_coord_t w)
{
    lv_obj_t * l = lv_label_create(parent);
    lv_label_set_text(l, txt);
    lv_obj_set_width(l, w);
    lv_obj_set_style_text_color(l, col, 0);
    lv_obj_set_style_text_align(l, align, 0);
    lv_label_set_long_mode(l, LV_LABEL_LONG_CLIP);
    return l;
}

static lv_obj_t * table_cell_btn(lv_obj_t * parent, const char * txt, lv_text_align_t align, lv_coord_t w)
{
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, w, 26);
    lv_obj_set_style_radius(btn, 4, 0);
    lv_obj_set_style_pad_all(btn, 2, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_TRANSP, 0);
    /* Keep table cells purely as display containers (no focus/key navigation). */
    lv_obj_add_style(btn, &st_btn_focus, LV_STATE_FOCUSED);
	lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	/* (클릭 가능은 기본이므로 굳이 flag 추가 안 해도 됨) */

	/* 포커스가 보이게 배경도 약간 */
	lv_obj_set_style_bg_color(btn, lv_color_hex(0x0F2430), LV_STATE_FOCUSED);
	lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_FOCUSED);

    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, txt);
    lv_obj_set_width(lbl, LV_PCT(100));
    lv_obj_set_style_text_align(lbl, align, 0);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_CLIP);
    lv_obj_center(lbl);

    return btn;
}

static lv_obj_t * table_btn_label(lv_obj_t * btn)
{
    return lv_obj_get_child(btn, 0);
}

/* (table focus/key editing intentionally removed) */

static void table_format_cell(ui_strobe_t * ui, uint16_t r, uint16_t c)
{
    if(!ui || !ui->tbl_cell_lbl || r>=ui->ch_count || c>=TBL_COLS) return;
    lv_obj_t * lbl = ui->tbl_cell_lbl[r][c];
    if(!lbl) return;

    char buf[16];
    const ch_data_t * d = &ui->ch_data[r];
    switch(c){
    case 0: lv_snprintf(buf, sizeof(buf), "%u CH", (unsigned)(r+1)); break;
    case 1: lv_snprintf(buf, sizeof(buf), "%u us", (unsigned)d->on); break;
    case 2: lv_snprintf(buf, sizeof(buf), "%u us", (unsigned)d->delay); break;
    case 3: lv_snprintf(buf, sizeof(buf), "%u us", (unsigned)d->block); break;
    case 4: lv_snprintf(buf, sizeof(buf), "%c", d->trg); break;
    default: buf[0]=0; break;
    }
    lv_label_set_text(lbl, buf);
}

/* (table LV_EVENT_KEY handling intentionally removed) */

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

    if(ui->ch_data){
        lv_free(ui->ch_data);
        ui->ch_data = NULL;
    }
}

static void table_build(ui_strobe_t * ui, lv_obj_t * parent)
{
    if(!ui || !parent) return;

    /* parent is p_grid */
    ui->tbl_root = parent;

    lv_obj_clean(parent);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(parent, 8, 0);
    lv_obj_set_style_pad_row(parent, 6, 0);

    /* Header row */
    ui->tbl_hdr = lv_obj_create(parent);
    lv_obj_remove_style_all(ui->tbl_hdr);
    lv_obj_set_width(ui->tbl_hdr, LV_PCT(100));
    lv_obj_set_height(ui->tbl_hdr, 26);
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

    /* Body */
    ui->tbl_body = lv_obj_create(parent);
    lv_obj_remove_style_all(ui->tbl_body);
    lv_obj_set_size(ui->tbl_body, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(ui->tbl_body, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_row(ui->tbl_body, 4, 0);
    lv_obj_set_flex_flow(ui->tbl_body, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(ui->tbl_body, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(ui->tbl_body, LV_SCROLLBAR_MODE_AUTO);

    /* allocate data + cell ptr arrays */
    ui->ch_data = (ch_data_t *)lv_malloc(sizeof(ch_data_t) * ui->ch_count);
    ui->tbl_cell_btn = (lv_obj_t ***)lv_malloc(sizeof(lv_obj_t **) * ui->ch_count);
    ui->tbl_cell_lbl = (lv_obj_t ***)lv_malloc(sizeof(lv_obj_t **) * ui->ch_count);

    ui->tbl_row = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *) * ui->ch_count);
    ui->sel_row = 0;


    for(uint16_t r=0; r<ui->ch_count; r++){
        /* init defaults */
        ui->ch_data[r].on = 1;
        ui->ch_data[r].delay = 0;
        ui->ch_data[r].block = 0;
        ui->ch_data[r].trg = 'F';

        ui->tbl_cell_btn[r] = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *) * TBL_COLS);
        ui->tbl_cell_lbl[r] = (lv_obj_t **)lv_malloc(sizeof(lv_obj_t *) * TBL_COLS);

        lv_obj_t * row = lv_obj_create(ui->tbl_body);
        lv_obj_remove_style_all(row);
        lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_width(row, LV_PCT(100));
        lv_obj_set_height(row, 26);
        lv_obj_set_style_pad_column(row, 6, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);

        /* add: store row + make it focusable by encoder */
		ui->tbl_row[r] = row;
		lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
		lv_obj_add_flag(row, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

		/*add: row focus highlight */
		lv_obj_add_style(row, &st_row, 0);
		lv_obj_add_style(row, &st_row_focus, LV_STATE_FOCUSED);
		lv_obj_add_event_cb(row, row_event_cb, LV_EVENT_KEY | LV_EVENT_CLICKED, ui);


        /* Create 5 cells */
        for(uint16_t c=0; c<TBL_COLS; c++){
            lv_obj_t * btn = table_cell_btn(row, "", (c==0)?LV_TEXT_ALIGN_LEFT:LV_TEXT_ALIGN_CENTER, widths[c]);
            lv_obj_t * lbl = table_btn_label(btn);

            /* Make table text clearly visible (avoid inheriting theme default) */
            lv_obj_set_style_text_color(lbl, (c==0) ? C_TEAL_TX : C_TX, 0);

            ui->tbl_cell_btn[r][c] = btn;
            ui->tbl_cell_lbl[r][c] = lbl;

            /*셀 r,c 저장 + 클릭 콜백 등록 (엔코더 PUSH=CLICKED) */
            lv_obj_set_user_data(btn, (void *)(uintptr_t)((r << 8) | (c & 0xFF)));
            lv_obj_add_event_cb(btn, table_cell_clicked_cb, LV_EVENT_CLICKED, ui);

            table_format_cell(ui, r, c);
        }
    }
}

/* rebuild encoder group objects in the intended order */
static void encoder_group_rebuild(ui_strobe_t * ui, lv_group_t * g)
{
    if(!ui || !g) return;
    lv_group_remove_all_objs(g);

    /*  1) table rows first: encoder rotate => row-by-row focus */
    if(ui->tbl_cell_btn) {
		for(uint16_t r=0; r<ui->ch_count; r++) {
			for(uint16_t c=0; c<TBL_COLS; c++) {
				if(ui->tbl_cell_btn[r][c]) lv_group_add_obj(g, ui->tbl_cell_btn[r][c]);
			}
		}
		/* 첫 포커스 위치 (예: CH1, 첫 데이터 컬럼) */
		if(ui->tbl_cell_btn[0][1]) lv_group_focus_obj(ui->tbl_cell_btn[0][1]);
		else if(ui->tbl_cell_btn[0][0]) lv_group_focus_obj(ui->tbl_cell_btn[0][0]);
	}

	/*2) keep footer buttons in group too (optional, but safe) */
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

    const int w = DISP_W();
    const int h = DISP_H();

    /* =========================
     * Root screen
     * ========================= */
    ui->scr = lv_obj_create(NULL);
    lv_obj_add_style(ui->scr, &st_scr, 0);
    lv_obj_clear_flag(ui->scr, LV_OBJ_FLAG_SCROLLABLE);

    /* “반드시” 실제 해상도로 고정 */
    lv_obj_set_size(ui->scr, w, h);

    /* =========================
     * Fixed layout (absolute positioning)
     *  - Header: y=0..HEADER_H-1
     *  - Body  : y=HEADER_H..h-FOOTER_H-1
     *  - Footer: y=h-FOOTER_H..h-1
     *
     * NOTE: We intentionally avoid flex layout for header/footer because
     * some projects end up with zero-sized internal containers when styles
     * are removed/overridden. Absolute placement is the most robust.
     * ========================= */
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

    /* ==== 좌: grid panel / 우: info panel ==== */
    const int info_w = 120;                    // 우측 고정폭
    const int grid_w = w - (GAP*2) - GAP - info_w;  // 좌측 자동

    lv_obj_t * p_grid = lv_obj_create(ui->body);
    lv_obj_add_style(p_grid, &st_panel, 0);
    lv_obj_set_size(p_grid, grid_w, body_h - (GAP * 2));
    lv_obj_clear_flag(p_grid, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * p_info = lv_obj_create(ui->body);
    lv_obj_add_style(p_info, &st_panel, 0);
    lv_obj_set_size(p_info, info_w, body_h - (GAP * 2));
    lv_obj_clear_flag(p_info, LV_OBJ_FLAG_SCROLLABLE);

    /* ==== p_grid 내부 ==== */
    ui->ch_count = 16; /* default, can be changed later */
    table_build(ui, p_grid);


    /* ==== p_info 내부 ==== */
    lv_obj_set_flex_flow(p_info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(p_info, 8, 0);

    lv_obj_t * info_title = lv_label_create(p_info);
    lv_label_set_text(info_title, "STATUS");
    lv_obj_set_style_text_color(info_title, C_TEAL_TX, 0);

    make_tag(p_info, "SLOT 2/8", false);
    make_tag(p_info, "REPEAT (5)", false);
    make_tag(p_info, "COMM: OK", false);

    lv_obj_t * hint = lv_label_create(p_info);
    /* Table editing/key handling removed: keep hint minimal */
    lv_label_set_text(hint, "ROT: Navigate\n(footer only)");
    lv_obj_set_style_text_color(hint, C_DIM, 0);

    /* =========================
     * Header (absolute)
     * ========================= */
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

    /* =========================
     * Footer (absolute)
     * ========================= */
    ui->footer = lv_obj_create(ui->scr);
    lv_obj_remove_style_all(ui->footer);
    lv_obj_clear_flag(ui->footer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(ui->footer, 0, h - FOOTER_H);
    lv_obj_set_size(ui->footer, w, FOOTER_H);
    lv_obj_set_style_bg_color(ui->footer, lv_color_hex(0x0A1017), 0);
    lv_obj_set_style_bg_opa(ui->footer, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(ui->footer, 6, 0);

    /* Buttons placed absolutely for robustness */
    ui->btn_start = make_btn(ui->footer, "START", true);
    lv_obj_set_style_text_font(ui->lbl_interlock, &lv_font_montserrat_12, 0);
    lv_obj_set_size(ui->btn_start, 92, 25);
    lv_obj_set_pos(ui->btn_start, 6, 0);

    ui->btn_stop  = make_btn(ui->footer, "STOP", false);
    lv_obj_set_style_text_font(ui->lbl_interlock, &lv_font_montserrat_12, 0);
    lv_obj_set_size(ui->btn_stop, 92, 25);
    lv_obj_set_pos(ui->btn_stop, 6 + 92 + 8, 0);

    ui->btn_save = make_btn(ui->footer, "SAVE", false);
    lv_obj_set_style_text_font(ui->lbl_interlock, &lv_font_montserrat_12, 0);
    lv_obj_set_size(ui->btn_save, 92, 25);
    lv_obj_set_pos(ui->btn_save, w - (92*2 + 8 + 6), 0);

    ui->btn_load = make_btn(ui->footer, "LOAD", false);
    lv_obj_set_style_text_font(ui->lbl_interlock, &lv_font_montserrat_12, 0);
    lv_obj_set_size(ui->btn_load, 92, 25);
    lv_obj_set_pos(ui->btn_load, w - (92 + 6), 0);

    /* Ensure header/footer are on top */
    lv_obj_move_foreground(ui->header);
    lv_obj_move_foreground(ui->footer);


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

    /* Table key/edit handling removed: keep encoder group in normal navigation. */
}



void widgets_table_set_channel_count(ui_strobe_t * ui, uint16_t new_count)
{
    if(!ui) return;
    if(new_count == 0) new_count = 1;
    if(new_count > UI_MAX_CH) new_count = UI_MAX_CH;

    /* free old */
    table_free(ui);

    ui->ch_count = new_count;

    if(ui->tbl_root) {
        table_build(ui, ui->tbl_root);
    }

    /* rebuild focus order if encoder group exists */
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

static void ui_open_blank_screen(ui_strobe_t * ui)
{
    if(!ui) return;

    /* create once */
    if(!ui->scr_blank) {
        ui->scr_blank = lv_obj_create(NULL);
        lv_obj_remove_style_all(ui->scr_blank);
        lv_obj_set_size(ui->scr_blank, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_color(ui->scr_blank, lv_color_hex(0x070C11), 0);
        lv_obj_set_style_bg_opa(ui->scr_blank, LV_OPA_COVER, 0);

        /* focus catcher (invisible full screen object) */
        ui->blank_focus = lv_obj_create(ui->scr_blank);
        lv_obj_remove_style_all(ui->blank_focus);
        lv_obj_set_size(ui->blank_focus, LV_PCT(100), LV_PCT(100));
        lv_obj_add_flag(ui->blank_focus, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(ui->blank_focus, blank_focus_event_cb,
                            LV_EVENT_KEY | LV_EVENT_CLICKED, ui);
    }

    /* load blank screen */
    lv_scr_load(ui->scr_blank);

    /* group: keep only blank_focus so encoder press works on blank screen */
    if(s_group) {
        lv_group_remove_all_objs(s_group);
        lv_group_add_obj(s_group, ui->blank_focus);
        lv_group_focus_obj(ui->blank_focus);
    }
}

static void ui_close_blank_screen(ui_strobe_t * ui)
{
    if(!ui) return;

    /* return to main screen */
    lv_scr_load(ui->scr);

    /* restore group objects (rows + footer buttons) */
    if(s_group) {
        encoder_group_rebuild(ui, s_group);
    }
}

static void row_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(!ui) return;

    if(code == LV_EVENT_CLICKED) {
        /* encoder press can arrive here depending on indev */
        ui_open_blank_screen(ui);
        return;
    }

    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_ENTER) {
            ui_open_blank_screen(ui);
        }
    }
}

static void blank_focus_event_cb(lv_event_t * e)
{
    ui_strobe_t * ui = (ui_strobe_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(!ui) return;

    if(code == LV_EVENT_CLICKED) {
        /* press again => back */
        ui_close_blank_screen(ui);
        return;
    }

    if(code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if(key == LV_KEY_ENTER || key == LV_KEY_ESC) {
            ui_close_blank_screen(ui);
        }
    }
}

