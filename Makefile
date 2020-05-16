m.bin:m.o
	arm-linux-objcopy -O binary m.o m.bin

m.o:m.c
	arm-linux-gcc -c m.c

clean:
	rm *.o *.bin
