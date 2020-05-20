ARMPRE = arm-linux-
ARMCC = $(ARMPRE)gcc
ARMLD = $(ARMPRE)ld
ARMOC = $(ARMPRE)objcopy
ARMOD = $(ARMPRE)objdump

m.bin:m.elf
	$(ARMOC) -O binary m.elf m.bin
	cp m.bin /tmp
	$(ARMOD) -D -S m.elf > m.asm

m.elf:m.o start.o
	$(ARMLD) -Bstatic -Ttext 0x31000000 -L/home/clean/tool/arm/3.4.1/arm-linux/lib/ -L/home/clean/tool/arm//3.4.1/lib/gcc/arm-linux/3.4.1/ -lm -lc -lgcc -o m.elf start.o m.o

m.o:m.c
	$(ARMCC) -g -fPIC -c m.c

start.o:start.S
	$(ARMCC) -fPIC -c start.S

clean:
	rm *.o *.bin *.elf *.asm
