m.bin:m.elf
	arm-linux-objcopy -O binary m.elf m.bin
	cp m.bin /tmp
	arm-linux-objdump -D -S m.elf > m.asm

m.elf:m.o start.o
	arm-linux-ld -Ttext 0x31000000 -o m.elf start.o m.o

m.o:m.c
	arm-linux-gcc -g -fPIC -c m.c

start.o:start.S
	arm-linux-gcc -fPIC -c start.S

clean:
	rm *.o *.bin *.elf
