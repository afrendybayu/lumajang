
#include "FreeRTOS.h"
#include "monita.h"
#include "ap_adc.h"
#include "adc/ad7708.h"

#ifdef PAKAI_ADC_7708

extern struct t_adc adc;
extern volatile float data_f[];
//extern float data_f[];
//extern struct t_env env;

void data_adc()	{
	char i;
	float tf;
	
	for (i=0; i<JML_KANAL_ADC; i++)		{
		tf = (float) (adc.data[i] * faktor_pengali_420 / 0xffff);
		//data_f[20+i] = env.kalib[i].m * tf + env.kalib[i].C;
		data_f[JML_KANAL+i] = tf;
	}
}
#endif
