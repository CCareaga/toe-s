BUILD_DIR=../build/bootloader

BOOTLOADER_SRC=bootloader.asm

all: bootloader

bootloader:
	nasm -f bin bootloader.asm -o $(BUILD_DIR)/bootloader

clean:
	rm $(BUILD_DIR)/*