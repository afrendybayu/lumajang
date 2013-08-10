
#include "FreeRTOS.h"
#include "task.h"

#include "sh_data.h"
#include "monita.h"
#include "sh_utils.h"
#include "manual.h"

//

//extern volatile float data_f[];

void cek_data(int argc, char **argv)	{
	int i, j ;
	struct t_data *st_data;
	
	uprintf("\r\n    Cek data input modul    \r\n****************************************\r\n");
	uprintf ("  NO |   ID   |        Nama       |   Nilai   | Satuan | rangeL | batasLL | batasL | batasH | batasHH | rangeH | status |\r\n");
	for (i=0; i<JML_SUMBER; i++ ) {
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)	{
			//printf("%d --> 0x%08X\r\n", i*PER_SUMBER+j, ALMT_DATA + i*JML_KOPI_TEMP);
			#if 1
			uprintf(" %3d | %6d | %-17s | %9.1f | %-6s | %6d | %7d | %6d | %6d | %7d | %6d | %-6s |\r\n", 	\
				i*PER_SUMBER+j+1, st_data[i*PER_SUMBER+j].id, st_data[i*PER_SUMBER+j].nama, data_f[i*PER_SUMBER+j], 		\
				st_data[i*PER_SUMBER+j].satuan,	st_data[i*PER_SUMBER+j].rangeL, 	\
				st_data[i*PER_SUMBER+j].batasLL, st_data[i*PER_SUMBER+j].batasL, 	\
				st_data[i*PER_SUMBER+j].batasH, st_data[i*PER_SUMBER+j].batasHH,	\
				st_data[i*PER_SUMBER+j].rangeH, st_data[i*PER_SUMBER+j].status?"Aktif":"Mati" );
			#endif
		}
	}
}


char set_data(int argc, char **argv)		{
	if (argc>4 || argc==1)		data_kitab();
	if (argc==2 || argc==3)	{
		printf("\r\n");
		if (strcmp(argv[1], "default") == 0)	{
			printf("set_data dengan konfig default !\r\n");
			set_data_default();
			return 0;
		}
		else if (strcmp(argv[1], "idurut") == 0)	{
			struct t_data *st_data;
			st_data = pvPortMalloc( JML_SUMBER*PER_SUMBER * sizeof (struct t_data) );
			if (st_data == NULL)	{
				printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
				return 3;
			}
			//printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);
			memcpy((char *) st_data, (char *) ALMT_DATA, (JML_SUMBER*PER_SUMBER * sizeof (struct t_data)));
	
			
			printf("set_data dengan id urut dari kanal 1: %d !\r\n", st_data[0].id);
			int i, awal = st_data[0].id;
			for (i=1; i<(JML_SUMBER*PER_SUMBER); i++)		{
				st_data[i].id = st_data[0].id + i;
			}
			vPortFree (st_data);
		} else	{
			data_kitab();
			return 1;
		}
	}
	
	int no = cek_nomor_valid(argv[1], PER_SUMBER);
	
	no--;
	if (no == TIDAK_VALID)	{
		printf("  no sumber TIDAK VALID\r\n");
		return 2;
	}
	
	struct t_data *st_data;
	st_data = pvPortMalloc( JML_SUMBER*PER_SUMBER * sizeof (struct t_data) );
	if (st_data == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return 3;
	}
	//printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_sbr);
	memcpy((char *) st_data, (char *) ALMT_DATA, (JML_SUMBER*PER_SUMBER * sizeof (struct t_data)));
	
	
	if (argc==4)	{
		printf("\r\n");
		if (strcmp(argv[2], "nama") == 0)	{
			printf("  set Nama Data ");
			if (strlen(argv[2]) > sizeof (st_data[no].nama))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_data[no].nama, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_data[no].nama);
			}
		}
		else if (strcmp(argv[2], "formula") == 0)	{
			printf("  set Keterangan Data ");
			if (strlen(argv[2]) > sizeof (st_data[no].formula))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_data[no].formula, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_data[no].formula);
			}
		}
		else if (strcmp(argv[2], "satuan") == 0)	{
			printf("  set satuan Data ");
			if (strlen(argv[2]) > sizeof (st_data[no].satuan))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_data[no].satuan, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_data[no].satuan);
			}
		}
		else if (strcmp(argv[2], "id") == 0)	{
			st_data[no].id = atoi( argv[3] );
			printf("  ID kanal-%d: %d\r\n", no+1, st_data[no].id);
		}
		else if (strcmp(argv[2], "rangeL") == 0)	{
			st_data[no].rangeL = atoi( argv[3] );
			printf("  RangeL: %d\r\n", st_data[no].rangeL);
		}
		else if (strcmp(argv[2], "batasLL") == 0)	{
			st_data[no].batasLL = atoi( argv[3] );
			printf("  Batas LL: %d\r\n", st_data[no].batasLL);
		}
		else if (strcmp(argv[2], "batasL") == 0)	{
			st_data[no].batasL = atoi( argv[3] );
			printf("  Batas L: %d\r\n", st_data[no].batasL);
		}
		else if (strcmp(argv[2], "batasH") == 0)	{
			st_data[no].batasH = atoi( argv[3] );
			printf("  Batas H: %d\r\n", st_data[no].batasH);
		}
		else if (strcmp(argv[2], "batasHH") == 0)	{
			st_data[no].batasHH = atoi( argv[3] );
			printf("  Batas HH: %d\r\n", st_data[no].batasHH);
		}
		else if (strcmp(argv[2], "rangeH") == 0)	{
			st_data[no].rangeH = atoi( argv[3] );
			printf("  Range H: %d\r\n", st_data[no].rangeH);
		} 
		else if (strcmp(argv[2], "status") == 0)	{
			st_data[no].status = atoi( argv[3] );
			printf("  Status: [%d] : %s\r\n", st_data[no].status, st_data[no].status?"Aktif":"Mati");
		} 
		else {
			printf("  __ Perintah SALAH __\r\n");
			data_kitab();
			return;
		}
	}
	//printf("\r\n");	
	vPortFree (st_data);
	return 0;
}

void set_data_default()		{
	int i;
	
	struct t_data st_data[PER_SUMBER*JML_SUMBER];
	
	for (i=0; i<PER_SUMBER; i++)	{
		sprintf(st_data[i].nama, "data %d", i+1);
		st_data[i].id      = 0;
		st_data[i].rangeL  = -50;
		st_data[i].batasLL = 0;
		st_data[i].batasL  = 1;
		st_data[i].batasH  = 500;
		st_data[i].batasHH = 800;
		st_data[i].rangeH  = 1000;
		st_data[i].status  = 0;
		strcpy(st_data[i].satuan, "-");
		strcpy(st_data[i].formula, "");
	}
	
	i = simpan_struct_block_rom(SEKTOR_DATA, -1, (char *) st_data);
	printf("hsl: %d\r\n", i);
}

