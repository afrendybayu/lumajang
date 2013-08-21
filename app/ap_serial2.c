
// Afrendy Bayu
// Depok, 25 Maret 2013
// 

/* Scheduler include files. */


#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial.h"
#include "modbus/mb.h"
#include "ap_serial2.h"
//#include "sh_serial.h"
//#include <stdarg.h>
#include "queue.h"
#include "hardware.h"
#include "monita.h"
#include <stdarg.h>
//#include "sh_hardware.h"

#ifdef PAKAI_SERIAL_2

//char strSer2[256];
//int nSer2;
static xComPortHandle xPort2;
extern volatile struct t_st_hw st_hw;

//void sedot_mod(int ch);
//int proses_mod_cmd();
int proses_mod(int mbn, char *mbstr);
//int parsing_mod(unsigned char *x);


#if 0
static xQueueHandle xPrintQueue2;

// qsprintf : custom printf yg dapat menyimpan data ke queue
void qsprintf2(char *fmt, ...) {
	//uprintf("---> KIRIM : ");
	char str_buffer[256];
	int lg=0;
	va_list args;
	va_start(args, fmt);
	lg = vsprintf(str_buffer, fmt, args);
	lg = xQueueSend( xPrintQueue2, ( void * ) str_buffer, ( portTickType ) 0 );
	va_end(args);

}
#endif

void printd2(int prio, const char *format, ...)	{
	va_list arg;
	int lg=0;
	char str_buffer[256];
	if (prio>0)	{
	//if (prio>env.prioDebug2)	{
		va_start (arg, format);
		lg = vsprintf (str_buffer, format, arg);	
		va_end (arg);
		//printf("%s", format);
		enaTX2_485();
		vSerialPutString2(xPort2, format, lg);
		vTaskDelay(10);
		disTX2_485();
	}
}

static portTASK_FUNCTION( vComTask2, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char s[30];
	char strmb[256];
	int  nmb = 0, balas = 1;
	char flag_ms = 0;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(100);
	init_banner2();
	
	vTaskDelay(500);
	st_hw.init++;
	//nSer2 = 0;
	int loop2 = 0;
	disTX2_485();
		
	for( ;; )	{
		//printd2(10, "serial 2: %d\r\n", loop2++);
		//vSerialPutString2(xPort2, "tes2\r\n", 6);
		xGotChar = xSerialGetChar2( xPort2, &ch, 10 );
		if( xGotChar == pdTRUE )		{
			printf("%02x ", (char) ch);
			//printf("%c ", (char) ch);
			strmb[nmb] = (char) ch;
			nmb++;
			//strSer2[nmb+1] = '\0';
			//sedot_mod(ch);
			flag_ms=1;
		}
		else {
			//printf("&");
			//if (flag_ms==1 && nSer2>0)	{
			//if (balas == 0)		flag_ms = 0;
			if (flag_ms==1 && nmb>4)	{
				balas = proses_mod(nmb, strmb);
			}
			nmb = 0;
			flag_ms = 0;
			//nSer2=0;
		}
	}
}

#if 0
int parsing_mod(unsigned char *x)	{
	struct t_env *p_env3;
	p_env3 = (char *) ALMT_ENV;
	
	if (p_env3->almtSlave != x[0])	{
		return 0;
	}
	return  x[1];
	
#if 0
	
	
	p_mod_sl.almt = x[0];
	p_mod_sl.cmd  = x[1];
	p_mod_sl.reg  = (x[2] << 8) + x[3]; 
	p_mod_sl.jml  = ((x[4]<<8)+x[5])/4;
	//printf("al: %d, cmd: %d, reg: %d, jml: %d\r\n", p_mod_sl.almt, p_mod_sl.cmd, p_mod_sl.reg, p_mod_sl.jml);
	
	if (p_mod_sl.almt==p_env3->almtSlave)	{		// untuk sendiri
		return 1;
	}
	return 0;
#endif
}
#endif

