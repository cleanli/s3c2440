#use_2019_gcc = true
ARMLD_FLAG = -Bstatic -Ttext 0x33000000
ifeq ($(use_2019_gcc),true)
ARMPRE = arm-none-eabi-
ARMLD_FLAG += -L/home/clean/tool/gcc-arm-none-eabi-9-2019-q4-major/arm-none-eabi/lib/ -L/home/clean/tool/gcc-arm-none-eabi-9-2019-q4-major/lib/gcc/arm-none-eabi/9.2.1 -lm -lc -lgcc
else
ARMPRE = arm-linux-
ARMLD_FLAG += -L/home/clean/tool/arm/3.4.1/arm-linux/lib/ -L/home/clean/tool/arm/3.4.1/lib/gcc/arm-linux/3.4.1 -lm -lc -lgcc
endif
ARMCC = $(ARMPRE)gcc
ARMLD = $(ARMPRE)ld
ARMOC = $(ARMPRE)objcopy
ARMOD = $(ARMPRE)objdump

m.bin:m.elf
	$(ARMOC) -O binary m.elf m.bin
	cp m.bin /tmp
	$(ARMOD) -D -S m.elf > m.asm

m.elf:m.o sha256.o start.o Makefile
	$(ARMLD) -o m.elf start.o sha256.o m.o $(ARMLD_FLAG)

m.o:m.c
	$(ARMCC) -g -fPIC -c m.c

sha256.o:sha256.c
	$(ARMCC) -g -fPIC -c sha256.c

start.o:start.S
	$(ARMCC) -fPIC -c start.S

clean:
	rm *.o *.bin *.elf *.asm
