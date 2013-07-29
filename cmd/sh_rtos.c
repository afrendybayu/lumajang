
#include "FreeRTOS.h"
#include "task.h"

#ifdef PAKAI_TINYSH
#include "tinysh/tinysh.h"
#endif

#include "sh_serial.h"
#include "sh_rtos.h"

void task_list()	{
static signed char cListBuffer[ mainLIST_BUFFER_SIZE ];
const signed char *pcList = &( cListBuffer[ 0 ] );
const char * const pcHeader = "Task          State  Priority  Stack	#\r\n************************************************\r\n";
	sprintf(cListBuffer, "\r\nJml Task: %d\r\n", uxTaskGetNumberOfTasks());
	qsprintf(cListBuffer);
	qsprintf(pcHeader);
	vTaskList( cListBuffer );
	qsprintf(cListBuffer);
}

#if 0
void task_run_time()	{
static signed char cListBuffer[ mainLIST_BUFFER_SIZE ];
const signed char *pcList = &( cListBuffer[ 0 ] );
const char * const pcHeader = "\r\nTask          Time  %	#\r\n************************************************\r\n";
	qsprintf(pcHeader);
	vTaskGetRunTimeStats( cListBuffer );
	qsprintf(cListBuffer);
}
#endif