int proses_mod(int mbn, char *mbstr)	{
	int hsl=0, cmd=0, jml=0, reg=0;

	printf("\r\nJml CMD: %d -->", mbn);
	int i;
	for (i=0; i<mbn; i++)		{
		printf(" %02x", mbstr[i]);
	}
	printf("\r\n");
	hsl = cek_crc_mod(mbn, mbstr);
	
	if (hsl==1 && mbn>=8)	{				// 8: min panjang modbus REQUEST
		//printf(" > LULUS < !!!\r\n");
		struct t_env *p_env3;
		p_env3 = (char *) ALMT_ENV;
		
		if (p_env3->almtSlave != mbstr[0])	{
			return 1;
		}
		cmd = mbstr[1];
		reg = (int) (mbstr[2]<<8 | mbstr[3]);
		jml = (int) (mbstr[4]<<8 | mbstr[5]);
		
		printf("++++ cmd: %02x, reg: %02x, jml: %d\r\n", cmd, reg, jml);
		//cmd = parsing_mod(strSer2);
		if (cmd>0)	{
			//printf("__PROSES DATA KITA !!\r\n");
			return respon_modbus(cmd, reg, jml, mbstr);
		}
	}
	return 2;
}

#if 0
int proses_mod_cmd()	{
	int hsl=0, cmd=0, jml=0, reg=0;
	//pr_mod("\r\n>> BACA: ", sping, np);
	printf("Jml CMD: %d -->", nSer2);
	int i;
	for (i=0; i<nSer2; i++)		{
		printf(" %02x", strSer2[i]);
	}
	printf("\r\n");
	hsl = cek_crc_mod(strSer2);
	
	if (hsl==1 && nSer2>=8)	{				// 8: min panjang modbus REQUEST
		//printf(" > LULUS < !!!\r\n");
		struct t_env *p_env3;
		p_env3 = (char *) ALMT_ENV;
		
		if (p_env3->almtSlave != strSer2[0])	{
			return 0;
		}
		cmd = strSer2[1];
		reg = (int) (strSer2[2]<<8 | strSer2[3]);
		jml = (int) (strSer2[4]<<8 | strSer2[5]);
		
		//cmd = parsing_mod(strSer2);
		if (cmd>0)	{
			//printf("__PROSES DATA KITA !!\r\n");
			//respon_modbus();
		}
	}
}

void sedot_mod(int ch)	{
	char x = (char) ch;
	int hsl=0, r;
	//char stmp[50];
	
	strSer2[nSer2] = x;
	strSer2[nSer2+1] = '\0';
	
	nSer2++;
	#if 0
	if (nSer2>7)	{
		#if 1
		printf("#### : ");
		for (r=0; r<nSer2; r++)	{
			printf("%02x ", strSer2[r]);
		}
		printf("\r\n");
		#endif

		//hsl = cek_crc_mod(strSer2);
		if (hsl==1)	{
			//printf(" > LULUS < !!!\r\n");
			if (parsing_mod(sping)==1)	{
				//printf("__PROSES DATA KITA !!\r\n");
				respon_modbus();
			}
			
		}
		
		//printf("######## : %s--\r\n", sping);
		np=0;
	}
	#endif
}
#endif

void vAltStartCom2( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate )		{
//const unsigned portBASE_TYPE uxQueueSize = 10;
//const unsigned portBASE_TYPE uxQueueLength = 256;
	//xPrintQueue2 = xQueueCreate( uxQueueSize, uxQueueLength );
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInit2( ulBaudRate, configMINIMAL_STACK_SIZE );

	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate( vComTask2, ( signed char * ) "Serial2", comSTACK_SIZE * ST_SER2, NULL, uxPriority, ( xTaskHandle * ) hdl_serial2 );
}

void init_banner2()		{
	printf("Task vComTask2 Init\r\n");
	//printd2(10, "masuk task RS485_2\r\n");
	//vSerialPutString2(xPort2, "vSerialPutString2\r\n", 6);
}

#endif
