
#ifndef __AP_FILE__
#define __AP_FILE__


//#ifdef PAKAI_SDCARD
#include "FreeRTOS.h"
#include "tinysh/tinysh.h"

void simpan_file_data()	;
int simpan_konfig(int argc, char **argv);

static tinysh_cmd_t simpan_file_cmd={0,"simpan_file","simpan konfig ke file","[args]", simpan_konfig,0,0,0};

#endif
