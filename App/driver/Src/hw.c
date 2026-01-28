#include "hw.h"
#include <stdio.h>

extern uint32_t _fw_flash_begin;
extern uint32_t _fw_flash_end;

volatile const firm_ver_t firm_ver __attribute__((section(".version"))) = 
{
  .magic_number = VERSION_MAGIC_NUMBER,
  .version_str  = _DEF_FIRMWATRE_VERSION,
  .name_str     = _DEF_BOARD_NAME,
  .firm_addr    = (uint32_t)&_fw_flash_begin
};


void hwInit(void)
{
  printf("FW Addr Begin \t\t: 0x%X\r\n", (unsigned int)_fw_flash_begin);  
  printf("FW Addr End   \t\t: 0x%X\r\n", (unsigned int)_fw_flash_end);  
  printf("FW Addr Size  \t\t: %d bytes\r\n", (int)&_fw_flash_end - (int)&_fw_flash_begin);  
  printf("\n");
}