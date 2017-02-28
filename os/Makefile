BUILD_DIR=../build/os
OS=$(BUILD_DIR)/kernel.elf

CFLAGS+=-Wall -I./include -O -fomit-frame-pointer -ffreestanding -fno-builtin -fno-stack-protector -nostdlib -nostartfiles -nodefaultlibs

OS_SRCS := $(wildcard *.c)
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))

all: $(OS)

$(BUILD_DIR)/%.o: %.c
	gcc $(CFLAGS) -m32 -c $< -o $@

$(OS): $(OS_OBJS)
	nasm -f elf32 loader.asm -o loader.o
	ld -melf_i386 -n -Tos.lds loader.o $(OS_OBJS) -o $(OS)


clean:
	rm $(BUILD_DIR)/*
