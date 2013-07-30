
#include "FreeRTOS.h"
#include "task.h"
//#include "serial/tinysh.h"
#include "sh_hardware.h"
#include "cpu_setup.h"
#include "monita.h"
#include "iap.h"
#include "manual.h"
#include "sh_utils.h"

extern struct t_st_hw st_hw;



void idle_tick()	{
	qsprintf("\r\n  idle tick : %d/s, adc: %d/s\r\n", st_hw.idle, st_hw.adc_pd);
}

int ukuran_rom (char no)	{
	if (no>=0 && no<=JML_SECTOR_LPC-1)	{
		if (no>7 && no<22)		return	32;
		else 					return  4;
	} else {
		return 0;
	}
}

void cek_blank_sector_rom()		{
	IAP_return_t iap_return;
	uchr i;
	uprintf("\r\n  Cek Kondisi Sektor ROM\r\n  ***********************************************\r\n");
	for (i=0; i<JML_SECTOR_LPC; i++)	{
		iap_return = iapReadBlankSector(i, i);
		if (iap_return.ReturnCode == 0)	{
			uprintf("  Sektor %2d kosong[%2dKB] : %08X - %04X\r\n", i, ukuran_rom(i), iap_return.Result[0], iap_return.Result[1]);
		} else if (iap_return.ReturnCode == SRC_ADDR_ERROR) {
			uprintf("  Sektor %2d ISI[2] : %08X - %04X\r\n", i, iap_return.Result[0], iap_return.Result[1]);
		} else if (iap_return.ReturnCode == SECTOR_NOT_BLANK) {
			uprintf("  Sektor %2d ISI[NOT_BLANK] : mulai sektor %02d - %04X\r\n", i, iap_return.Result[0], iap_return.Result[1]);
		} else {
			uprintf("  Error no : %d\r\n", iap_return.ReturnCode);
		}
	}
}

void hapus_sector_rom(int argc, char **argv)			{
	printf("\r\n");
	if (argc != 2)	{
		//iap_kitab();
		return;
	}

	// menghindari argumen ngawur/bukan angka
	if (atoi(argv[1])==0)	return;
	
	int nSktr = cek_nomor_valid(argv[1], JML_SECTOR_LPC-1);
	hapuskan_sektor(nSktr);
	#if 0
	IAP_return_t iap_return = iapSiapSektor(nSktr, nSktr);
	
	if ( iap_return.ReturnCode == CMD_SUCCESS )	{
		printf("  SS%d", nSktr);
		iap_return = iapEraseSector(nSktr, nSktr);
	}

	if (iap_return.ReturnCode == CMD_SUCCESS)	
		printf("  HS%d.\r\n", nSktr);
	else
		printf("  GAGAL Hapus Sektor %d !!\r\n", nSktr);
	#endif
}


void simpan_sector_rom(int argc, char **argv)			{
	printf("\r\n");

	char *pdata, *pch;
	int jml = 0, nn=256, sektor = SEKTOR_ENV;
	int ndata=-1, st=-1;
	
	char* istilah[] = {"data", "env", "sumber", "cron", NULL};
	int i = parsing_istilah(argv[1], istilah);
	if (i>=0)	{
		if ( argc>2 && strcmp("data", argv[1])==0 )	{
			ndata = atoi(argv[2]);
			//printf("Simpan data sumber: %d!!\r\n", ndata);
		} else {
			st = i;
			//printf("Simpan: %d!!\r\n", st);
		}
	} else return;


	unsigned int alamat = 1024*(st-1) + ALMT_SEKTOR_21;
	if (st==SUMBER)		{
		jml = cek_jml_struct(SUMBER)*JML_SUMBER;
		pch = &st_sumber[0];
		printf("  Simpan SumberROM\r\n");
	}
	else if (st==ENV)	{
		jml = cek_jml_struct(ENV);
		pch = &st_env;
		printf("  Simpan EnvROM\r\n");
	}

	if (ndata>=0)	{
		alamat = 1024*ndata + ALMT_SEKTOR_20;
		sektor = SEKTOR_DATA;
		jml = cek_jml_struct(DATA)*PER_SUMBER;
		pch = &st_data[0];
		printf("  Simpan DataROM\r\n");
	}

	if (jml > 256)  nn =  512;
	if (jml > 512)  nn = 1024;
	if (jml > 1024) nn = 4096;
	
	pdata = pvPortMalloc(jml);
	if (pdata!=NULL)	{
		taskENTER_CRITICAL();
		memcpy(pdata, pch, jml);
		taskEXIT_CRITICAL();
		printf("  memSIP : %d/%d, Alm: 0x%08X.\r\n", jml, nn, alamat);
	} else {
		printf("  GAGAL alokasi memori !!\r\n");
		vPortFree (pdata);
		return;
	}

	simpan_data_rom(sektor, alamat, (unsigned short *)pdata, nn);	
	
	printf("  data: ");
	for (i=0; i<10; i++)	{
		printf("%02X ", pdata[i]);
	}
	printf("\r\n");
	
	vPortFree (pdata);
}

void baca_rom()	{
	struct t_env *penv;
	penv = (char *) ALMT_ENV;
	
	printf("\r\n  SN: %s, prio: %d\r\n", penv->SN, penv->prioDebug);
	
	int i;
	char *envx;
	envx = (char *) ALMT_ENV;
	printf("  data: ");
	for (i=0; i<10; i++)	{
		printf("%02X ", envx[i]);
	}
	printf("\r\n");
}
