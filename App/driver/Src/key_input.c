#include "main.h"
#include "key_input.h"
#include <string.h>

/* =========================
 * Active level 설정
 *  - 눌렀을 때 GPIO가 LOW면 GPIO_PIN_RESET
 *  - 눌렀을 때 GPIO가 HIGH면 GPIO_PIN_SET
 * ========================= */
#ifndef KEY_ACTIVE_LEVEL
#define KEY_ACTIVE_LEVEL GPIO_PIN_RESET   // 보통 버튼은 Pull-up + 눌림=LOW가 많음
#endif

// volatile uint32_t g_hotkey = 0;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} GpioRef_t;

/* === 너 프로젝트 핀 이름 그대로 사용 === */
static const GpioRef_t s_key_gpio[KEY_COUNT] = {
    [KEY_DATA0]  = { KEY_DATA0_GPIO_Port,  KEY_DATA0_Pin  },
    [KEY_DATA1]  = { KEY_DATA1_GPIO_Port,  KEY_DATA1_Pin  },
    [KEY_DATA2]  = { KEY_DATA2_GPIO_Port,  KEY_DATA2_Pin  },
    [KEY_DATA3]  = { KEY_DATA3_GPIO_Port,  KEY_DATA3_Pin  },
    [KEY_DATA4]  = { KEY_DATA4_GPIO_Port,  KEY_DATA4_Pin  },
    [KEY_LOCK]   = { LOCK_KEY_GPIO_Port,   LOCK_KEY_Pin   },
    [KEY_LOCAL]  = { LOCAL_KEY_GPIO_Port,  LOCAL_KEY_Pin  },
    [KEY_REMOTE] = { REMOTE_KEY_GPIO_Port, REMOTE_KEY_Pin },
};

/* Rotary */
static const GpioRef_t s_encA = { EN_A_GPIO_Port, EN_A_Pin };
static const GpioRef_t s_encB = { EN_B_GPIO_Port, EN_B_Pin };

static KeyConfig_t s_cfg = {
    .debounce_ms      = 80,
    .longpress_ms     = 800,
    .repeat_start_ms  = 0,
    .repeat_period_ms = 200,
};

typedef struct {
    uint8_t  raw;
    uint8_t  stable;
    uint32_t last_change_ms;

    uint32_t press_start_ms;
    uint8_t  long_fired;
    uint32_t last_repeat_ms;
} PushState_t;

static PushState_t s_push[KEY_COUNT];

/* ===== Event FIFO ===== */
#define KEY_EVENT_QSIZE 32
static KeyEvent_t s_q[KEY_EVENT_QSIZE];
static volatile uint16_t s_q_w, s_q_r;

static void q_push(KeyEventType_t type, KeyId_t id, int16_t value)
{
    uint16_t next = (uint16_t)((s_q_w + 1u) % KEY_EVENT_QSIZE);
    if (next == s_q_r) return; // overflow drop
    s_q[s_q_w] = (KeyEvent_t){ .type = type, .id = id, .value = value };
    s_q_w = next;
}

bool KeyInput_GetEvent(KeyEvent_t *ev)
{
    if (s_q_r == s_q_w) return false;
    *ev = s_q[s_q_r];
    s_q_r = (uint16_t)((s_q_r + 1u) % KEY_EVENT_QSIZE);
    return true;
}

static inline uint8_t read_pressed(GPIO_TypeDef *port, uint16_t pin)
{
    return (HAL_GPIO_ReadPin(port, pin) == KEY_ACTIVE_LEVEL) ? 1u : 0u;
}

static inline uint8_t read_key(KeyId_t id)
{
    return read_pressed(s_key_gpio[id].port, s_key_gpio[id].pin);
}

