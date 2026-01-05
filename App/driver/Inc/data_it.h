#ifndef DATA_IT_H
#define DATA_IT_H

#include <stdint.h>

#define CH_COUNT 16

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



