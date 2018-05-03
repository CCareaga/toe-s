
KERN_TARG = ~/i386-elf-7.3.0-Linux-x86_64/bin/i386-elf

# build the kernel

CFLAGS = -ffreestanding
CFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
CFLAGS += -pedantic -fno-omit-frame-pointer

ASFLAGS = --32

KCC = $(KERN_TARG)-gcc
KNM = $(KERN_TARG)-nm
KCXX= $(KERN_TARG)-g++
KAR = $(KERN_TARG)-ar
KAS = $(KERN_TARG)-as
KSTRIP = $(KERN_TARG)-strip
KLD = $(KERN_TARG)-ld
KOC = $(KERN_TARG)-objcopy

KERN_OBJS = $(patsubst %.c,%.o,$(wildcard kern/*.c))
KERN_OBJS += $(patsubst %.c,%.o,$(wildcard kern/*/*.c))

KERN_ASMOBJS = $(patsubst %.S,%.o,$(wildcard kern/asm/*.S))

kernel: ${KERN_ASMOBJS} ${KERN_OBJS}
	${KCC} -T kern/link.ld ${CFLAGS} -g -nostdlib -o kernel ${KERN_ASMOBJS} ${KERN_OBJS} -lgcc
	${KOC} --only-keep-debug kernel kernel.sym
	${KOC} --strip-debug kernel

HEADERS = $(shell find kern/inc/ -type f -name '*.h')

kern/asm/%.o: kern/asm/%.S
	${KAS} ${ASFLAGS} $< -o $@

kern/%.o: kern/%.c ${HEADERS}
	${KCC} ${CFLAGS} -nostdlib -g -I./kern/inc -c -o $@ $<

# cleaning 
clean:
	@-rm -f kern/*.o
	@-rm -f kern/*/*.o
	@-rm -f ${KERN_OBJS}
	@-rm -f kernel.sym
	@-rm -f kernel

# run the kernel with qemu
qemu-debug:
	qemu-system-i386 -s -S -kernel kernel

qemu:
	qemu-system-i386 -kernel kernel 

# test
test:
	gcc tests/bs.c tests/bit_test.c -o tests/bitset

