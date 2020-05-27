SRCEXTS   := .c .S
SRCDIRS   := .
SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS    = $(foreach x,$(SRCEXTS), \
      $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))

#use_2019_gcc = true
ARMLD_FLAG = -Bstatic -pie -Ttext 0x30000000
ARMCC_FLAGS = -g -fPIE -pie
ARMS_FLAGS = -fPIE -pie
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

GIT_SHA1="$(shell git log --format='_%h ' -1)"
DIRTY="$(shell git diff --quiet || echo 'dirty')"
CLEAN="$(shell git diff --quiet && echo 'clean')"
ARMCC_FLAGS+=-DGIT_SHA1=\"$(GIT_SHA1)$(DIRTY)$(CLEAN)\"

m.bin:m.elf
	$(ARMOC) -O binary m.elf m.bin
	cp m.bin /tmp
	$(ARMOD) -D -S m.elf > m.asm

m.elf: $(OBJS) Makefile
	$(ARMLD) -o m.elf $(OBJS) -T m.lds $(ARMLD_FLAG)

%.o:%.c
	$(ARMCC) $(ARMCC_FLAGS) -c $<

%.o:%.S
	$(ARMCC) $(ARMS_FLAGS) -c $<

clean:
	rm *.o *.bin *.elf *.asm
