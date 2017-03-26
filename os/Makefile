BUILD_DIR=../build/os
OS=$(BUILD_DIR)/kernel.elf
ASM=asm

CFLAGS+=-ffreestanding -O2 -Wall -Wextra -nostdlib -m32

OS_SRCS := $(wildcard *.c)
OS_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(OS_SRCS))

ASM_SRCS := $(wildcard $(ASM)/*.asm)
ASM_OBJS := $(patsubst $(ASM)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SRCS))

$(info $(ASM_OBJS))

all: $(OS)

$(BUILD_DIR)/%.o: %.c
	i386-elf-gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(ASM)/%.asm
	nasm -f elf32 $< -o $@

$(OS): $(OS_OBJS) $(ASM_OBJS)
	ld -m32 -melf_i386 -T linker.lds -s -o $(OS) $(OS_OBJS) $(ASM_OBJS)

clean:
	rm $(BUILD_DIR)/*
