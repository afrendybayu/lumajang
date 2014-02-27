
#include "FreeRTOS.h"
#include "task.h"

#include "sh_data.h"
#include "monita.h"
#include "sh_utils.h"
#include "manual.h"
#include "hardware.h"

//

extern volatile float data_f[];

void cek_data(int argc, char **argv)	{
	int i=0, j ;
	struct t_data *st_data;
	if (argc==2)	{
		int smb = atoi(argv[1]);
		if (smb==0 || smb>JML_SUMBER)		{
			printf("\r\n--> Sumber SALAH !!\r\n");
			return;
		}
		uprintf("\r\n    Cek data sumber: %d\r\n******************************************************\r\n", smb);
		uprintf ("  NO |   ID   |        Nama       |    Nilai    | Satuan | Status | \r\n");
		smb--;
		st_data = ALMT_DATA + smb*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)	{
			//printf("%d --> 0x%08X\r\n", i*PER_SUMBER+j, ALMT_DATA + i*JML_KOPI_TEMP);
			uprintf(" %3d | %6d | %-17s | %11.2f | %-6s | %6d | %d\r\n", 	\
				j+1, st_data[j].id, st_data[j].nama, data_f[smb*PER_SUMBER+j], \
					st_data[j].satuan, st_data[j].status, smb*PER_SUMBER+j);
		}
		return;
	}
	
	uprintf("\r\n    Cek data input modul    \r\n****************************************\r\n");
	uprintf ("  NO |   ID   |        Nama       |   Nilai   | Satuan | rangeL | batasLL | batasL | batasH | batasHH | rangeH | status | relay\r\n");
	for (i=0; i<JML_SUMBER; i++ ) 	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)	{
			//printf("%d --> 0x%08X\r\n", i*PER_SUMBER+j, ALMT_DATA + i*JML_KOPI_TEMP);
			#if 1
			uprintf(" %3d | %6d | %-17s | %9.1f | %-6s | %6d | %7d | %6d | %6d | %7d | %6d | %-6s | %d,%d,%d,%d\r\n", 	\
				i*PER_SUMBER+j+1, st_data[j].id, st_data[j].nama, data_f[i*PER_SUMBER+j], 		\
				st_data[j].satuan,  st_data[j].rangeL, st_data[j].batasLL, st_data[j].batasL, 	\
				st_data[j].batasH, st_data[j].batasHH, st_data[j].rangeH, st_data[j].status?"Aktif":"Mati",\
				st_data[j].relay_LL, st_data[j].relay_L, st_data[j].relay_H, st_data[j].relay_HH );
			#endif
		}
	}
}


