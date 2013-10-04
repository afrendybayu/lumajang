

// Afrendy Bayu
// Depok, 27 Sept 2013
// 


#include "FreeRTOS.h"
#include "task.h"
#include "ff/fatfs/ff.h"
#include "monita.h"
#include "ap_file.h"
#include <time.h>

#ifdef PAKAI_FILE_SIMPAN

//#define FORMAT_SIMPAN_STRING

void sendHexFile(int nilai, int jml, FIL fp)	{
	int i;
	char s[4];
	memcpy(s, (void*) &nilai, jml);
	f_write(&fp, s, jml, &i);
	
	#if 0
	for (i=0; i<jml; i++)	{
		fputc(s[i], fp);
	}
	#endif
}

void simpan_file_data()		{
	FIL filx;
	FRESULT res;
	DIR dir;
	char st[50], st2[50], s[4], fol[20];
	char isi[256];
	int i=0, oo, menit;
	unsigned int wx;
	int j, k;
	
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	//rtcCTIME2_t ctime2;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	//ctime2.i = RTC_CTIME2;
	
	//uprintf("waktu: %d%02d%02d_%02d%02d\r\n", ctime1.year, ctime1.month, ctime1.dom, ctime0.hours, ctime0.minutes );
	menit =  (ctime0.minutes<30)?0:30;
	sprintf(fol, "0:\\%d%02d%02d", ctime1.year, ctime1.month, ctime1.dom);
	sprintf(st, "%s\\%d%02d%02d_%02d%02d.str", fol, ctime1.year, ctime1.month, ctime1.dom, ctime0.hours, menit);
	
	
	res = f_opendir(&dir, fol);
	//uprintf("res buka folder: %d\r\n", res);
	
	if (res != FR_OK)	{
		res = f_mkdir(fol);
		//uprintf("res buat folder: %d\r\n", res);
	}
	
	struct t_file  *st_file;
	st_file = (char *) ALMT_FILE;
	
	struct t_data *st_data;
	
	res = f_open(&filx, st, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
	if (res)	{		// GAGAL buka, bikin baru !!!
		res = f_open(&filx, st, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			uprintf("res: %d\r\n", res);
			return;
			//die(res);
		} else {
			// tambahan header untuk file baru !! -- disini --
			isi[0] = '\r'; isi[1] = '\n';
			for (i=0; i<st_file->jml; i++)	{
				j = (int) ((st_file->urut[i]-1)/PER_SUMBER);
				k = (st_file->urut[i]-1) % PER_SUMBER;
				st_data = ALMT_DATA + j*JML_KOPI_TEMP;
				
				memcpy((void*) &isi[2*i+2], (void*) &st_data[k].id, 2);
			}
			f_write(&filx, isi, (st_file->jml*4+2), &oo);
		}
	} else {		// file sudah ada
		res = f_lseek(&filx, f_size(&filx));
	}
	
	time_t wkt;
	struct tm a;
	//res = f_lseek(&filx, f_size(&filx));
	//f_write(&filx, "cobacoba\r\n", 10, &i);
	for (i=0; i<st_file->jml; i++)	{
		#ifdef FORMAT_SIMPAN_STRING
			if (i==0)	{
				sprintf(isi, "%ld ", now_to_time(1, a));
			}
			sprintf(st2, "%.1f ", data_f[st_file->urut[i]-1]);
			strcat(isi, st2);
		
		#else
			if (i==0)	{
				isi[0] = '\r'; isi[1] = '\n';
				wx = (unsigned int) now_to_time(1, a);
				memcpy(&isi[2], (void*) &wx, 4);
			}
			
			memcpy((void*) &isi[4*i+6], (void*) &data_f[st_file->urut[i]-1], 4);
			//f_write(&filx, s, 4, &oo);
		#endif

		//sendHexFile(data_f[10+i], 4, filx);
	}
	#ifdef FORMAT_SIMPAN_STRING
		//uprintf("%s", isi);
		f_puts(isi, &filx);
	#else
		f_write(&filx, isi, (st_file->jml*4+6), &oo);
	#endif
	
	//f_sync(&filx);
	f_close(&filx);
}

void tulis_konfig_file(char *s, FIL* fp)	{
	char ss[50];
	int jml, i;
	
	sprintf(ss, "%s\r\n", s);
	jml = strlen(ss);
	//uprintf("%s: jml: %d\r\n", ss, jml);
	f_write(fp, ss, jml, &i);
}

int simpan_konfig(int argc, char **argv)		{
	FIL filx;
	FRESULT res;
	char st1[50], st2[50], s[4];
	int i, j, oo;
	
	if (argc==1)	{
		//file_kitab(0);
		uprintf("\r\n  argc = 1\r\n");
		return;
	}
	
	uprintf("agv[1] : %s\r\n", argv[1]);
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	
	sprintf(st2, "%d%02d%02d %02d:%02d:%02d", \
		ctime1.year, ctime1.month, ctime1.dom, ctime0.hours, ctime0.minutes, ctime0.seconds);
	res = f_open(&filx, argv[1], FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    if (res) {
		uprintf("\r\n  res: %d\r\n", res);
		return;
		//die(res);
	}
	
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	//uprintf("--> %s\r\n", __FUNCTION__);

	tulis_konfig_file("[env]", &filx);
	sprintf(st1, "#tanggal = %s", st2);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "nama = %s", st_env->nama_board);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "sn = %s", st_env->SN);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "idslave = %d", st_env->almtSlave);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "debug1 = %d", st_env->prioDebug);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "debug2 = %d", st_env->prioDebug2);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "#board = %s %s", PROMPT, BOARD_SANTER_versi);
	tulis_konfig_file(st1, &filx);
	tulis_konfig_file("", &filx);
	
	tulis_konfig_file("[kanal]", &filx);
	for (i=0; i<(JML_KANAL*2); i++)	{
		sprintf(st1, "kalib%d = %.3f %.3f", i+1, st_env->kalib[i].m, st_env->kalib[i].C);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "status%d = %d", i+1, st_env->kalib[i].status);
		tulis_konfig_file(st1, &filx);
	}
	tulis_konfig_file("", &filx);
	
	struct t_sumber *st_sumber;
	st_sumber = (char *) ALMT_SUMBER;
	
	tulis_konfig_file("[sumber]", &filx);
	for (i=0; i<JML_SUMBER; i++)	{
		sprintf(st1, "nama%d = %s", i+1, st_sumber[i].nama);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "ip%d = %d.%d.%d.%d", i+1, st_sumber[i].IP0, st_sumber[i].IP1, st_sumber[i].IP2, st_sumber[i].IP3);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "alamat%d = %d", i+1, st_sumber[i].alamat);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "stack%d = %d", i+1, st_sumber[i].stack);
		tulis_konfig_file(st1, &filx);
	}
	tulis_konfig_file("", &filx);
	
	struct t_data *st_data;
	tulis_konfig_file("[data]", &filx);
	for (i=0; i<JML_SUMBER; i++)	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		
		for (j=0; j<PER_SUMBER; j++)	{
			sprintf(st1, "nama%d = %s", i*PER_SUMBER+j+1, st_data[j].nama);
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "satuan%d = %s", i*PER_SUMBER+j+1, st_data[j].satuan);
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "status%d = %s", i*PER_SUMBER+j+1, st_data[j].status?"Aktif":"Mati");
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "batas%d = %d %d %d %d %d %d", i*PER_SUMBER+j+1, st_data[j].rangeL, \
					st_data[j].batasLL, st_data[j].batasL, 	\
					st_data[j].batasH, st_data[j].batasHH, st_data[j].rangeH);
			tulis_konfig_file(st1, &filx);
		}
	}
	tulis_konfig_file("", &filx);
	
	struct t_file  *st_file;
	st_file = (char *) ALMT_FILE;
	tulis_konfig_file("[file]", &filx);
	sprintf(st1, "jumlah = %d", st_file->jml);
	tulis_konfig_file(st1, &filx);
	for (i=0; i<JML_TITIK_DATA; i++)	{
		if (st_file->urut[i]==0)	break;
		sprintf(st1, "urut%d = %d", i+1, st_file->urut[i]);
		tulis_konfig_file(st1, &filx);
	}
	
	f_close(&filx);
}


#endif
