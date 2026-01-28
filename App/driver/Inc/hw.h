#include "com_define.h"

typedef struct 
{
  uint32_t magic_number;
  char     version_str[32];
  char     name_str[32];
  uint32_t firm_addr;
} firm_ver_t;

void hwInit(void);