char set_data(int argc, char **argv)		{
	//printf("\r\n");
	printf("%s(): argc %d\r\n", __FUNCTION__, argc);
	
	if (argc>4 || argc==1)
	{		
		data_kitab();
		return;	
	}	
	if (argc==2 || argc==3)	{
		printf("\r\n");
		if (strcmp(argv[1], "default") == 0)	{
			printf("  set_data dengan konfig default !\r\n");
			set_data_default();
			return 0;
		}
		else if (strcmp(argv[1], "idurut") == 0)	{
			struct t_data *st_data;
			
			st_data = pvPortMalloc( PER_SUMBER * sizeof (struct t_data) );
			if (st_data == NULL)	{
				printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
				return 3;
			}
			//printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);
			memcpy((char *) st_data, (char *) ALMT_DATA, (PER_SUMBER * sizeof (struct t_data)));

			printf("set_data dengan id urut dari kanal 1: %d !\r\n", st_data[0].id);
			int i, awal = st_data[0].id;
			for (i=1; i<(PER_SUMBER); i++)		{
				st_data[i].id = st_data[0].id + i;
			}
			
			//simpan_st_rom(SEKTOR_DATA, lok, 1, (unsigned short *) st_data, 1);
			vPortFree (st_data);
		} else	{
			data_kitab();
			return 1;
		}
	}
	
	int no = cek_nomor_valid(argv[1], JML_SUMBER*PER_SUMBER);
		
	if (no == TIDAK_VALID)	{
		printf("  no sumber TIDAK VALID\r\n");
		return 2;
	}
	no--;
	int nox = no % PER_SUMBER;

	
	struct t_data *st_data;
	st_data = pvPortMalloc( PER_SUMBER * sizeof (struct t_data) );
	if (st_data == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_data);
		return 3;
	}
	
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, st_data);
	int lok = (int) (no/PER_SUMBER);
	
	printf("no: %d, lok: %d, nox: %d\r\n", no, lok, nox);
	
	taskENTER_CRITICAL();
	memcpy((char *) st_data, (char *) ALMT_DATA+(lok*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
	taskEXIT_CRITICAL();
	
	if (argc==4)	{
		printf("\r\n");
		if (strcmp(argv[2], "nama") == 0)	{
			printf("  set Nama Data ");
			if (strlen(argv[2]) > sizeof (st_data[nox].nama))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_data[nox].nama, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_data[nox].nama);
			}
		}
		else if (strcmp(argv[2], "formula") == 0)	{
			printf("  set Keterangan Data ");
			if (strlen(argv[2]) > sizeof (st_data[nox].formula))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_data[nox].formula, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_data[nox].formula);
			}
		}
		else if (strcmp(argv[2], "satuan") == 0)	{
			printf("  set satuan Data ");
			if (strlen(argv[2]) > sizeof (st_data[nox].satuan))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_data[nox].satuan, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_data[nox].satuan);
			}
		}
		else if (strcmp(argv[2], "id") == 0)	{
			st_data[nox].id = atoi( argv[3] );
			printf("  ID kanal-%d: %d\r\n", no+1, st_data[nox].id);
		}
		else if (strcmp(argv[2], "rangeL") == 0)	{
			st_data[nox].rangeL = atoi( argv[3] );
			printf("  RangeL: %d\r\n", st_data[nox].rangeL);
		}
		else if (strcmp(argv[2], "batasLL") == 0)	{
			st_data[nox].batasLL = atoi( argv[3] );
			printf("  Batas LL: %d\r\n", st_data[nox].batasLL);
		}
		else if (strcmp(argv[2], "batasL") == 0)	{
			st_data[nox].batasL = atoi( argv[3] );
			printf("  Batas L: %d\r\n", st_data[nox].batasL);
		}
		else if (strcmp(argv[2], "batasH") == 0)	{
			st_data[nox].batasH = atoi( argv[3] );
			printf("  Batas H: %d\r\n", st_data[nox].batasH);
		}
		else if (strcmp(argv[2], "batasHH") == 0)	{
			st_data[nox].batasHH = atoi( argv[3] );
			printf("  Batas HH: %d\r\n", st_data[nox].batasHH);
		}
		else if (strcmp(argv[2], "rangeH") == 0)	{
			st_data[nox].rangeH = atoi( argv[3] );
			printf("  Range H: %d\r\n", st_data[nox].rangeH);
		} 
		else if (strcmp(argv[2], "status") == 0)	{
			st_data[nox].status = atoi( argv[3] );
			printf("  Status: [%d] : %s\r\n", st_data[nox].status, st_data[nox].status?"Aktif":"Mati");
		} 
		else if (strncmp(argv[2], "relay_LL", 8) == 0)	
		{
			//st_data[nox].status = atoi( argv[3] );
			printf("OK akan set relayLL pada data %d dengan relay %d\r\n", nox+1, atoi( argv[3] ));
			st_data[nox].relay_LL = atoi( argv[3] );
		}
		else if (strncmp(argv[2], "relay_L", 7) == 0)	
		{
			//st_data[nox].status = atoi( argv[3] );
			printf("OK akan set relayL pada data %d dengan relay %d\r\n", nox+1, atoi( argv[3] ));
			st_data[nox].relay_L = atoi( argv[3] );
		}
		else if (strncmp(argv[2], "relay_HH", 8) == 0)	
		{
			//st_data[nox].status = atoi( argv[3] );
			printf("OK akan set relayHH pada data %d dengan relay %d\r\n", nox+1, atoi( argv[3] ));
			st_data[nox].relay_HH = atoi( argv[3] );
		}
		else if (strncmp(argv[2], "relay_H", 7) == 0)	
		{
			//st_data[nox].status = atoi( argv[3] );
			printf("OK akan set relay_H pada data %d dengan relay %d\r\n", nox+1, atoi( argv[3] ));
			st_data[nox].relay_H = atoi( argv[3] );
		}
		else {
			printf("  __ Perintah SALAH __\r\n");
			data_kitab();
			return;
		}
	}
	//printf("\r\n");
	
	simpan_st_rom(SEKTOR_DATA, lok, 1, (unsigned short *) st_data, 1);
	vPortFree (st_data);
	return 0;
}

void set_data_default()		{
	int i, j;
	
	for (j=0; j<JML_SUMBER; j++)	{
		struct t_data *st_data;
		st_data = pvPortMalloc( sizeof (struct t_data)*PER_SUMBER );
	
		if (st_data==NULL)	{
			printf("  GAGAL alokmem !");
			vPortFree (st_data);
			return;
		}
		
		printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_data);
		for (i=0; i<PER_SUMBER; i++)	{
			sprintf(st_data[i].nama, "data %d", j*PER_SUMBER+i+1);
			//st_data[i].id      = j*PER_SUMBER+i+1;
			st_data[i].id      = 0;
			st_data[i].rangeL  = -50;
			st_data[i].batasLL = 0;
			st_data[i].batasL  = 10;
			st_data[i].batasH  = 500;
			st_data[i].batasHH = 800;
			st_data[i].rangeH  = 1000;
			st_data[i].status  = 0;
			strcpy(st_data[i].satuan, "-");
			strcpy(st_data[i].formula, "");
		}
		if (j==0)		// hapus sektor dulu, flag=1
			simpan_st_rom(SEKTOR_DATA, j, 1, (unsigned short *) st_data, 0);
		else
			simpan_st_rom(SEKTOR_DATA, j, 0, (unsigned short *) st_data, 0);
		
		vPortFree (st_data);
	}
}

