
#include "FreeRTOS.h"
#include "monita.h"
#include "mb.h"

#ifdef PAKAI_MODBUS

extern volatile float data_f[];

int cek_crc_mod(int nstr, unsigned char *x)	{
	unsigned char lo, hi;

	unsigned int i, Crc = 0xFFFF;
	for (i=0; i<(nstr-2); i++) {
		Crc = CRC16 (Crc, x[i]);
	}
	hi = ((Crc&0xFF00)>>8);	lo = (Crc&0xFF);
	printf("CRC Modbus: %04X\r\n", Crc);
	
	if (hi==x[nstr-1] && lo==x[nstr-2]) {
		//printf("SIP crc\r\n");
		return 1;
	}
	return 0;
}

unsigned int CRC16(unsigned int crc, unsigned int data)		{
	const unsigned int Poly16=0xA001;
	unsigned int LSB, i;
	crc = ((crc^data) | 0xFF00) & (crc | 0x00FF);
	for (i=0; i<8; i++) {
		LSB = (crc & 0x0001);
		crc = crc/2;
		if (LSB)
			crc=crc^Poly16;
	}
	return(crc);
}

int respon_modbus(int cmd, int reg, int jml, char *str)	{
	//printf("-->%s, cmd: %02x=%d, reg: %04x=%d, jml: %d\r\n\r\n", __FUNCTION__, cmd, cmd, reg, reg, jml);
	int i=0, j, index=0;
	char ketemu=0;
	
	struct t_data *st_data;
	do	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		//printf("i: %d, st_data: %08x\r\n", i, st_data);
		for (j=0; j<PER_SUMBER; j++)	{
		//	printf("id: %d, reg: %d\r\n", st_data[j].id, reg);
			if (st_data[j].id == reg)	{
		//		printf("ketemu: %d\r\n", i*PER_SUMBER+j);
				ketemu=1;
				index = i*PER_SUMBER+j;
				break;
			}
		}
		i++;
		if (i>4)	return 1;
	} while (ketemu==0);
	printf("Data index: %d\r\n", index);

	
	if (cmd==READ_HOLDING_REG)		{
		baca_reg_mb(index, jml);
	}
	if (cmd==WRITE_MULTIPLE_REG)	{
		tulis_reg_mb(reg, index, jml, str);
	}
	if (cmd==READ_FILE_NAME)		{
		
	}
	
	if (cmd==READ_FILE_CONTENT)		{
		
	}
	return 0;
}

int baca_reg_mb(int index, int jml)	{			// READ_HOLDING_REG
	int i, nX;
	char *respon; 
	
	nX = jml_st_mb3H(jml);
	respon = pvPortMalloc( nX );
	
	if (respon == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (respon);
		return 1;
	}
	printf("Alok: %d @%#08x\r\n", nX, respon);
	
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	respon[0] = st_env->almtSlave;
	respon[1] = READ_HOLDING_REG;
	respon[2] = jml*4;
	
	unsigned int *ifl;
	for (i=0; i<jml; i++)	{
		if (1)	{		// almt ROM lintas struct (kelipatan 10)
			
		}
		
		ifl = (unsigned int *) &data_f[index+i];
		printf("  data[%d]: %.2f = 0x%08x\r\n", index+i, data_f[index+i], *ifl);
		respon[3+i*4] = (unsigned char) (*ifl>>24) & 0xff;
		respon[4+i*4] = (unsigned char) (*ifl>>16) & 0xff;
		respon[5+i*4] = (unsigned char) (*ifl>> 8) & 0xff;
		respon[6+i*4] = (unsigned char) (*ifl & 0xff);
	}


	unsigned int Crc = 0xFFFF;
	for (i=0; i<(nX-2); i++)	{
		Crc = CRC16 (Crc, respon[i]);
	}
	respon[nX-1] = ((Crc&0xFF00)>>8);	
	respon[nX-2] = (Crc&0xFF);
	
	printf("===> Respon: ");
	for (i=0; i<nX; i++)		{
		printf(" %02x", respon[i]);
		// kirim Serial2 modbus
		//xSerialPutChar2 (0, respon[i], 0xffff);
	}
	printf("\r\n");
	
	vPortFree (respon);
	return 0;
}

int tulis_reg_mb(int reg, int index, int jml, char* str)	{	// WRITE_MULTIPLE_REG: 16
	int i, j, tmpFl;
	char *respon; 
	
	//************  PROSESS  ************//
	int nby, byte = str[6];
	if (byte%4!=0)	{
		printf("===> Data 32bit tidak VALID\r\n");
		return 1;
	}
	
	float *fl;
	for (i=0; i<jml; i++)	{
		tmpFl = (str[7+i*4]<<24) | (str[8+i*4]<<16) | (str[9+i*4]<<8) | (str[10+i*4]) ;
		fl = (float *)&tmpFl;
		printf("data[%d]: %.3f, %#08x\r\n", index+i, *fl, tmpFl);

		data_f[index+i] = *fl;		
	}
	
	
	//************   BALAS   ************//
	
	respon = pvPortMalloc( jml_st_mb10H );
	
	if (respon == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (respon);
		return 1;
	}
	printf("Alok: %d @%#08x\r\n", jml_st_mb3H(jml), respon);

	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	respon[0] = st_env->almtSlave;
	respon[1] = WRITE_MULTIPLE_REG;
	respon[2] = (reg>>8) & 0xff;
	respon[3] = (reg & 0xff);
	respon[4] = (jml>>8) & 0xff;
	respon[5] = (jml & 0xff);
	
	unsigned int Crc = 0xFFFF;
	for (i=0; i<(jml_st_mb10H-2); i++)	{
		Crc = CRC16 (Crc, respon[i]);
	}
	respon[jml_st_mb10H-1] = ((Crc&0xFF00)>>8);	
	respon[jml_st_mb10H-2] = (Crc&0xFF);
	
	printf("===> Respon: ");
	for (i=0; i<jml_st_mb10H; i++)		{
		printf(" %02x", respon[i]);
		// kirim Serial2 modbus
		//xSerialPutChar2 (0, respon[i], 0xffff);
	}
	printf("\r\n");
	
	vPortFree (respon);
	return 0;
}

#endif
