
#ifndef __SH_FILE__
#define __SH_FILE__



#ifdef PAKAI_SDCARD
void set_file_default();
void cek_file(int argc, char **argv);
char set_file(int argc, char **argv);


#ifdef PAKAI_TINYSH
#include "tinysh/tinysh.h"
	static tinysh_cmd_t cek_file_cmd={ 0,"cek_file","cek konfig modul","", cek_file,0,0,0 };
	static tinysh_cmd_t set_file_cmd={ 0,"set_file","set konfig modul","", set_file,0,0,0 };
#endif

#endif

#endif
