/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "key_input.h"
#include "main.h"
//#include "touch.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);

/* encoder/keypad read 콜백 */
static void encoder_read(lv_indev_t * indev, lv_indev_data_t * data);
static void keypad_read(lv_indev_t * indev, lv_indev_data_t * data);
// static bool touchpad_is_pressed(void);
// static void touchpad_get_xy(int32_t * x, int32_t * y);

static void button_read(lv_indev_t * indev, lv_indev_data_t * data);
static int  my_hardware_button_scan_function(void);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;

static lv_indev_t * indev_encoder;
static lv_indev_t * indev_keypad;

/* ===== BUTTON indev ===== */
static lv_indev_t * indev_button;

/* 하드웨어 버튼 ID(0..N-1) => 화면 좌표 매핑 */
static const lv_point_t s_btn_points[] = {
    // { 12,  30 },   /* btn_id 0 : 예) KEY_DATA0 */
    // { 60,  90 },   /* btn_id 1 : 예) KEY_DATA1 */
    // {120,  90 },   /* btn_id 2 : 예) KEY_DATA2 */
    // {180,  90 },   /* btn_id 3 : 예) KEY_DATA3 */
    // {240,  90 },   /* btn_id 4 : 예) KEY_DATA4 */
};
#define HW_BTN_COUNT  (sizeof(s_btn_points)/sizeof(s_btn_points[0]))

static uint32_t s_last_key = 0;
volatile uint32_t g_hotkey = 0;
/**********************
 *  GETTERS
 **********************/

lv_indev_t * lv_port_indev_get_encoder(void) { return indev_encoder; }
lv_indev_t * lv_port_indev_get_keypad(void)  { return indev_keypad;  }

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);

    /* ===== KeyInput init ===== */
    KeyConfig_t cfg = {
		.debounce_ms = 80,
		.longpress_ms = 800,
		.repeat_start_ms = 0,
		.repeat_period_ms = 200
	};
	KeyInput_Init(&cfg);

	/* ===== Encoder indev 등록 ===== */
	indev_encoder = lv_indev_create();
	lv_indev_set_type(indev_encoder, LV_INDEV_TYPE_ENCODER);
	lv_indev_set_read_cb(indev_encoder, encoder_read);

	/* ===== Keypad indev 등록 ===== */
	indev_keypad = lv_indev_create();
	lv_indev_set_type(indev_keypad, LV_INDEV_TYPE_KEYPAD);
	lv_indev_set_read_cb(indev_keypad, keypad_read);

    /* ===== BUTTON indev 등록 ===== */
    indev_button = lv_indev_create();
    lv_indev_set_type(indev_button, LV_INDEV_TYPE_BUTTON);
    lv_indev_set_read_cb(indev_button, button_read);

    /* 버튼 ID ↔ 화면 좌표 연결 */
    // lv_indev_set_button_points(indev_button, s_btn_points);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void lv_port_indev_poll_5ms(void)
{
    KeyInput_Tick(HAL_GetTick());
}

/* ================= Encoder read =================
 * enc_diff: 로터리 회전(+/-)
 * state:   LOCK_KEY 눌림/뗌
 */
static void encoder_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    (void)indev;

    data->enc_diff = KeyInput_EncoderGetDiffAndClear();  // -1/0/+1 누적
    data->state = KeyInput_EncoderIsPressed() ? LV_INDEV_STATE_PRESSED
                                             : LV_INDEV_STATE_RELEASED;
}

static user_key_map map_keyid_to_user(KeyId_t id)
{
    switch(id) {
    case KEY_LOCAL:   return USE_KEY_UP;
    case KEY_REMOTE:  return USE_KEY_DOWN;

    case KEY_DATA0:   return USE_KEY_SET;
    case KEY_DATA1:   return USE_KEY_MODE;
    case KEY_DATA2:   return USE_KEY_MEM;
    case KEY_DATA3:   return USE_KEY_INTER;
    case KEY_LOCK:   return USE_KEY_LOCK;

    default:          return 0;
    }
}


// static uint32_t map_key_to_lv(KeyId_t id)
// {
//     // switch (id) {
//     // case KEY_LOCAL:  return LV_KEY_ENTER;	//로터리푸쉬키입력
//     // case KEY_REMOTE: return LV_KEY_DOWN;

//     // case KEY_DATA0:  return LV_KEY_BACKSPACE;
//     // case KEY_DATA1:  return LV_KEY_RIGHT;
//     // case KEY_DATA2:  return LV_KEY_UP;
//     // case KEY_DATA3:  return LV_KEY_ESC;
//     // case KEY_DATA4:  return LV_KEY_HOME;

//     // default:         return 0;
//     // }
// //     user_key_map uk = map_keyid_to_user(id);
// //     return user_key_to_lv(uk);
//     return 0;
// }



// static uint32_t map_hw_to_special(KeyId_t id)
// {
//     switch(id) {
//     case KEY_DATA4:  return SPK_OPEN_HIDDEN_MENU;  // 예시
//     default:         return 0;
//     }
// }

static void keypad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    (void)indev;

    /* 기본값: release + last_key 유지 */
    data->state = LV_INDEV_STATE_RELEASED;
    data->key   = (lv_key_t)s_last_key;

    /* 이벤트 큐에서 CLICK/DOWN을 LVGL 키로 변환 */
    KeyEvent_t ev;
    while (KeyInput_GetEvent(&ev)) {

        /* 로터리 관련 이벤트는 encoder indev가 처리하므로 skip */
        if (ev.type == KEYEV_ROT_CW || ev.type == KEYEV_ROT_CCW) continue;

        /* keypad는 DOWN을 pressed로*/
        if (ev.type == KEYEV_DOWN) {
            uint32_t k = map_keyid_to_user(ev.id);
            if(k == 0) k = map_keyid_to_user(ev.id);
            if (k != 0) {
                if (k == USE_KEY_UP || k == USE_KEY_DOWN || k == USE_KEY_SET || k == USE_KEY_MODE || 
                    k == USE_KEY_MEM || k == USE_KEY_LOCK || k == USE_KEY_INTER ) 
                {
                    g_hotkey = k;
                }
                s_last_key = k;
                data->key = (lv_key_t)k;
                data->state = LV_INDEV_STATE_PRESSED;
                return;
            }
        }

        /* UP 시점에 release */
        if (ev.type == KEYEV_UP) {
            uint32_t k = map_keyid_to_user(ev.id);
            if(k == 0) k = map_keyid_to_user(ev.id);
            if (k != 0) {
                s_last_key = k;
                data->key = (lv_key_t)k;
                data->state = LV_INDEV_STATE_RELEASED;
                return;
            }
        }
    }
}

/* ================= BUTTON indev =================
 * 여기서는 이벤트 큐(KeyInput_GetEvent)를 소비하지 않고,
 * “현재 눌림 상태”만 보고 좌표 클릭을 만들어 충돌을 피함.
 */
static int my_hardware_button_scan_function(void)
{
    /* 눌린 버튼이 있으면 해당 id 리턴, 없으면 -1 */
    if (KeyInput_IsPressed(KEY_DATA0)) return 0;
    if (KeyInput_IsPressed(KEY_DATA1)) return 1;
    if (KeyInput_IsPressed(KEY_DATA2)) return 2;
    if (KeyInput_IsPressed(KEY_DATA3)) return 3;
    if (KeyInput_IsPressed(KEY_DATA4)) return 4;
    return -1;
}

static void button_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    (void)indev;
    static uint16_t last_btn = 0;
    static bool prev_pressed = false;

    int id = my_hardware_button_scan_function();

    if (id >= 0 && id < (int)HW_BTN_COUNT) {
        last_btn = (uint16_t)id;
        data->btn_id = last_btn;
        data->state  = LV_INDEV_STATE_PRESSED;
        prev_pressed = true;
    } else {
        data->btn_id = last_btn;
        data->state  = prev_pressed ? LV_INDEV_STATE_RELEASED
                                    : LV_INDEV_STATE_RELEASED;
        prev_pressed = false;
    }
}



/* ================= Keypad read =================
 * LVGL은 data->key에 LV_KEY_* 를 넣어줘야 함.
 * 여기선 예시 매핑:
 *  LOCAL  -> LV_KEY_ESC
 *  REMOTE -> LV_KEY_ENTER
 *  DATA0  -> LV_KEY_LEFT
 *  DATA1  -> LV_KEY_RIGHT
 *  DATA2  -> LV_KEY_UP
 *  DATA3  -> LV_KEY_DOWN
 *  DATA4  -> LV_KEY_HOME
 */



/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
//    static int32_t last_x = 0;
//    static int32_t last_y = 0;
//    static int8_t    last_id = -1;
//
//    touch_info_t info;
//    bool is_pressed = false;
//
//    info.count = 0;
//    touchGetInfo(&info);
//
//    for (int i=0; i<info.count; i++)
//    {
//        if (last_id < 0)
//        {
//            last_id = info.point[i].id;
//            last_x = info.point[i].x;
//            last_y = info.point[i].y;
//            is_pressed = true;
//            break;
//        }
//        else
//        {
//        if (info.point[i].id == last_id)
//        {
//            last_x = info.point[i].x;
//            last_y = info.point[i].y;
//            is_pressed = true;
//            break;
//        }
//        }
//    }
//
//    if(is_pressed == true)
//    {
//        data->state = LV_INDEV_STATE_PRESSED;
//    }
//    else
//    {
//        last_id = -1;
//        data->state = LV_INDEV_STATE_RELEASED;
//    }
//
//    /*Set the last pressed coordinates*/
//    data->point.x = last_x;
//    data->point.y = last_y;
}

/*Return true is the touchpad is pressed*/
// static bool touchpad_is_pressed(void)
// {
//     /*Your code comes here*/

//     return false;
// }

/*Get the x and y coordinates if the touchpad is pressed*/
// static void touchpad_get_xy(int32_t * x, int32_t * y)
// {
//     /*Your code comes here*/

//     (*x) = 0;
//     (*y) = 0;
// }


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
