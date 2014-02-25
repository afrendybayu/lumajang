
// Afrendy Bayu
// Depok, 25 Maret 2013
// 
// 25 Feb 2014
// ditambahkan timer untuk reset jika lebih dari waktu yang ditentukan
// tidak ada data yang masuk.
// 
// skenario :
// saat jadi modbus slave, request biasanya 8 byte (8 karakter)
// jika kecepatan 115200, maka 8 byte itu kira2 akan diterima dalam
// waktu sbb :
// 115200 ==> 14400 byte/s, maka 1 byte perlu : 0.069 ms
// maka jika 8 byte, mestinya akan diterima dalam : 0.55 ms
// 
// jika 9600 bps, ==> 9600/8 = 1200 byte/s, maka 1 byte perlu : 0.83 ms
// jika 8 byte, maka : 6.6 ms
// 
// jadi jika dalam 10 ms, data tidak komplit 8, dengan aman mestinya 
// bisa kita reset

/* Scheduler include files. */


#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial/serial.h"
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

static xComPortHandle xPort2;
extern volatile struct t_st_hw st_hw;

//void sedot_mod(int ch);
//int proses_mod_cmd();

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

char strmb[MAX_RX_MB]		__attribute__ ((section (".usbram1")));
char outmb[MAX_RX_MB]		__attribute__ ((section (".usbram1")));

static portTASK_FUNCTION( vComTask2, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char s[30];
	//char strmb[MAX_RX_MB];
	int  nmb = 0, balas = 0;
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
	
	do {
		vTaskDelay(100);
	} while(st_hw.init != uxTaskGetNumberOfTasks());
		
	for( ;; )	{
		//printd2(10, "serial 2: %d\r\n", loop2++);
		//vSerialPutString2(xPort2, "tes2\r\n", 6);
		//xGotChar = xSerialGetChar2( xPort2, &ch, 10 );
		xGotChar = xSerialGetChar2( xPort2, &ch, 1 );
		if( xGotChar == pdTRUE )		{
			//printf("%02x ", (char) ch);
			//printf("%c ", (char) ch);
			strmb[nmb] = (char) ch;
			nmb++;
			//strSer2[nmb+1] = '\0';
			//sedot_mod(ch);
			
			//printf("n%d:%X,", nmb, (unsigned char) ch);
			flag_ms=1;
			
			if (nmb >= 8)
			{
				balas = proses_mod(8, &strmb[nmb - 8]);
				//nmb = 0;
				loop2 = 0;
				//printf("<bal %d>", balas);
			}
		}
		else {
			
			/* setiap lepas dari loop diatas direset saja */
			nmb = 0;
			
			#if 0
			// sedot data respon (sendiri), clear buffer
			if ( (balas==nmb) && (balas>0) )	{			
				printf("Reset MB2 !!!\r\n");
				nmb = 0;
				flag_ms = 0;
				balas = 0;
			}
			
			if (flag_ms==1 && nmb>4)	{
				//balas = proses_mod(nmb, strmb);
				//printf("--==> BALAS MB: %d\r\n", balas);
				nmb = 0;
				//loop2 = 0;
			}
			if (balas==0)	{
				nmb = 0;
			}
			//
			#if 0
			flag_ms = 0;
			nmb = 0;
			#endif
			#endif
			
		}
	}
}

int proses_mod(int mbn, char *mbstr)	{
	int hsl=0, cmd=0, jml=0, reg=0;
	
	//printf(">PP %d<", mbn);
	#if 0
	printf("\r\nJml CMD: %d -->", mbn);
	int i,mm;
	
	mm = (mbn>40)?40:mbn;
	//for (i=0; i<mbn; i++)		{
	for (i=0; i<mm; i++)		{
		//printf(" %02x", mbstr[i]);
		printf(" %02x", strmb[i]);
	}
	printf("\r\n");
	#endif
	
	struct t_env *p_env3;
	p_env3 = (char *) ALMT_ENV;
		
	//if (p_env3->almtSlave != mbstr[0])	{
	if (p_env3->almtSlave != strmb[0])	{
		return 1;
	}
	
	//cmd = mbstr[1];
	cmd = strmb[1];
	if (cmd==READ_FILE_CONTENT && mbn>8)	return 3;		// kiriman sendiri
	
	if (cmd>=READ_FILE_CONTENT)
		//hsl = cek_crc_ccitt_0xffff(mbn, mbstr);		// modbus-serial dari skywave data file
		hsl = cek_crc_ccitt_0xffff(mbn, strmb);			//
	else
		//hsl = cek_crc_mod(mbn, mbstr);				// modbus murni
		hsl = cek_crc_mod(mbn, strmb);
	
	
	
	if (hsl==1 && mbn>=8)	{				// 8: min panjang modbus REQUEST
		//printf(" > LULUS < !!!\r\n");
		
		#if 0
		cmd = mbstr[1];
		reg = (int) (mbstr[2]<<8 | mbstr[3]);
		jml = (int) (mbstr[4]<<8 | mbstr[5]);
		#endif
		
		cmd = strmb[1];
		reg = (int) (strmb[2]<<8 | strmb[3]);
		jml = (int) (strmb[4]<<8 | strmb[5]);
		
		//printf("++++ cmd: 0x%02x, reg: 0x%02x, jml: %d\r\n", cmd, reg, jml);
		//cmd = parsing_mod(strSer2);
		if (cmd>0)	{
			//printf("__PROSES DATA KITA !!\r\n");
			//return respon_modbus(cmd, reg, jml, mbstr, mbn);
			return respon_modbus(cmd, reg, jml, strmb, mbn);
		}
	}
	return 2;
}


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
