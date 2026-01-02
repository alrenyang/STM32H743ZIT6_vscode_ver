#ifndef WIDGETS_H
#define WIDGETS_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* You can raise this if your product supports more channels later. */
#ifndef UI_MAX_CH
#define UI_MAX_CH 32
#endif

typedef struct {
    uint16_t on;     /* us (e.g. 1~1000) */
    uint16_t delay;  /* us (e.g. 0~10000) */
    uint16_t block;  /* us (e.g. 0~10000) */
    char     trg;    /* 'F' / 'R' / 'B' */
} ch_data_t;

typedef struct {
    lv_obj_t * scr;

    /*add: blank screen */
    lv_obj_t * scr_blank;
    lv_obj_t * blank_focus;

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
    ch_data_t * ch_data;       /* array length ch_count */

} ui_strobe_t;

ui_strobe_t * widgets_create_strobe_screen(void);
void widgets_bind_encoder(ui_strobe_t * ui, lv_indev_t * indev_encoder);

/* Optional helpers */
void widgets_table_set_channel_count(ui_strobe_t * ui, uint16_t new_count);
void widgets_table_set_cell(ui_strobe_t * ui, uint16_t ch0, uint16_t col, const char * txt);

#ifdef __cplusplus
}
#endif

#endif /* WIDGETS_H */
