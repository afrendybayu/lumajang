
// Afrendy Bayu
// Depok, 13 Agustus 2013
// 
#include "FreeRTOS.h"

#ifdef PAKAI_RELAY

#include "task.h"
#include "monita.h"

#include "sh_relay.h"
#include "sh_utils.h"
#include "manual.h"
#include "hardware.h"


char cek_relay(int argc, char **argv)	{
	if (argc>2)		{
		relay_kitab();
	}
	
	int i;
	printf("\r\nCek Relay\r\n*********************************\r\n");
	int tRelay = JML_SUMBER * PER_SUMBER;
	if (argc==1)	{
		for(i=0; i<JML_RELAY; i++)		{
			//qsprintf("  Relay-%d : %d - %s\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]) ?"Aktif","Mati");
			uprintf("  Relay-%d : %d - %d\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]));		
		}
	}
	
	if (argc==2)	{
		i = cek_nomor_valid(argv[1], JML_RELAY);		
		if ( (i==TIDAK_VALID) || (i==0) )	{
			printf("  no sumber TIDAK VALID\r\n");
			return 1;
		}
		i--;
		//qsprintf("  Relay-%d : %d - %s\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]) ?"Aktif","Mati");
		qsprintf("  Relay-%d : %d - %d\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]));
	}
	return 0;
}

char set_relay(int argc, char **argv)	{
	if (argc!=3)	{
		printf("\r\n  --> Perintah Salah.\r\n");
		relay_kitab();
		return 1;
	}
	
	int i;

	int no = cek_nomor_valid(argv[1], JML_RELAY);		
	if ( (no==TIDAK_VALID) || (no==0) )	{
		printf("  no sumber TIDAK VALID\r\n");
		return 1;
	}
	//no--;
	int tRelay = JML_SUMBER * PER_SUMBER;
	
	uprintf("\r\n");
	if ( (strcmp(argv[2], "aktif")==0) || (argv[2][0]=='1') )	{
		sRelay(no);
		uprintf("relay %d AKTIF\r\n", no);
	}
	else if ( (strcmp(argv[2], "mati")==0) || (argv[2][0]=='0') )	{
		unsRelay(no);
		uprintf("relay %d MATI\r\n", no);
	}
	int r = (int) data_f[tRelay+no-1];
	qsprintf("  Relay-%d : %d - %s\r\n", no, r, (r==1)?"Aktif":"Mati");
	//qsprintf("  Relay-%d : %d - %d\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]));
}



#endif
