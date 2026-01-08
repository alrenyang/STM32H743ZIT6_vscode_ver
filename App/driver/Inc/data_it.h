#ifndef DATA_IT_H
#define DATA_IT_H

#include <stdint.h>

#define CH_COUNT 16

typedef enum{
    interlock_1     = 1,   //interlock mode 1
    interlock_2     = 2,   //interlock mode 2
    interlock_4     = 4,   //interlock mode 4
    interlock_8     = 8,   //interlock mode 8
    interlock_16    = 16,   //interlock mode 16
}interlock_t;

typedef struct{
    uint8_t gete_way_ip[4];
    uint8_t user_ip[4];
} tcpip_t;

typedef struct{
    uint8_t slot;
} slot_t;

typedef struct{
    uint8_t repeat;
} repeat_t;

typedef struct {
    uint16_t on;
    uint16_t delay;
    uint16_t block;
    char     trg;   // 'F', 'R', 'B'
} ch_data_t;

/*역 채널 데이터 선언 (정의 아님) */
extern ch_data_t g_ch[CH_COUNT];

void update_ch_data(void);
void data_it_on_close_apply(uint16_t sel_row);
void data_it_save_to_flash(void); 

#endif /* DATA_IT_H */



