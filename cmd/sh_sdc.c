
#include "FreeRTOS.h"
#include "task.h"


#ifdef PAKAI_SDCARD

#include "monita.h"
#include "ff/ff9b/src/ff.h"
#include "ff/sdc.h"
#include "sh_sdc.h"

//extern FATFS *FatFs[];


void sh_cek_status_sdc()	{
	qsprintf("status : %s\r\n", (cek_status_sdc()==0)?"AKTIF":"MATI" );
}

void mount_disk(unsigned char disk)	{
	FATFS xFatFs[2];
	char strFs[24];
	unsigned char a;
	a = f_mount(disk, &xFatFs);
	//a = f_mount(disk, &xFatFs[0]);
	ket_fs(a, &strFs);
	//qsprintf("a: %d-%s, xFatFs.drive: %d, size: %d\r\n", a, ket_fs(a), xFatFs[0].drv, xFatFs[0].csize);
	qsprintf("a: %d-%s\r\n", a, strFs);
}

void sh_cek_read_sdc(void)	{
	unsigned char data[2];
	qsprintf("====> baca %s()\r\n", __FUNCTION__);
	sdc_read(data, 0, 1);
}

void sh_cek_free_cluster()	{
	FATFS *fs;
	char ketfs[25];
    DWORD fre_clust, fre_sect, tot_sect;

    /* Get volume information and free clusters of drive 1 */
    unsigned char res;
    res = f_getfree("0:", &fre_clust, &fs);
    ket_fs(res, &ketfs);
    uprintf("res: %d-%s\r\n", res, ketfs);
    if (res) {
		return;
		//die(res);
	}

    /* Get total sectors and free sectors */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    /* Print free space in unit of KiB (assuming 512 bytes/sector) */
	uprintf("%lu KB total drive space.\n%lu KB available.\n",
           tot_sect / 2, fre_sect / 2);
}

void sh_baca_sektor(int argc, char **argv)		{
	if (argc!=3)	{
		qsprintf("argumen KURANG, baca_sektor [no] [Nsektor]\r\n");
	}
	
	int skr=atoi(argv[1]);
	int pjg=atoi(argv[2]);
	char isi[512];
	qsprintf("---> baca_sektor %d, sebanyak %d\r\n", skr, pjg);
	vTaskDelay(10);
	sdc_read(isi, skr, pjg);
}
#endif
