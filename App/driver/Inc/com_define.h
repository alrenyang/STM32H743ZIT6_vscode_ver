#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define 	LCD_FRAME_BUFFER_ADDR  0xC0000000
#define     HW_SDRAM_MEM_ADDR      0xC0000000
#define     HW_SDRAM_MEM_SIZE      (8*1024*1024)
#define 	SDRAM_TIMEOUT          ((uint32_t)0xFFFF)


#define VERSION_MAGIC_NUMBER      0x56455220    // "VER "
#define _DEF_FIRMWATRE_VERSION    "V260122R1"
#define _DEF_BOARD_NAME           "STM32H743-TRI"