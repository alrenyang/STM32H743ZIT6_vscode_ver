
#include "data_it.h"
#include <stdint.h>


/* 전역 채널 데이터 실체 */
ch_data_t g_ch[CH_COUNT] =
{
    {100, 0, 0, 'F'},
    {300, 0, 0, 'F'},
    {200, 0, 0, 'F'},
    {400, 0, 0, 'F'},
    {3200, 0, 6666, 'F'},
    {2400, 0, 8888, 'F'},
    {100, 0, 9999, 'F'},
    {200, 1, 0, 'F'},
    {100, 2, 0, 'F'},
    {100, 3, 0, 'F'},
    {100, 0, 500, 'F'},
    {100, 3, 0, 'F'},
    {100, 0, 4, 'F'},
    {100, 4, 0, 'F'},
    {100, 0, 3, 'F'},
    {100, 0, 2, 'F'}
};


void update_ch_data(void)
{

}

void data_it_on_close_apply(uint16_t sel_row)
{
    (void)sel_row;
    // 여기서 “CLOSE 시점에 해야 할 일” 수행
    // 예: 변경된 값 검증, CRC 업데이트, dirty flag clear, 로그 등
}

void data_it_save_to_flash(void)
{
    // 플래시에 저장하는 코드 호출
}

