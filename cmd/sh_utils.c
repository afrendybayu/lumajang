
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
