

#include "FreeRTOS.h"
#include "hardware.h"


void setup_hardware()	{
	
	sysInit();
	gpio_init();
	
	setup_watchdog();

	PCLKSEL0 = 0x55555555;		// PCLK is the same as CCLK
	PCLKSEL1 = 0x55555555;
	
	PCONP   |= BIT(31);			// Power USB aktif
	PCONP   |= BIT(30);			// Power Ethernet Aktif
	FIO1DIR  = 0xFFFFFFFF;

	/* USB Power dinyalakan supaya memory USB bisa dipakai */
	//PCONP   |= 0x80000000;
	//FIO1DIR  = 0xFFFFFFFF;
	SCS 	|= (1<<0); 				// fast mode for port 0 and 1
	MEMMAP = 0x01;					// SCB
	
	//#ifdef PAKAI_RELAY
	//	setup_relay();
	//#endif
	
	#ifdef PAKAI_RTC
		setup_rtc();
	#endif
	
	#ifdef PAKAI_SPI_SSP0

	#endif

	#ifdef PAKAI_SPI_SSP1
		setup_spi_ssp1();
	#endif
	
	#ifdef PAKAI_LED_UTAMA
		setup_led_utama();
	#endif

	
	
	#ifdef CEK_BLINK
		blink_led();
	#endif

	#ifdef PAKAI_SDCARD
		setup_sdc();
	#endif 

	#ifdef PAKAI_SHELL
		setup_serial0_P0();
	#endif
	
	#ifdef PAKAI_SERIAL_3
		#ifdef PAKAI_SERIAL_3_P0
			setup_serial3_P0();
		#endif
	#endif

	#ifdef PAKAI_SERIAL_2			
		#ifdef SPEED_SERIAL_2_P0
			setup_serial2_P0();
		#endif
	#endif

	#ifdef PAKAI_ADC_7708
		//setup_eint1();
		//setup_adc();
	#endif

	#ifdef PAKAI_RELAY
		setup_relay();
	#endif
}

void init_hardware()	{
	#ifdef PAKAI_RTC
		start_rtc();
	#endif
	
	#ifdef PAKAI_LED_UTAMA
		FIO1SET = LED_UTAMA;		// mati dulu;
	#endif

	#ifdef PAKAI_SHELL
		vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE );
	#endif


	
	#if 1
	gpio_int_init();
	#endif
	
	#ifdef PAKAI_SPI_SSP0
		//init_ssp0();
	#endif
	
	#ifdef PAKAI_SPI_SSP1
		init_ssp1();
	
		#ifdef PAKAI_ADC_7708
		adc_int_init();
		#endif
	
	#endif
	
	#ifdef PAKAI_SERIAL_2	
		//qsprintf("init serial 2\r\n");
		vAltStartCom2( mainCOM_TEST_PRIORITY, SPEED_SERIAL_2_P0 );		
	#endif
	
	#ifdef PAKAI_SDCARD
		
	#endif 
}

