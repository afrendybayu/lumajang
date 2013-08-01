
// Afrendy Bayu
// Depok, 29 Juli 2013
// 



#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"

#include "sh_utils.h"
#include "sh_sumber.h"
#include "manual.h"


void cek_sumber(int argc, char **argv)	{
	int i=0;
	
	uprintf("\r\n  Cek Sumber\r\n  ******************************\r\n");
	uprintf (" no |        Nama       |        IP       | Almt | Stack |      Keterangan     |\r\n");
	for (i=0; i<JML_SUMBER; i++)	{
		printf(" %2d | %-17s | %3d.%3d.%3d.%3d | %4d |  %3d  | %-19s |\r\n", i+1, st_sumber[i].nama, \
			st_sumber[i].IP0, st_sumber[i].IP1, st_sumber[i].IP2, st_sumber[i].IP3,	\
			st_sumber[i].alamat, st_sumber[i].stack, st_sumber[i].ket);
	}
}



char set_sumber(int argc, char **argv)		{
	if (argc>4 || argc==1)		sumber_kitab();
	if (argc==2 || argc==3)	{
		printf("\r\n");
		if (strcmp(argv[1], "default") == 0)	{
			printf("set_sumber dengan konfig default !\r\n");
			set_sumber_default();
			return 0;
		}
		sumber_kitab();
		return 1;
	}
	
	int no = cek_nomor_valid(argv[1], JML_SUMBER);
	no--;
	if (no == TIDAK_VALID)	{
		printf("  no sumber TIDAK VALID\r\n");
		return 2;
	}
	
	if (argc==4)	{
		printf("\r\n");
		if (strcmp(argv[2], "nama") == 0)	{
			printf("  set Nama sumber ");
			if (strlen(argv[2]) > sizeof (st_sumber[no].nama))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_sumber[no].nama, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_sumber[no].nama);
			}
		}
		#ifdef PAKAI_ETH
		else if (strcmp(argv[2], "ipaddr") == 0)  	{
			printf("  set IP ");  
			
			uint ret_ip = baca_ip( argv[3] );
			if (ret_ip > 0)	{
				st_sumber[no].IP0 = (unsigned char)(ret_ip >> 24);
				st_sumber[no].IP1 = (unsigned char)(ret_ip >> 16);
				st_sumber[no].IP2 = (unsigned char)(ret_ip >> 8);
				st_sumber[no].IP3 = (unsigned char)(ret_ip);
				printf(":%d.%d.%d.%d\r\n", \
					st_sumber[no].IP0, st_sumber[no].IP1, st_sumber[no].IP2, st_sumber[no].IP3);
			} 
			else		{
				printf("ERROR !!\r\n");
				return 2;
			}
		}
		#endif
		else if (strcmp(argv[2], "ket") == 0)	{
			printf("  Set nama ");

			if (strlen(argv[3]) > sizeof (st_sumber[no].ket))		{
				printf("terlalu panjang !\r\n");
				return 1;	
			}
			else	{
				sprintf(st_env.nama_board, "%s", argv[3]);
				printf(": %s\r\n", st_sumber[no].ket);
			}
		}
		else if (strcmp(argv[2], "tipe") == 0)	{
			st_sumber[no].tipe = atoi( argv[3] );
			printf("  Tipe:%d\r\n", st_sumber[no].tipe);
			
		}
		else if (strcmp(argv[2], "alamat") == 0)	{
			st_sumber[no].alamat = atoi( argv[3] );
			printf("  Tipe:%d\r\n", st_sumber[no].alamat);
			
		}	
	}
	printf("\r\n");
	
	int i;
	char *pch;
	pch = &st_env;
	printf("  data: ");
	for (i=0; i<10; i++)	{
		printf("%02X ", pch[i]);
	}
	printf("\r\n");
	
	
	return 0;
}

void set_sumber_default()		{
	int i;
	
	for (i=0; i<JML_SUMBER; i++)	{
		sprintf(st_sumber[i].nama, "Sumber %d", i+1);
		st_sumber[i].IP0 = 192;
		st_sumber[i].IP1 = 168;
		st_sumber[i].IP2 = 1;
		st_sumber[i].IP3 = 250;
		st_sumber[i].stack  = 0;
		st_sumber[i].alamat = 0;
		st_sumber[i].tipe = 0;
		strcpy(st_sumber[i].ket, "---");
		
	}
}