/* ===== Push Key Tick ===== */
static void push_tick_one(KeyId_t id, uint32_t now_ms)
{
    PushState_t *ps = &s_push[id];

    uint8_t raw = read_key(id);
    if (raw != ps->raw) {
        ps->raw = raw;
        ps->last_change_ms = now_ms;
    }

    if (ps->stable != ps->raw) {
        if ((now_ms - ps->last_change_ms) >= s_cfg.debounce_ms) {
            ps->stable = ps->raw;

            if (ps->stable) {
                ps->press_start_ms = now_ms;
                ps->long_fired = 0;
                ps->last_repeat_ms = now_ms;
                q_push(KEYEV_DOWN, id, 0);
            } else {
                q_push(KEYEV_UP, id, 0);
                if (!ps->long_fired) q_push(KEYEV_CLICK, id, 1);
            }
        }
    }

    if (ps->stable) {
        uint32_t held = now_ms - ps->press_start_ms;

        if (!ps->long_fired && held >= s_cfg.longpress_ms) {
            ps->long_fired = 1;
            q_push(KEYEV_LONG, id, 1);
        }

        if (s_cfg.repeat_start_ms > 0 && held >= s_cfg.repeat_start_ms) {
            if ((now_ms - ps->last_repeat_ms) >= s_cfg.repeat_period_ms) {
                ps->last_repeat_ms = now_ms;
                q_push(KEYEV_REPEAT, id, 1);
            }
        }
    }
}

bool KeyInput_IsPressed(KeyId_t id)
{
    if (id >= KEY_COUNT) return false;
    return (s_push[id].stable != 0);
}

/* ===== Rotary Quadrature =====
 * table-based decoder (noise robust)
 * detent = 4 step 가정 (필요시 변경)
 */
static volatile int16_t s_enc_diff;
static uint8_t s_enc_prev = 0;
static int16_t s_enc_acc  = 0;     // 내부 누적(4step 모아서 detent)

static const int8_t s_qdec_table[16] = {
     0, -1, +1,  0,
    +1,  0,  0, -1,
    -1,  0,  0, +1,
     0, +1, -1,  0
};

static void rotary_tick(void)
{
    uint8_t a = read_pressed(s_encA.port, s_encA.pin);
    uint8_t b = read_pressed(s_encB.port, s_encB.pin);
    uint8_t cur = (uint8_t)((a << 1) | b);

    uint8_t idx = (uint8_t)((s_enc_prev << 2) | cur);
    int8_t step = s_qdec_table[idx];

    if (step != 0) {
        s_enc_acc += step;

        // detent=4step
        if (s_enc_acc >= 4) {
            s_enc_acc = 0;
            s_enc_diff += 1;
            q_push(KEYEV_ROT_CW, KEY_LOCK, +1);
        } else if (s_enc_acc <= -4) {
            s_enc_acc = 0;
            s_enc_diff -= 1;
            q_push(KEYEV_ROT_CCW, KEY_LOCK, -1);
        }
    }

    s_enc_prev = cur;
}

int16_t KeyInput_EncoderGetDiffAndClear(void)
{
    int16_t d;

    uint32_t primask = __get_PRIMASK(); 
    __disable_irq();

    d = s_enc_diff;
    s_enc_diff = 0;

    if(!primask) __enable_irq();
    return d;
}

bool KeyInput_EncoderIsPressed(void)
{
    return KeyInput_IsPressed(KEY_LOCK);
}

/* ===== Public ===== */
void KeyInput_Init(const KeyConfig_t *cfg)
{
    if (cfg) s_cfg = *cfg;

    memset(s_push, 0, sizeof(s_push));
    s_q_w = s_q_r = 0;

    uint32_t now = HAL_GetTick();
    for (int i = 0; i < KEY_COUNT; i++) {
        uint8_t r = read_key((KeyId_t)i);
        s_push[i].raw = r;
        s_push[i].stable = r;
        s_push[i].last_change_ms = now;
    }

    uint8_t a = read_pressed(s_encA.port, s_encA.pin);
    uint8_t b = read_pressed(s_encB.port, s_encB.pin);
    s_enc_prev = (uint8_t)((a << 1) | b);
    s_enc_acc = 0;
    s_enc_diff = 0;
}

void KeyInput_Tick(uint32_t now_ms)
{
    for (int i = 0; i < KEY_COUNT; i++) {
        push_tick_one((KeyId_t)i, now_ms);
    }
    rotary_tick();
}
