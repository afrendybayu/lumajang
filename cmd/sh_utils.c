
#include "FreeRTOS.h"
#include "monita.h"
#include "sh_utils.h"
#include <stdio.h>

unsigned int cek_nomor_valid(char *str, int max)	{
	int nx = atoi(str);
	if (nx>=0 && nx<=max)	return nx;
	else return TIDAK_VALID;
}

unsigned int baca_ip(char *sIP)	{
	unsigned char byte = atoi(sIP);
	unsigned ip, i=2;
	char *pch;
	
	if (byte==0)	{
		printf("ERROR parsing IP !!\r\n");
		return 0;
	}
	ip = byte<<24;	
	pch = strchr(sIP,'.');
	while (pch!=NULL)	{
		byte = atoi(pch+1);
		//printf("pch: %s, %d, %d\r\n", pch+1, atoi(pch+1), byte);
		ip |= byte<<(i--*8);
		pch = strchr(pch+1,'.');
	}
	return ip;
}

int cek_jml_struct(char *str)	{
	if (strcmp(str, "env")==0)
		return (sizeof(struct t_env));
	else if (strcmp(str, "data")==0)
		return (sizeof(struct t_data));
	else 
		return 0;
}

void cek_struct(int argc, char **argv)		{
	if (argc>2)	{
		return;
	}
	if (argc==1)	{
		printf("\r\n");
		printf("  Jml struct env : %d\r\n", cek_jml_struct("env"));
		printf("  Jml struct data: %d\r\n", (cek_jml_struct("data")*JML_SUMBER*PER_SUMBER));
		return;
	}
	printf("  Jml struct env : %d\r\n", cek_jml_struct(argv[1]));
}
