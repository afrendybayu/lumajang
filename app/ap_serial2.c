
// Afrendy Bayu
// Depok, 25 Maret 2013
// 

/* Scheduler include files. */


#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial.h"
#include "ap_serial2.h"
//#include "sh_serial.h"
//#include <stdarg.h>
#include "queue.h"
#include "hardware.h"
#include "monita.h"
//#include "sh_hardware.h"



extern struct t_st_hw st_hw;

static portTASK_FUNCTION( vComTask2, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char s[30];

	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(1);
	
	
	vTaskDelay(500);
	st_hw.init++;
	//int itung=0;
	
	
	for( ;; )	{
		vTaskDelay(100);
		vSerialPutString2(xPort2, "tes2\r\n", 6);
		//qsprintf("serial 2\r\n");

		#if 0
		xGotChar = xSerialGetChar2( xPort2, &ch, 100 );
		if( xGotChar == pdTRUE )		{

			
		}
		
		#endif
		//qrprintf(0);
	}
}



void vAltStartCom2( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate )		{
const unsigned portBASE_TYPE uxQueueSize = 10;
const unsigned portBASE_TYPE uxQueueLength = 128;
	//xPrintQueue = xQueueCreate( uxQueueSize, uxQueueLength );
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInit2( ulBaudRate, configMINIMAL_STACK_SIZE );

	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate( vComTask2, ( signed char * ) "Serial2", comSTACK_SIZE * ST_SER2, NULL, uxPriority, ( xTaskHandle * ) hdl_serial2 );
}
