
#include "monita.h"

#define jml_st_mb10H		(8)
#define jml_st_mb3H(no)		(3+no*4+2)	

#define READ_COIL_STATUS		 1
#define READ_INPUT_STATUS		 2
#define READ_HOLDING_REG		 3
#define READ_INPUT_REG			 4
#define WRITE_SINGLE_COIL		 5
#define WRITE_SINGLE_REG		 6
#define WRITE_MULTIPLE_COIL		15
#define WRITE_MULTIPLE_REG		16
#define READ_FILE_NAME			24
#define READ_FILE_CONTENT		25



int cek_crc_mod(int nstr, unsigned char *x);
unsigned int CRC16(unsigned int crc, unsigned int data)	;
int respon_modbus(int cmd, int reg, int jml, char *str);
int tulis_reg_mb(int reg, int index, int pjg, char* str);
int baca_reg_mb(int reg, int pjg);
//int tulis_single_mb(int reg, int index, char* str);
