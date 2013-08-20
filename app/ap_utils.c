
// Afrendy Bayu
// Depok, 29 Juli 2013
// 


#include "FreeRTOS.h"
#include "task.h"
#include "ap_utils.h"

#ifdef PAKAI_MODBUS



unsigned int CRC16(unsigned int crc, unsigned int data)		{
	const unsigned int Poly16=0xA001;
	unsigned int LSB, i;
	crc = ((crc^data) | 0xFF00) & (crc | 0x00FF);
	for (i=0; i<8; i++) {
		LSB = (crc & 0x0001);
		crc = crc/2;
		if (LSB)
			crc=crc^Poly16;
	}
	return(crc);
}


#endif