int setup_konter_onoff22(unsigned int aaa, unsigned char statk) {
	//printf("\r\n aaa: %d, statk: %d\r\n", aaa, statk);
	int bbb = 0;
	if (statk==1) {
		/*
		if (aaa==0) {	IO2_INT_EN_R |= kont_1;		bbb = 101;	}
		if (aaa==1) {	IO2_INT_EN_R |= kont_2;		bbb = 102;	}
		if (aaa==2) {	IO2_INT_EN_R |= kont_3;		bbb = 103;	}
		if (aaa==3) {	IO2_INT_EN_R |= kont_4;		bbb = 104;	}
		if (aaa==4) {	IO2_INT_EN_R |= kont_5;		bbb = 105;	}
		if (aaa==5) {	IO2_INT_EN_R |= kont_6;		bbb = 106;	}
		if (aaa==6) {	IO2_INT_EN_R |= kont_7;		bbb = 107;	}
		if (aaa==7) {	IO2_INT_EN_R |= kont_8;		bbb = 108;	}
		if (aaa==8) {	IO2_INT_EN_R |= kont_9;		bbb = 109;	}
		if (aaa==9) {	IO2_INT_EN_R |= kont_10;	bbb = 110;	}
		//*/
	} else if (statk==3)	{
		if (aaa==0) {	;	bbb = 201;	}
		if (aaa==1) {	;	bbb = 202;	}
		if (aaa==2) {	;	bbb = 203;	}
		if (aaa==3) {	;	bbb = 204;	}
		if (aaa==4) {	;	bbb = 205;	}
		if (aaa==5) {	;	bbb = 206;	}
		if (aaa==6) {	;	bbb = 207;	}
		if (aaa==7) {	;	bbb = 208;	}
		if (aaa==8) {	;	bbb = 209;	}
		if (aaa==9) {	;	bbb = 210;	}
	} else {
		if (aaa==0) {	IO2_INT_EN_F &= ~iKonter_1;		bbb = 1;	}
		if (aaa==1) {	IO2_INT_EN_F &= ~iKonter_2;		bbb = 2;	}
		if (aaa==2) {	IO2_INT_EN_F &= ~iKonter_3;		bbb = 3;	}
		if (aaa==3) {	IO2_INT_EN_F &= ~iKonter_4;		bbb = 4;	}
		if (aaa==4) {	IO2_INT_EN_F &= ~iKonter_5;		bbb = 5;	}
		if (aaa==5) {	IO2_INT_EN_F &= ~iKonter_6;		bbb = 6;	}
		if (aaa==6) {	IO2_INT_EN_F &= ~iKonter_7;		bbb = 7;	}
		if (aaa==7) {	IO2_INT_EN_F &= ~iKonter_8;		bbb = 8;	}
		if (aaa==8) {	IO2_INT_EN_F &= ~iKonter_9;		bbb = 9;	}
		if (aaa==9) {	IO2_INT_EN_F &= ~iKonter_10;	bbb = 10;	}
		
	}
	return bbb;
}

void gpio_init()	{
	// paksa PINSEL untuk GPIO //
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2 = 0x00000000;
	PINSEL3 = 0x00000000;
	PINSEL4 = 0x00000000;
	PINSEL5 = 0x00000000;
	PINSEL6 = 0x00000000;
	PINSEL7 = 0x00000000;
	PINSEL8 = 0x00000000;
	PINSEL9 = 0x00000000;
	PINSEL10 = 0x00000000;
	
	
	// paksa untuk enable pull up //
	PINMODE0 = 0x00000000;
	PINMODE1 = 0x00000000;
	PINMODE2 = 0x00000000;
	PINMODE3 = 0x00000000;
	PINMODE4 = 0x00000000;
	PINMODE5 = 0x00000000;
	PINMODE6 = 0x00000000;
	PINMODE7 = 0x00000000;
	PINMODE8 = 0x00000000;
}

