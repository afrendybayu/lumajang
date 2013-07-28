#/*
#    FreeRTOS V7.5.2 - Copyright (C) 2013 Real Time Engineers Ltd.
#	
#
#    ***************************************************************************
#     *                                                                       *
#     *    FreeRTOS tutorial books are available in pdf and paperback.        *
#     *    Complete, revised, and edited pdf reference manuals are also       *
#     *    available.                                                         *
#     *                                                                       *
#     *    Purchasing FreeRTOS documentation will not only help you, by       *
#     *    ensuring you get running as quickly as possible and with an        *
#     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
#     *    the FreeRTOS project to continue with its mission of providing     *
#     *    professional grade, cross platform, de facto standard solutions    *
#     *    for microcontrollers - completely free of charge!                  *
#     *                                                                       *
#     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
#     *                                                                       *
#     *    Thank you for using FreeRTOS, and thank you for your support!      *
#     *                                                                       *
#    ***************************************************************************
#
#
#    This file is part of the FreeRTOS distribution.
#
#    FreeRTOS is free software; you can redistribute it and/or modify it under
#    the terms of the GNU General Public License (version 2) as published by the
#    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
#    >>>NOTE<<< The modification to the GPL is included to allow you to
#    distribute a combined work that includes FreeRTOS without being obliged to
#    provide the source code for proprietary components outside of the FreeRTOS
#    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#    more details. You should have received a copy of the GNU General Public
#    License and the FreeRTOS license exception along with FreeRTOS; if not it
#    can be viewed here: http://www.freertos.org/a00114.html and also obtained
#    by writing to Richard Barry, contact details for whom are available on the
#    FreeRTOS WEB site.
#
#    1 tab == 4 spaces!
#
#    http://www.FreeRTOS.org - Documentation, latest information, license and
#    contact details.
#
#    http://www.SafeRTOS.com - A version that is certified for use in safety
#    critical systems.
#
#    http://www.OpenRTOS.com - Commercial support, development, porting,
#    licensing and training services.
#*/

NAMA_FILE=santer
RTOS_SOURCE_DIR=../../../Source
DEMO_COMMON_DIR=../../Common/Minimal
DEMO_INCLUDE_DIR=../../Common/include
#UIP_COMMON_DIR=../../Common/ethernet/uIP/uip-1.0/uip
PENULIS=../../../../../../atinom/modul/Penulis/lpc_dbe

HW=hardware
MODUL=modul
CMD=cmd
APP=app

GCC=../../../../../../atinom/modul/TOOLCHAIN/bin/
OBJCOPY=$(GCC)arm-elf-objcopy
CC=$(GCC)arm-elf-gcc
UKURAN=$(GCC)arm-elf-size
DISAMB=$(GCC)arm-elf-objdump

LDSCRIPT=$(HW)/lpc2368.ld
BOOTS=$(HW)/boot.s
#LINKER_FLAGS=-mthumb -nostartfiles -Xlinker -oRTOSDemo.elf -Xlinker -M -Xlinker -Map=rtosdemo.map
LINKER_FLAGS=-mthumb -nostartfiles -Xlinker -o$(NAMA_FILE).elf -Xlinker -M -Xlinker -Map=hasil/$(NAMA_FILE).map
#DEBUG=-g
OPTIM=-O0

LIBS = -lc -lgcc -lm -Llib

CFLAGS= $(DEBUG) \
		$(OPTIM) \
		-T$(LDSCRIPT) \
		-I . \
		-I ./$(HW)/	\
		-I ./$(MODUL)/	\
		-I ./$(CMD)/	\
		-I ./$(APP)/	\
		-I $(RTOS_SOURCE_DIR)/include \
		-I $(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx \
		-I $(DEMO_INCLUDE_DIR) \
		-D ROWLEY_LPC23xx \
		-D THUMB_INTERWORK \
		-mcpu=arm7tdmi \
		-D PACK_STRUCT_END=__attribute\(\(packed\)\) \
		-D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
		-fomit-frame-pointer \
		-mthumb-interwork \

		
THUMB_SOURCE= \
		main.c \
		$(HW)/hardware.c		\
		$(HW)/cpu_setup.c	\
		$(HW)/syscalls.c		\
		$(HW)/iap.c			\
		$(CMD)/sh_data.c			\
		$(APP)/ap_rpm.c				\
		$(APP)/ap_ambilcepat.c		\
		$(RTOS_SOURCE_DIR)/list.c \
		$(RTOS_SOURCE_DIR)/queue.c \
		$(RTOS_SOURCE_DIR)/tasks.c \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx/port.c \
		$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c \
#		$(CMD)/sh_rtos.c			\
#		$(CMD)/sh_hardware.c		\

SERIAL_SOURCE=	\
		$(MODUL)/serial/serial.c 	\
		$(CMD)/sh_serial.c			\
#		$(CMD)/serial_tes.c			\		



		

ARM_SOURCE= \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx/portISR.c \
		

SERIAL_SOURCE_ISR=	\
		$(MODUL)/serial/serialISR.c	\
		
		
		
THUMB_OBJS = $(THUMB_SOURCE:.c=.o)
ARM_OBJS = $(ARM_SOURCE:.c=.o)

THUMB_SOURCE += $(SERIAL_SOURCE)

ARM_SOURCE	+=  $(SERIAL_SOURCE_ISR)

all: RTOSDemo.bin sizebefore

RTOSDemo.bin : RTOSDemo.hex
	$(OBJCOPY) $(NAMA_FILE).elf -O binary hasil/$(NAMA_FILE).bin
	 
RTOSDemo.hex : RTOSDemo.elf
	$(OBJCOPY) $(NAMA_FILE).elf -O ihex $(NAMA_FILE).hex
	cp $(NAMA_FILE).hex hasil/$(NAMA_FILE).hex
	rm $(THUMB_OBJS)
	rm $(ARM_OBJS)

RTOSDemo.elf : $(THUMB_OBJS) $(ARM_OBJS) $(BOOTS) Makefile
	$(CC) $(CFLAGS) $(ARM_OBJS) $(THUMB_OBJS) $(LIBS) $(BOOTS) $(LINKER_FLAGS) 

$(THUMB_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) -mthumb $< -o $@

$(ARM_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm $(NAMA_FILE).elf
	rm $(NAMA_FILE).hex
	rm hasil/$(NAMA_FILE).bin
	rm hasil/$(NAMA_FILE).map
	rm $(THUMB_OBJS)
	rm $(ARM_OBJS)

#	touch Makefile
	

tulis:
	sudo $(PENULIS) -hex $(NAMA_FILE).hex /dev/ttyUSB0 115200 14748

tulis1 :
	sudo $(PENULIS) -hex $(NAMA_FILE).hex /dev/ttyUSB1 115200 14748


MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:

HEXSIZE = $(UKURAN) --target=$(FORMAT) $(NAMA_FILE).hex
ELFSIZE = $(UKURAN) -A $(NAMA_FILE).elf
sizebefore:
	@if [ -f $(NAMA_FILE).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi
