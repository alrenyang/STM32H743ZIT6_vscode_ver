#ifndef KEY_INPUT_H
#define KEY_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32h7xx_hal.h"   // MCU에 맞게

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint32_t g_hotkey;

typedef enum {
    KEY_DATA0 = 0,
    KEY_DATA1,
    KEY_DATA2,
    KEY_DATA3,
    KEY_DATA4,
    KEY_LOCK,	//로터리 푸쉬 스위
    KEY_LOCAL,
    KEY_REMOTE,
    KEY_COUNT
} KeyId_t;

typedef enum {
    KEYEV_NONE = 0,
    KEYEV_DOWN,
    KEYEV_UP,
    KEYEV_CLICK,
    KEYEV_LONG,
    KEYEV_REPEAT,
    KEYEV_ROT_CW,
    KEYEV_ROT_CCW
} KeyEventType_t;

typedef struct {
    KeyEventType_t type;
    KeyId_t        id;
    int16_t        value;   // ROT: +1/-1 등
} KeyEvent_t;

typedef struct {
    uint16_t debounce_ms;
    uint16_t longpress_ms;
    uint16_t repeat_start_ms;   // 0이면 repeat off
    uint16_t repeat_period_ms;
} KeyConfig_t;

void  KeyInput_Init(const KeyConfig_t *cfg);
void  KeyInput_Tick(uint32_t now_ms);
bool  KeyInput_GetEvent(KeyEvent_t *ev);

bool  KeyInput_IsPressed(KeyId_t id);

/* LVGL encoder에 편한 API */
int16_t KeyInput_EncoderGetDiffAndClear(void);   // 누적 diff 반환 후 0으로
bool    KeyInput_EncoderIsPressed(void);         // LOCK_KEY 안정 상태(눌림) 반환

#ifdef __cplusplus
}
#endif

#endif