#if 0
static void set_relay(char nomer)
{
	if (nomer == 1)	sRelay1();
	else if (nomer == 2)	sRelay2();
	else if (nomer == 3)	sRelay3();
	else if (nomer == 4)	sRelay4();
	else if (nomer == 5)	sRelay5();
	else if (nomer == 6)	sRelay6();
	else if (nomer == 7)	sRelay7();
	else if (nomer == 8)	sRelay8();
}

static void unset_relay(char nomer)
{
	if (nomer == 1)	cRelay1();
	else if (nomer == 2)	cRelay2();
	else if (nomer == 3)	cRelay3();
	else if (nomer == 4)	cRelay4();
	else if (nomer == 5)	cRelay5();
	else if (nomer == 6)	cRelay6();
	else if (nomer == 7)	cRelay7();
	else if (nomer == 8)	cRelay8();
}
#endif

/* sementara saja, supaya relay bisa on off berdasarkan data 
	disiapkan untuk lumajang 

	dipanggil tiap detik oleh ap_ambilcepat.c
*/
#if 0
void cek_alarm_data()
{
	int i, j;
	struct t_data *st_data;

	for (i=0; i<JML_SUMBER; i++ ) 	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)	
		{
			/* cek apakah relay diset */
			if ( st_data[j].relay_LL)
			{ 			
				if (  data_f[i*PER_SUMBER+j] < (float) (st_data[j].batasLL * 1.0))	/* batasHH dalam integer */
				{
					printf("Data %d Melebihi batas LL, reset relay !\r\n", i*PER_SUMBER+j+1);
					printf("(%f < %d) \r\n", data_f[i*PER_SUMBER+j] , st_data[j].batasLL);

					set_relay( st_data[j].relay_LL );
				}
				else
					unset_relay( st_data[j].relay_LL );
			}	
			if ( st_data[j].relay_L)
			{ 			
				if (  data_f[i*PER_SUMBER+j] < (float) (st_data[j].batasL * 1.0))	/* batasHH dalam integer */
				{
					printf("Data %d Melebihi batas L, reset relay !\r\n", i*PER_SUMBER+j+1);
					printf("(%f < %d) \r\n", data_f[i*PER_SUMBER+j] , st_data[j].batasL);

					set_relay( st_data[j].relay_L );
				}
				else
					unset_relay( st_data[j].relay_L );	
			}	
			if ( st_data[j].relay_H)
			{ 			
				if (  data_f[i*PER_SUMBER+j] > (float) (st_data[j].batasH * 1.0))	/* batasHH dalam integer */
				{
					printf("Data %d Melebihi batas H, reset relay !\r\n", i*PER_SUMBER+j+1);
					printf("(%f > %d) \r\n", data_f[i*PER_SUMBER+j] , st_data[j].batasH);

					set_relay( st_data[j].relay_H );
				}
				else
					unset_relay( st_data[j].relay_H );	
			}	
			if ( st_data[j].relay_HH)
			{ 			
				if (  data_f[i*PER_SUMBER+j] > (float) (st_data[j].batasHH * 1.0))	/* batasHH dalam integer */
				{
					printf("Data %d Melebihi batas HH, reset relay !\r\n", i*PER_SUMBER+j+1);
					printf("(%f > %d) \r\n", data_f[i*PER_SUMBER+j] , st_data[j].batasHH);

					set_relay( st_data[j].relay_HH );
				}
				else
					unset_relay( st_data[j].relay_HH );	
			}	
		}
	}

}
#endif

#if 0
void set_data_default()		{
	int i, j;
	
	struct t_data *st_data;
	st_data = pvPortMalloc( sizeof (struct t_data)*PER_SUMBER );
	
	if (st_data==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_data);
		return;
	}
	
	for (i=0; i<PER_SUMBER; i++)	{
		sprintf(st_data[i].nama, "data %d", i+1);
		st_data[i].id      = 0;
		st_data[i].rangeL  = -50;
		st_data[i].batasLL = 0;
		st_data[i].batasL  = 10;
		st_data[i].batasH  = 500;
		st_data[i].batasHH = 800;
		st_data[i].rangeH  = 1000;
		st_data[i].status  = 0;
		strcpy(st_data[i].satuan, "-");
		strcpy(st_data[i].formula, "");
	}
	
	//i = simpan_struct_block_rom(SEKTOR_DATA, -1, (char *) st_data);
	simpan_st_rom(SEKTOR_DATA, DATA, 0, (unsigned short *) st_data);
	vPortFree (st_data);
	//printf("hsl: %d\r\n", i);
}
#endif
