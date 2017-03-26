BUILD_DIR=../build/bootloader

BOOTLOADER_SRC=boot.asm

all: bootloader

bootloader:
	nasm -f bin boot.asm -o $(BUILD_DIR)/bootloader

clean:
	rm $(BUILD_DIR)/*