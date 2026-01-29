#ifndef WIDGETS_H
#define WIDGETS_H

#include "lvgl.h"
#include "src/misc/lv_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* You can raise this if your product supports more channels later. */
#ifndef UI_MAX_CH
#define UI_MAX_CH 32
#endif

// #define MODE_MODE 0
// #define MODE_CH  0
#define MODE_SET    0

#define TBL_COLS 5

typedef enum {
    CH_F_ON = 0,
    CH_F_DELAY,
    CH_F_BLOCK,
    CH_F_TRG,
} ch_field_t;

#define ON_MIN      1
#define ON_MAX      9999
#define DELAY_MIN   0
#define DELAY_MAX   9999
#define BLOCK_MIN   0
#define BLOCK_MAX   9999


typedef struct {
    lv_obj_t * scr;

    lv_obj_t * CH_panel_mask;
    lv_obj_t * CH_panel;
    lv_obj_t * CH_btn_close;
    lv_obj_t * focus_restore;

    lv_obj_t * header;
    lv_obj_t * body;
    lv_obj_t * footer;

    /* Footer/status labels */
    lv_obj_t * lbl_mode;
    lv_obj_t * lbl_mem;
    lv_obj_t * lbl_interlock;
    lv_obj_t * lbl_comm;

    /* Main action buttons (footer) */
    lv_obj_t * btn_start;
    lv_obj_t * btn_stop;
    lv_obj_t * btn_save;
    lv_obj_t * btn_load;

    /* Table (grid) */
    lv_obj_t * tbl_root;
    lv_obj_t * tbl_hdr;
    lv_obj_t * tbl_body;

    /* Row containers (for encoder line-by-line focus) */
    lv_obj_t ** tbl_row;      /* [ch_count] */
    uint16_t   sel_row;       /* currently focused row (0-based) */

    /* Focusable cell buttons/labels (allocated at runtime) */
    lv_obj_t *** tbl_cell_btn; /* [ch_count][5] */
    lv_obj_t *** tbl_cell_lbl; /* [ch_count][5] */

    uint16_t   ch_count;       /* current channel count */
    // ch_data_t * ch_data;       /* array length ch_count */

    /* modal group */
    lv_group_t * ch_grp_prev;
    lv_group_t * ch_grp;
    lv_timer_t * ch_timer;

    /* value rows (On/Delay/Block/Trg) */
    lv_obj_t * ch_item_btn[4];
    lv_obj_t * ch_item_val[4];
    lv_obj_t * ch_title;     /* "CHxx" 표시용 라벨 */

    lv_obj_t * chk_save_all;    //체크박스 객체
    bool       save_all;

    lv_timer_t * hotkey_timer;  //글로벌 키 타이머

    /* MODE window */
    lv_obj_t * MODE_mask;
    lv_obj_t * MODE_panel;
    lv_obj_t * MODE_btn_close;

    //SETTING window
    lv_obj_t * SETTING_mask;
    lv_obj_t * SETTING_panel;
    lv_obj_t * SETTING_btn_close;

    lv_group_t * mode_grp;
    lv_group_t * mode_grp_prev;

    lv_timer_t * mode_timer;   /* 필요 없으면 빼도 됨 */
    uint8_t seq_page;   // 0~PAGE_MAX-1 (Sequence 모드에서 현재 페이지)

    // ui_strobe_t 안에 추가(예시)
    lv_obj_t * dd_baud;
    lv_obj_t * ta_ip[4];
    lv_obj_t * btn_apply;
    lv_obj_t * btn_close;

    lv_group_t * grp_setting;   // 설정창 포커스 그룹(있으면)
    lv_group_t * grp_setting_prev;

} ui_strobe_t;

ui_strobe_t * widgets_create_strobe_screen(void);
void widgets_bind_encoder(ui_strobe_t * ui, lv_indev_t * indev_encoder);

/* Optional helpers */
void widgets_table_set_channel_count(ui_strobe_t * ui, uint16_t new_count);
void widgets_table_set_cell(ui_strobe_t * ui, uint16_t ch0, uint16_t col, const char * txt);
void table_format_cell(ui_strobe_t * ui, uint16_t row, uint16_t col);


#ifdef __cplusplus
}
#endif

#endif /* WIDGETS_H */
