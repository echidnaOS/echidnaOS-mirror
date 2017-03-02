#include "libs/variables.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include "libs/partition.h"
#include "libs/memory.h"
#include <ctype.h>
#include "libs/panic.h"
#include "drivers/system.h"
#include "drivers/textdrv.h"
#include "drivers/ata.h"
#include "drivers/pic.h"
#include "drivers/gdt.h"
#include "drivers/idt.h"
#include "drivers/keyboard.h"
#include "libs/fs/fat32.h"
#include "drivers/device_abstraction.h"

void _start(void) {
	char buf[16];
        char buf_test[512] = {1};

	uint32_t memory_size = mem_load_d(0x7DF9);
	uint32_t kernel_size = mem_load_d(0x7DF5);
	uint32_t available_page = (0x1000000+kernel_size)/0x400000;
	if ((0x1000000+kernel_size)%0x400000) available_page++;
        
	text_clear();

	text_putstring("echidnaOS\n\n");

	text_putstring(itoa(memory_size, buf, 10));
	text_putstring(" bytes of memory detected.\n");
	text_putstring("The kernel is ");
	text_putstring(itoa(kernel_size, buf, 10));
	text_putstring(" bytes long.\n\n");

	text_putstring("Initialising PIC...");

	map_PIC(0x20, 0x28);	// map the PIC0 at int 0x20-0x27 and PIC1 at 0x28-0x2F

	text_putstring(" Done.\n");

	text_putstring("Building descriptor tables...");

	create_GDT();		// build the GDT
	create_IDT();		// build the IDT

	load_segments();	// activate the GDT
	enable_ints();		// activate the IDT

	printf(" Done.\n");

	if ((available_page*0x400000)+0x400000 >= memory_size) panic("insufficient memory to start ramdrive");
	memory_map(0, (available_page*0x400000), 0xD0000000, 0);	// allocate 4 MiB of memory for the ramdrive
	available_page++;

	mem_store_w(0xD0000000, 0xAA55);
	mem_load_w(0xD0000000);

        devices_initalize();
        
        partition_table table; table = enumerate_partitions("ata1");        // 0b00000000 is ATA Primary Master
        
        fat32_filesystem fs = get_fs(table.partitions[0], "ata1");          // 0b00000000 is ATA Primary Master
        
        printf("OEM name: %s\n", fs.oem_name);
        printf("Volume label: %s\n", fs.volume_name);
        printf("Serial number: %x\n", fs.serial_number);
        printf("Exists: %s\n", fs.exists != 0 ? "true" : "false");
            
        
        /*disk_load_sector(char* device_name, uint32_t lba_start, uint32_t sector_count, uint32_t mem_location)*/ 
        //disk_load_sector("ata6", 0, 1, *buf);
        
        char last_c;
        
        printf("> ");
        
        while (1) {
            char c = get_last_char();
            
            if (c != last_c) {
                text_putchar(c);
                last_c = c;
                clear_last_char();
                
                // interactive "shell"
                if (c == '\n') {
                    char* kbuf = get_keyboard_buffer();
                    if (strncmp("clear", kbuf, 5) == 0) text_clear();
                    if (strncmp("panic", kbuf, 5) == 0) panic("manually triggered panic");
                    if (strncmp("read", kbuf, 4) == 0) {
                        kbuf += 5;
                        disk_load_sector(kbuf, 0, 1, *buf);
                    }
                    
                    if (strncmp("write", kbuf, 5) == 0) {
                        kbuf += 6;
                        disk_write_sector(kbuf, 0, 1, *buf);
                    }
                    
                    if (strncmp("fsinit", kbuf, 6) == 0) {
                        kbuf += 6;
                        disk_write_sector(kbuf, 0, 1, *buf);
                    }
                    clear_keyboard_buffer();
                
                    printf("> ");
                }
            }
        }
        
	printf("\nSoft halting system.");
	system_soft_halt();

}
