/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "lcd.h"

/*********************
 *      DEFINES
 *********************/

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_display_t * disp;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_port_callback(void)
{
  lv_display_flush_ready(disp);
}

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, disp_flush);

    lv_display_set_buffers(disp, lcdGetFrameBuffer(), NULL, LCD_WIDTH*LCD_HEIGHT*2, LV_DISPLAY_RENDER_MODE_DIRECT);
    lcdRegisterVsyncCallback(lv_port_callback);
    lcdEnableVsyncLineInterrupt(0);
//    lv_display_flush_ready(disp);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
#if 0 /* Original Code */
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    #if 0
    if(disp_flush_enabled)
    {
        if (lcdDrawAvailable())
        {
            lcdRequestDraw();
        }
    }
    #else
    if (disp_flush_enabled)
    {
        // lcdRequestDraw();
        lcdUpdateDraw();

        while (!lcdDrawAvailable())
        {
            // delay(1);
        }
        // lv_disp_draw_buf_init(&draw_buf_dsc, lcdGetFrameBuffer(), NULL, LCD_WIDTH * LCD_HEIGHT);

        /*IMPORTANT!!!
        *Inform the graphics library that you are ready with the flushing*/
        // lv_disp_flush_ready(disp_drv);
        lv_display_flush_ready(disp);
    }
    #endif
}
#endif

static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if (disp_flush_enabled)
    {
        /*
         * lcdUpdateDraw should start an asynchronous transfer to the display.
         * lv_display_flush_ready will be called from lv_port_callback when the transfer is complete.
         */
        lcdUpdateDraw();
    }
    else
    {
        /* If flushing is disabled, tell LVGL that we are ready. */
        lv_display_flush_ready(disp_drv);
    }
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
