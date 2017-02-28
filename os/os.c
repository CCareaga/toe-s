#include "types.h"
#include "vga.h"


void main() {
	init_vga();
	
	/*int i = 0;
	for (;;) {
		i++;
		for (int j = 1; j < i; j++){
			vga_write("l");
		}
		vga_write("\n");
	}*/

	vga_write("kernel loaded!");
	
	for(;;){}
	asm("hlt");
}






