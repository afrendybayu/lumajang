
// Afrendy Bayu
// Depok, 29 Juli 2013
// 

#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"

#include "sh_env.h"
#include "sh_utils.h"
#include "manual.h"

extern struct t_env env;

void cek_env(int argc, char **argv)	{
	uprintf("\r\n    Cek modul %s %s   \r\n****************************************\r\n", BOARD_SANTER, BOARD_SANTER_v1_0);
	uprintf("  No Seri  : %s\r\n", env.SN);
	uprintf("  No Ajaib : %02X:%02X\r\n", env.magic1, env.magic2);
	#ifdef PAKAI_ETH
	uprintf("Konfigurasi Ethernet\r\n");
	uprintf("  Alamat IP: %d.%d.%d.%d\r\n", env.IP0, env.IP1, env.IP2, env.IP3);
	uprintf("  Gateway  : %d.%d.%d.%d\r\n", env.GW0, env.GW1, env.GW2, env.GW3);
	uprintf("  Server IP: %d.%d.%d.%d\r\n", env.wIP0, env.wIP1, env.wIP2, env.wIP3);
	uprintf("  File     : %s\r\n", env.berkas);
	uprintf("  Webclient: [%d] : %s\r\n", env.statusWebClient, (env.statusWebClient?"Aktif":"Mati"));
	#endif
	#ifdef MODBUS_RTU_SLAVE
	uprintf("Konfigurasi Modbus Slave:\r\n");
	uprintf("  Alamat Device: %d\r\n", env.almtSlave);
	#endif
	uprintf("Konfig Cron  : [%d] : %s\r\n", env.statusCron, (env.statusCron?"Aktif":"Mati"));
	uprintf("Konfig Debug1: %d\r\n", env.prioDebug);
	uprintf("Konfig Debug2: %d\r\n", env.prioDebug2);
}

char set_env(int argc, char **argv)	{
	if (argc>3 || argc==1)		env_kitab();
	if (argc==2)	{
		printf("\r\n");
		if (strcmp(argv[1], "help") == 0)	{
			env_kitab();
			return 0;
		}
		else if (strcmp(argv[1], "default") == 0)	{
			printf("set env dengan konfig default !\n");
			set_env_default();
			return 0;
		}
		return 1;
	}
	if (argc==3)	{
		printf("\r\n");
		if (strcmp(argv[1], "SN") == 0)	{
			printf(" set no seri \r\n");
			if (strlen(argv[2]) > sizeof (env.SN))		{
				printf("SN terlalu panjang !");
				return 1;	
			}
			else	{
				sprintf(env.SN, "%s", argv[2]);
				printf(" SN : %s\n", env.SN);
			}
		}
		else if (strcmp(argv[1], "ipaddr") == 0)  	{
			printf(" set Alamat IP ");  
			
			uint ret_ip = baca_ip( argv[2] );
			if (ret_ip > 0)	{
				env.IP0 = (unsigned char)(ret_ip >> 24);
				env.IP1 = (unsigned char)(ret_ip >> 16);
				env.IP2 = (unsigned char)(ret_ip >> 8);
				env.IP3 = (unsigned char)(ret_ip);
				printf("%d.%d.%d.%d\r\n", env.IP0, env.IP1, env.IP2, env.IP3);
			} 
			else		return 2;
		}
		else if (strcmp(argv[1], "gateway") == 0)  	{
			printf(" set GATEWAY address ");  

			uint ret_ip = baca_ip( argv[2] );
			if (ret_ip > 0)	{
				env.GW0 = (unsigned char)(ret_ip >> 24);
				env.GW1 = (unsigned char)(ret_ip >> 16);
				env.GW2 = (unsigned char)(ret_ip >> 8);
				env.GW3 = (unsigned char)(ret_ip);
				printf("%d.%d.%d.%d\r\n", env.GW0, env.GW1, env.GW2, env.GW3);
			} 
			else		return 2;
		}
		else if (strcmp(argv[1], "server") == 0)  	{
			printf(" set Server Tujuan address ");  

			uint ret_ip = baca_ip( argv[2] );
			if (ret_ip > 0)	{
				env.wIP0 = (unsigned char)(ret_ip >> 24);
				env.wIP1 = (unsigned char)(ret_ip >> 16);
				env.wIP2 = (unsigned char)(ret_ip >> 8);
				env.wIP3 = (unsigned char)(ret_ip);
				printf("%d.%d.%d.%d\r\n", env.wIP0, env.wIP1, env.wIP2, env.wIP3);
			} 
			else		return 2;
		}
		else if (strcmp(argv[1], "nama") == 0)	{
			printf(" set nama board\r\n");

			if (strlen(argv[2]) > sizeof (env.nama_board))		{
				printf("Nama board terlalu panjang !");
				return 1;	
			}
			else	{
				sprintf(env.nama_board, "%s", argv[2]);
				printf(" Nama board : %s\n", env.nama_board);
			}
		}
		else if (strcmp(argv[1], "prio") == 0)	{
			printf("  Debug prio serial0\r\n");
			env.prioDebug = atoi( argv[2] );
			printf("  Nomor batas prioritas debug : %d\r\n", env.prioDebug);
			
		}
		else if (strcmp(argv[1], "prio2") == 0)	{
			printf("  Debug prio serial2\r\n");
			env.prioDebug2 = atoi( argv[2] );
			printf("  Nomor batas prioritas debug2 : %d\r\n", env.prioDebug2);
			
		}	
	}
	printf("\r\n");
	return 0;
}

void set_env_default()		{
	int i;
	strcpy(env.nama_board, BOARD_SANTER);
	env.IP0 = 192;
	env.IP1 = 168;
	env.IP2 = 1;
	env.IP3 = 250;
	env.GW0 = 192;
	env.GW1 = 168;
	env.GW2 = 1;
	env.GW3 = 1;
	
	for (i=0; i<JML_KANAL; i++)	{
		env.kalib[i].m = 1;
		env.kalib[i].C = 0;
		//strcpy(env.kalib[i].ket, "----");
	}
	
	env.magic1 = 0x01;
	env.magic2 = 0x01;
	env.mmc_serial = 0;
	strcpy(env.SN, "STR.kalender");

	strcpy(env.berkas, "/monita3/ml.php");
	env.statusWebClient = 0;
	env.banyak_sumber = 5;
	env.burst = 0;
	env.wIP0 = 192;
	env.wIP1 = 168;
	env.wIP2 = 1;
	env.wIP3 = 99;
	env.k1 = 0;
	env.k2 = 0;
	env.uRTC = 0;
	env.statusSerClient = 0;
	env.intKirim = 10;
	env.intReset = 30;
	env.intTole = 10;
	env.netTot = 10;
	strcpy(env.passwd, "monita");
	strcpy(env.madein, "Afrendy Bayu");
	strcpy(env.nohp, "082114722505");
	env.statusCron = 0;
	env.almtSlave = 1;
	env.statusSlave = 0;
	env.prioDebug = 0;
	env.prioDebug2 = 0;
}

