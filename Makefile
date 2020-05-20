ARMPRE = arm-none-eabi-
ARMCC = $(ARMPRE)gcc
ARMLD = $(ARMPRE)ld
ARMOC = $(ARMPRE)objcopy
ARMOD = $(ARMPRE)objdump

m.bin:m.elf
	$(ARMOC) -O binary m.elf m.bin
	cp m.bin /tmp
	$(ARMOD) -D -S m.elf > m.asm

m.elf:m.o start.o
	$(ARMLD) -Bstatic -Ttext 0x31000000 -pie -o m.elf start.o m.o -L/home/clean/tool/gcc-arm-none-eabi-9-2019-q4-major/arm-none-eabi/lib/ -L/home/clean/tool/gcc-arm-none-eabi-9-2019-q4-major/lib/gcc/arm-none-eabi/9.2.1 -lm -lc -lgcc

m.o:m.c
	$(ARMCC) -g -fPIC -pie -c m.c

start.o:start.S
	$(ARMCC) -fPIC -pie -c start.S

clean:
	rm *.o *.bin *.elf *.asm
