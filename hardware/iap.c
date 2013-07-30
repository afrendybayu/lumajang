
#include "FreeRTOS.h"
#include "task.h"
#include "iap.h"
#include "monita.h"
 
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

IAP_return_t iapCopyMemorySector(unsigned int addr, char* data, int pjg)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	
	//printf("\r\n%s cmd: %d, addr: %d, pjg: %d\r\n", __FUNCTION__, IAP_CMD_COPYRAMTOFLASH, addr, pjg);
	
	param_table[0] = IAP_CMD_COPYRAMTOFLASH;
	param_table[1] = addr;
	param_table[2] = data;
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
		printf("  HS[%d].\r\n", nSktr);
	else	{
		printf("  GAGAL Hapus Sektor %d !!\r\n", nSktr);
		return 2;
	}
	return 0;
}

char simpan_data_rom(int sektor, unsigned int addr, char * data, int jml)	{
	//if (hapuskan_sektor(sektor)==CMD_SUCCESS)	
	//int nSktr = cek_nomor_valid(sektor, JML_SECTOR_LPC-1);
	int nSktr = sektor;
	IAP_return_t iap_return = iapSiapSektor(nSktr, nSktr);
	//printf("  ---> hasil siap: %d\r\n", iap_return.ReturnCode);
	if ( iap_return.ReturnCode == CMD_SUCCESS )		{
		printf("  SS[%d]", nSktr);
		iap_return = iapCopyMemorySector(addr, data, jml);
	//	printf("  ---> hasil kopi: %d\r\n", iap_return.ReturnCode);
	} 
	else
		return 1;
	
	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		printf("  TS[%d]. BERHASIL\r\n", nSktr);
	}
	else	{
		printf("  GAGAL SALIN S%d : %d\r\n", nSktr, iap_return.ReturnCode);
		return 2;
	}
	
	return 0;
}
