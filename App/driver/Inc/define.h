#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define 	LCD_FRAME_BUFFER_ADDR  0xC0000000
#define     HW_SDRAM_MEM_ADDR      0xC0000000
#define     HW_SDRAM_MEM_SIZE      (8*1024*1024)
#define 	SDRAM_TIMEOUT          ((uint32_t)0xFFFF)