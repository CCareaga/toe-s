BUILD_DIR=../build/os
OS=$(BUILD_DIR)/kernel.elf

CFLAGS+=-ffreestanding -O2 -Wall -Wextra

OS_SRCS := $(wildcard *.c)
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))

all: $(OS)

$(BUILD_DIR)/%.o: %.c
	i386-elf-gcc $(CFLAGS) -c $< -o $@

$(OS): $(OS_OBJS)
	nasm -f elf32 loader.asm -o loader.o
	nasm -f elf32 gdt.asm -o gdt.o
	i386-elf-gcc -T os.lds -o $(OS) -ffreestanding -O2 -nostdlib loader.o $(OS_OBJS) gdt.o -lgcc

clean:
	rm $(BUILD_DIR)/*
