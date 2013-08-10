
#include "FreeRTOS.h"
#include "task.h"
#include "iap.h"
#include "monita.h"

#include <stdio.h>
#include <stdarg.h>
 
unsigned int param_table[5];

//typedef void (*IAP)(unsigned int [],unsigned int[]);

void iap_entry(unsigned int param_tab[], unsigned int result_tab[])		{
	IAP_return_t iap_return;
	void (*iap)(unsigned int[], unsigned int[]);
	
	iap = (void (*)(unsigned int[], unsigned int[]))IAP_ADDRESS;

	taskENTER_CRITICAL();
	iap(param_tab,result_tab);
	taskEXIT_CRITICAL();
}

IAP_return_t iapReadSerialNumber(void)	{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_READPARTID;

	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapReadBootVersion(void)	{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_READBOOTCODEVERSION;
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapReadBlankSector(uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_BLANKCHECKSECTOR;
	param_table[1] = awal;
	param_table[2] = akhir;
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapPrepareSector(uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_PREPARESECTORFORWRITE;
	param_table[1] = awal;
	param_table[2] = akhir;
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapJob(uchr iapcmd, uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = iapcmd;
	param_table[1] = awal;
	param_table[2] = akhir;

	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapEraseSector(uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_ERASESECTORS;
	param_table[1] = awal;
	param_table[2] = akhir;
	param_table[3] = (configCPU_CLOCK_HZ/configTICK_RATE_HZ);
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapCopyMemorySector(unsigned int addr, unsigned short * data, int pjg)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	
	//printf("\r\n%s cmd: %d, addr: %d, pjg: %d\r\n", __FUNCTION__, IAP_CMD_COPYRAMTOFLASH, addr, pjg);
	
	param_table[0] = IAP_CMD_COPYRAMTOFLASH;
	param_table[1] = addr;
	param_table[2] = (unsigned short*) &data[0];
	param_table[3] = pjg;
	param_table[4] = (configCPU_CLOCK_HZ/configTICK_RATE_HZ);
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

char hapuskan_sektor(int sektor)	{
	//int nSktr = cek_nomor_valid(sektor, JML_SECTOR_LPC-1);
	int nSktr = sektor;
	IAP_return_t iap_return = iapSiapSektor(nSktr, nSktr);
	
	if ( iap_return.ReturnCode == CMD_SUCCESS )	{
		printf("  SS[%d]", nSktr);
		iap_return = iapEraseSector(nSktr, nSktr);
	} 
	else	{
		return 1;
	}

	if (iap_return.ReturnCode == CMD_SUCCESS)	
		printf(" HS[%d].\r\n", nSktr);
	else	{
		printf("  GAGAL Hapus Sektor %d !!\r\n", nSktr);
		return 2;
	}
	return 0;
}

int hitung_ram(int jml)	{
	int nn;
	
	if (jml > 256)  nn =  512;
	if (jml > 512)  nn = 1024;
	if (jml > 1024) nn = 4096;
	
	return nn;
}

char simpan_data_rom(int no, ...)	{
	int i, nn, sektor, addr, hapus;
	unsigned short *pdata;
	
	va_list vl;
	va_start(vl,no);
	//printf("jml no: %d\r\n", no);

	for (i=0; i<no; i++)	{
		if (i==0)	{
			sektor = va_arg(vl, int);			// sektor
			//printf("%d Sektor: %d\r\n", i, sektor);
		}
		else if (i==1)	{					// alamat memori
			hapus = va_arg(vl, int);
			if (hapus)	hapuskan_sektor(sektor);
			//printf("%d Hapus: %d\r\n", i, hapus);
		}
		else if (i%3==2)	{					// alamat memori
			addr = va_arg(vl, int);
			//printf("%d addr: %d\r\n", i, addr);
		}
		else if (i%3==0)	{					// jml data
			nn = va_arg(vl, int);
			//printf("%d jml: %d\r\n", i, nn);
		} else {								// datanya
			pdata = va_arg(vl, char*);
			//printf("%d data: %s\r\n", i, pch);
			printf("--->sktr: %d, addr: 0x%08X, n: %d/%d\r\n", sektor, addr, nn, hitung_ram(nn));
			simpan_rom(sektor, addr,  (unsigned short *)pdata, hitung_ram(nn));
		}
	}
	va_end(vl);	
}

int ukuran_rom (char no)	{
	if (no>=0 && no<=JML_SECTOR_LPC-1)	{
		if (no>7 && no<22)		return	32;
		else 					return  4;
	} else {
		return 0;
	}
}

unsigned int alamat_sektor(int sektor)	{
	unsigned int alm = 0;
	int i;
	
	if (sektor>=0 && sektor<JML_SECTOR_LPC)		{		
		for (i=0; i<sektor; i++)	{
			if (i<8)			alm += 0x1000;
			else if (i<22)		alm += 0x8000;
			else 				alm += 0x1000;
		}
		return alm;
	}
	return 0;
}

char kopikan_sektor_tmp(int ALAMAT_SEKTOR)	{
	int i, almt_data, almt_tuj, hasil;
	char *pch, *pdata;
	
	hapuskan_sektor(SEKTOR_TEMP);
	
	// pindah tiap 1KB * 32
	for (i=0; i<SUMBER_PER_SEKTOR; i++)		{
		almt_data = i*JML_KOPI_TEMP + ALAMAT_SEKTOR;
		pch = (char *) almt_data;
		
		almt_tuj = i*JML_KOPI_TEMP + ALMT_SKTR_TEMP;
		pdata = pvPortMalloc(JML_KOPI_TEMP);
		if (pch!=NULL)	{
			taskENTER_CRITICAL();
			memcpy((char *) pdata, (char *) pch, JML_KOPI_TEMP);
			taskEXIT_CRITICAL();
		} else {
			printf("  GAGAL alokmem !\r\n");
			vPortFree (pdata);
			return;
		}
	
		hasil = simpan_rom(SEKTOR_TEMP, almt_tuj,  (unsigned short *)pdata, JML_KOPI_TEMP);
		vPortFree (pdata);
		if (hasil>0) return 1;
	}
	return 0;
}

char simpan_rom(int sektor, unsigned int addr, unsigned short *data, int jml)	{
	//if (hapuskan_sektor(sektor)==CMD_SUCCESS)	
	//int nSktr = cek_nomor_valid(sektor, JML_SECTOR_LPC-1);
	int nSktr = sektor;
	IAP_return_t iap_return = iapSiapSektor(nSktr, nSktr);
	//printf("  ---> hasil siap: %d\r\n", iap_return.ReturnCode);
	if ( iap_return.ReturnCode == CMD_SUCCESS )		{
		printf("  SS[%d]", nSktr);
		iap_return = iapCopyMemorySector(addr, data, jml);
		//printf("  ---> hasil kopi: %d\r\n", iap_return.ReturnCode);
	} 
	else
		return 1;
	
	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		printf("  TS[%d].\r\n", nSktr);
	}
	else	{
		printf("  GAGAL SALIN S%d : %d\r\n", nSktr, iap_return.ReturnCode);
		return 2;
	}
	
	return 0;
}

char simpan_struct_block_rom(int sektor, int st, int flag, char *pdata)	{
	char *pdata1, *pdata2;
	char *pch1, *pch2;
	int hapus=0, i;
	int jml, jml1, jml2;
	unsigned almt;
	
	if (sektor == SEKTOR_ENV)		{
		//printf("sektor: %d, st: %d\r\n", sektor, st);
		jml = 2 + 3*1;
		jml1 = cek_jml_struct(ENV);
		jml2 = cek_jml_struct(SUMBER)*JML_SUMBER;

		// ENV diamankan
		printf("env");
		pdata1 = pvPortMalloc(jml1);
		if (pdata1!=NULL)	{
			taskENTER_CRITICAL();
			if (st == ENV)	{
				//memcpy((char *) pdata1, (char *) pdata, jml1);
			} else {
				//memcpy((char *) pdata1, (char *) ALMT_ENV, jml1);
			}
			taskEXIT_CRITICAL();
			//printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml1, ALMT_ENV);
		} else {
			printf("  GAGAL alokmem !\r\n");
			//vPortFree (pdata1);
			return 1;
		}
		printf(" end\r\n");
		
		// SUMBER
		printf("sumber");
		pdata2 = pvPortMalloc(jml2);
		if (pdata2!=NULL)	{
			taskENTER_CRITICAL();
			if (st == SUMBER) 	{
				memcpy((char *) pdata2, (char *) pdata, jml2);
			} else {
				memcpy((char *) pdata2, (char *) ALMT_SUMBER, jml2);
			}
			taskEXIT_CRITICAL();
			printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml2, ALMT_SUMBER);
		} else {
			printf("  GAGAL alokmem !\r\n");
			vPortFree (pdata1);
			vPortFree (pdata2);
			return 3;
		}
		printf(" end\r\n");
#if 0		
		hapuskan_sektor(SEKTOR_ENV);
		//simpan_data_rom(jml, SEKTOR_ENV, hapus, alm1, jml1, (unsigned short *)pch1, alm2, jml2, (unsigned short *)pch2);
		simpan_data_rom(jml, SEKTOR_ENV, hapus, ALMT_ENV,    jml1, (unsigned short *)pdata1);
		simpan_data_rom(jml, SEKTOR_ENV, hapus, ALMT_SUMBER, jml2, (unsigned short *)pdata2);
#endif
		vPortFree (pdata1);
		vPortFree (pdata2);
	}
	else if (sektor == SEKTOR_DATA)	{				// 10 data, per_sumber
		jml1 = cek_jml_struct(DATA)*PER_SUMBER;
		jml = 2 + 3*1;
		IAP_return_t iap_return = iapReadBlankSector(SEKTOR_DATA, SEKTOR_DATA);
		if ( (iap_return.ReturnCode==SECTOR_NOT_BLANK) && (st!=-1) )
			kopikan_sektor_tmp(SEKTOR_DATA);
		hapuskan_sektor(SEKTOR_DATA);
		
		for (i=0; i<JML_SUMBER; i++)	{
		//for (i=0; i<SUMBER_PER_SEKTOR; i++)	{
			almt = i*JML_KOPI_TEMP + ALMT_DATA;
			pdata1 = pvPortMalloc(JML_KOPI_TEMP);
			if ( (st==i) || (st==-1) )	{				// data yg berubah
				printf("-1/%d almt: 0x%08X\r\n", i, almt);
				if (pdata1!=NULL)	{
					taskENTER_CRITICAL();
					memcpy(pdata1, pdata, jml1);
					taskEXIT_CRITICAL();
					//printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml2, alm2);
				} else {
					printf("  GAGAL alokmem: %d !\r\n", i+1);
					vPortFree (pdata1);
					return 5;
				}
			} else {
				if (pdata1!=NULL)	{
					taskENTER_CRITICAL();
					memcpy(pdata1, (char *) ALMT_DATA, jml1);
					taskEXIT_CRITICAL();
					//printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml2, alm2);
				} else {
					printf("  GAGAL alokmem: %d !\r\n", i+1);
					vPortFree (pdata1);
					return 6;
				}
			}
			//simpan_data_rom(jml, SEKTOR_DATA, hapus, almt, JML_KOPI_TEMP, (unsigned short *)pdata1);
			simpan_rom(SEKTOR_DATA, almt,  (unsigned short *)pdata1, JML_KOPI_TEMP);
			vPortFree (pdata1);
		}
	}
	vPortFree (pdata1);
	vPortFree (pdata2);
	return 0;
}

void baca_konfig_rom()		{
	printf("Data env & sumber\r\n");
	IAP_return_t iap_return = iapReadBlankSector(ALMT_ENV, ALMT_ENV);
	if (iap_return.ReturnCode == SECTOR_NOT_BLANK)	{		// setting sudah ada
		
	}
	else if (iap_return.ReturnCode == CMD_SUCCESS)	{		// setting KOSONG
		
	} else {
		
	}
	printf("Data data\r\n");
}
