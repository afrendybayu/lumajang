
#include "FreeRTOS.h"
#include "task.h"

#include "sh_kanal.h"
#include "monita.h"
#include "sh_utils.h"
#include "manual.h"


void cek_kanal()	{
	struct t_env *st_env;
	st_env = ALMT_ENV;
	int i;
	
	uprintf("\r\n  Info setting kanal Konter\r\n");
	for (i=0; i<JML_KANAL; i++)		{
		uprintf("    Kanal %2d. m: %8.3f, C: %8.3f, status: %d\r\n", \
			i+1, st_env->kalib[i].m, st_env->kalib[i].C, st_env->kalib[i].status);
	}
	
	uprintf("  Info setting kanal analog 4-20mA\r\n");
	for (i=0; i<JML_KANAL; i++)		{
		uprintf("    Kanal %2d. m: %8.3f, C: %8.3f, status: %d\r\n", \
			(i+JML_KANAL+1), st_env->kalib[i+JML_KANAL].m, st_env->kalib[i+JML_KANAL].C, st_env->kalib[i+JML_KANAL].status);
	}
	info_kanal();
}


char set_kanal(int argc, char **argv)		{
	unsigned char str_kanal[24];
	unsigned int kanal;
	float m;
	float c;
	int ret;
	
	printf("\r\n");
	if (argc>4 || argc==1)		kanal_kitab();
	
	//cek_kanal();
	sprintf(str_kanal, "%s", argv[1]);
	ret = sscanf(str_kanal, "%d", &kanal);
	
	int no = cek_nomor_valid(argv[1], JML_KANAL*2);
		
	if (no == TIDAK_VALID || no == NULL)	{
		printf("  no sumber TIDAK VALID\r\n");
		return 2;
	}
	
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	
	if (st_env==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_env);
		return;
	}
	
	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	if (argc==4 || argc==2)	{
		//int no = cek_nomor_valid(argv[1], JML_KANAL);
		if (strcmp(argv[1], "default") == 0)	{
			printf("  set_kanal dengan konfig default !\r\n");
			set_kanal_default();
			return 0;
		}
		else if (strcmp(argv[2], "status") == 0)	{
			int stx;
			uprintf("no: %d, [0]: %s, [1]: %s, [2]: %s, [3]: %s\r\n", no, argv[0], argv[1], argv[2], argv[3]);
			stx = atoi( argv[3] );
			st_env->kalib[no-1].status = stx;
			if (stx == sRPM_RH)	{
				st_env->kalib[no].status = sRUNNING_HOURS;
			}
			uprintf("  status[%d] : %d\r\n", no, st_env->kalib[no-1].status);
		} else	{
			sprintf(str_kanal, "%s", argv[2]);
			ret = sscanf(str_kanal, "%f", &m);
		
			if (ret == NULL) {
				printf(" Err m !\r\n"); 
				vPortFree( st_env );
				return ;
			}
		
			sprintf(str_kanal, "%s", argv[3]);
			ret = sscanf(str_kanal, "%f", &c);
		
			if (ret == NULL) {
				printf(" Err C !\r\n"); 
				vPortFree( st_env );
				return ;
			}
		
			printf(" Seting kanal %d, m = %f, dan C = %f\r\n", kanal, m, c);
			st_env->kalib[kanal - 1].m = m;
			st_env->kalib[kanal - 1].C = c;
			
		}
	}

	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	vPortFree (st_env);
	return 0;
}

void set_kanal_default()		{
	int i, j;
	
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	
	if (st_env==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_env);
		return;
	}
	
	for (i=0; i<(JML_KANAL*2); i++)	{
		st_env->kalib[i].m = 1;
		st_env->kalib[i].C = 0;
		st_env->kalib[i].status = sRPM;
		//strcpy(env.kalib[i].ket, "----");
	}
	
	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	//simpan_struct_block_rom(SEKTOR_ENV, ENV, 1, (char *) &st_env);
	//simpan_struct_block_rom(SEKTOR_ENV, ENV, 1, &st_env);
	vPortFree (st_env);
}