// fungsi gpio_int_init untuk inisialisasi input interrupt konter
#if 1
void gpio_int_init()	{
	extern void ( gpio_ISR_Wrapper )( void );
	extern void ( timer1_ISR_Wrapper )( void );
	
	//reset_konter();
	portENTER_CRITICAL();

	// Timer1 untuk free running clock
	T1TCR = TxTCR_Counter_Reset;           // reset & disable timer 0

	// setup Timer 1 to count forever
	// Periode Timer 1 adalah 50 ns
	// akan overflow setiap 3.57 menit, interrupt dan
	// lakukan sesuatu pada variabel2.
	T1PR = 3 - 1;               	// set the prescale divider (60 / 3, 20 Mhz (50 ns))
	T1CCR = 0;                      // disable dulu compare registers
	T1EMR = 0;                      // disable external match register
	T1TCR = TxTCR_Counter_Enable;
	
	#if 1
	//siapkan interrupt handler untuk Timer 1
	VICIntSelect &= ~(VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1));
	VICIntEnClr  = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1);
	VICVectAddr5 = ( portLONG )timer1_ISR_Wrapper;
	VICVectPriority5 = 0x08;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_Timer1);
	T1MR0 = 0xFFFFFFFF;		// maksimum
	T1MCR = TxMCR_MR0I;     // interrupt on cmp-match0
	T1IR  = TxIR_MR0_Interrupt; // clear match0 interrupt

	// karena masih 3 menit yang akan datang, harusnya dienable dari sini gak masalah
	T1TCR = TxTCR_Counter_Enable;         // enable timer 0
	#endif
	
	#if 1
	//siapkan interrupt handler untuk GPIO
	VICIntSelect     &= ~VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICIntEnClr       = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	VICVectAddr17	  = ( portLONG )gpio_ISR_Wrapper;
	VICVectPriority17 = 0x05;
	VICIntEnable	  = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT3);
	#endif
	
	// 25 Feb 2014
	PINSEL4  = 0x00000000;	// sebagai GPIO
	//PINMODE4 |= BIT(14) | BIT(15);	//P2.7 iKonter_3
	// pull down enable, karena kita tertarik risiing edge	
	PINMODE4 |= pmode_konter_1 | pmode_konter_2 | pmode_konter_3 | pmode_konter_4 | pmode_konter_5; 	
	PINMODE1 |= pmode_konter_6 | pmode_konter_7 | pmode_konter_8 | pmode_konter_9 | pmode_konter_10; 

	// setup GPIO direction as input & interrupt
	FIO2DIR &= ~(iKonter_1 | iKonter_2 | iKonter_3 | iKonter_4 | iKonter_5);
	FIO0DIR &= ~(iKonter_6 | iKonter_7 | iKonter_8 | iKonter_9 | iKonter_10);
	
	// enable rising edge interrupt
	// inverse input	--> input MPU rising edge, masuk inverter falling edge
	
	IO2_INT_EN_R |= iKonter_1 | iKonter_2 | iKonter_3 | iKonter_4 | iKonter_5;
	IO0_INT_EN_R |= iKonter_6 | iKonter_7 | iKonter_8 | iKonter_9 | iKonter_10;
	
	#ifdef PAKAI_ADC_7708
	//FIO2DIR		 &= ~(RDY_AD7708);
	//IO2_INT_EN_F |= RDY_AD7708;
	#endif

	portEXIT_CRITICAL();
	//uprintf("%s...masuk\r\n", __FUNCTION__);
}
#endif
#ifdef PAKAI_RTC
void rtcWrite(struct tm *newTime)	{
	portENTER_CRITICAL ();

	RTC_CCR &= ~RTC_CCR_CLKEN;
	RTC_CCR |=  RTC_CCR_CTCRST;

	RTC_SEC   = newTime->tm_sec;
	RTC_MIN   = newTime->tm_min;
	RTC_HOUR  = newTime->tm_hour;
	RTC_DOM   = newTime->tm_mday;
	RTC_MONTH = newTime->tm_mon + 1;
	RTC_YEAR  = newTime->tm_year + 1900;
	RTC_DOW   = newTime->tm_wday;
	RTC_DOY   = newTime->tm_yday + 1;

	RTC_CCR &= ~RTC_CCR_CTCRST;
	RTC_CCR |=  RTC_CCR_CLKEN;

	portEXIT_CRITICAL ();
}
#endif


#ifdef PAKAI_ADC_7708
void adc_int_init()	{
	extern void ( adc_ISR_Wrapper )( void );

	portENTER_CRITICAL();
	
	VICIntSelect    &= ~VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT1);
	VICIntEnClr      = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT1);
	VICVectAddr15 = ( portLONG )adc_ISR_Wrapper;
	VICVectPriority15 = 0x05;
	VICIntEnable = VIC_CHAN_TO_MASK(VIC_CHAN_NUM_EINT1);
	
	FIO2DIR &= ~(RDY_AD7708);
	IO2_INT_EN_F |= RDY_AD7708;
	portEXIT_CRITICAL();
}
#endif
