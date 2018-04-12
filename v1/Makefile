BUILD_DIR=build

BOOTLOADER=$(BUILD_DIR)/bootloader/boot.o
OS=$(BUILD_DIR)/os/kernel.elf
DISK_IMG=floppy.img

all: bootdisk

.PHONY: bootdisk os

os:
	make -C os

bootdisk: os
	bash build.sh

qemu:
	sudo /sbin/losetup /dev/loop0 $(DISK_IMG)
	qemu-system-i386 -machine q35 -fda $(DISK_IMG) -gdb tcp::26000 -S
	sudo /sbin/losetup -d /dev/loop0 

clean:
	make -C os clean
