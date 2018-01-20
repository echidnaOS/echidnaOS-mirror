#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdint.h>
#include <stddef.h>




#define _BIG_FONTS_
//#define _SERIAL_KERNEL_OUTPUT_

#ifdef _BIG_FONTS_
  #define VD_ROWS 25
#else
  #define VD_ROWS 50
#endif
#define VD_COLS 160

#define KB_L1_SIZE 256
#define KB_L2_SIZE 2048

#define KRNL_PIT_FREQ 4000
#define KRNL_TTY_COUNT 7

#define TTY_DEF_CUR_PAL 0x70
#define TTY_DEF_TXT_PAL 0x07





#define MAX_SIMULTANOUS_VFS_ACCESS  4096

// memory statuses

// signals

#define SIGABRT                 0
#define SIGFPE                  1
#define SIGILL                  2
#define SIGINT                  3
#define SIGSEGV                 4
#define SIGTERM                 5

#define SIG_ERR                 0xffffffff

// macros

#define DISABLE_INTERRUPTS      asm volatile ("cli")
#define ENABLE_INTERRUPTS       asm volatile ("sti")
#define ENTER_IDLE              \
    asm volatile (              \
                    "sti;"      \
                    "1:"        \
                    "mov esp, 0xeffff0;"    \
                    "hlt;"      \
                    "jmp 1b;"   \
                 )



typedef struct {
    uint8_t version_min;
    uint8_t version_maj;
    char* oem;
    uint32_t capabilities;
    uint16_t* vid_modes;
    uint16_t vid_mem_blocks;
    uint16_t software_rev;
    char* vendor;
    char* prod_name;
    char* prod_rev;
} __attribute__((packed)) vbe_info_struct_t;

// driver inits

void init_pcspk(void);
void init_tty_drv(void);
void init_streams(void);
void init_com(void);
void init_stty(void);
void init_graphics(void);
void init_initramfs(void);

void graphics_init(vbe_info_struct_t* vbe_info_struct);

// end driver inits
// fs inits

void install_devfs(void);
void install_echfs(void);

// end fs inits

void kernel_add_device(char* name, uint32_t gp_value, uint64_t size,
                       int (*io_wrapper)(uint32_t, uint64_t, int, uint8_t));

// prototypes

#ifdef _SERIAL_KERNEL_OUTPUT_
  void debug_kernel_console_init(void);
  int com_io_wrapper(uint32_t dev, uint64_t loc, int type, uint8_t payload);
#endif

void switch_tty(uint8_t which_tty);
void init_tty(void);

typedef struct {
    char name[32];
    uint32_t gp_value;
    uint64_t size;
    int (*io_wrapper)(uint32_t, uint64_t, int, uint8_t);
} device_t;

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) GDT_entry_t;

typedef struct {
    uint32_t cursor_offset;
    int cursor_status;
    uint8_t cursor_palette;
    uint8_t text_palette;
    char field[VD_ROWS * VD_COLS];
    char kb_l1_buffer[KB_L1_SIZE];
    char kb_l2_buffer[KB_L2_SIZE];
    uint16_t kb_l1_buffer_index;
    uint16_t kb_l2_buffer_index;
    int escape;
    int* esc_value;
    int esc_value0;
    int esc_value1;
    int* esc_default;
    int esc_default0;
    int esc_default1;
    int raw;
    int noblock;
    int noscroll;
} tty_t;

extern uint32_t memory_size;
extern uint8_t current_tty;

extern tty_t tty[KRNL_TTY_COUNT];

extern device_t* device_list;
extern uint32_t device_ptr;

void panic(const char *msg);

void vga_disable_cursor(void);
void vga_80_x_50(void);
uint32_t detect_mem(void);

void tty_refresh(uint8_t which_tty);

void text_putchar(char c, uint8_t which_tty);
uint32_t text_get_cursor_pos_x(uint8_t which_tty);
uint32_t text_get_cursor_pos_y(uint8_t which_tty);
void text_set_cursor_pos(uint32_t x, uint32_t y, uint8_t which_tty);
void text_set_cursor_palette(uint8_t c, uint8_t which_tty);
uint8_t text_get_cursor_palette(uint8_t which_tty);
void text_set_text_palette(uint8_t c, uint8_t which_tty);
uint8_t text_get_text_palette(uint8_t which_tty);
void text_clear(uint8_t which_tty);
void text_disable_cursor(uint8_t which_tty);
void text_enable_cursor(uint8_t which_tty);

void map_PIC(uint8_t PIC0Offset, uint8_t PIC1Offset);
void set_PIC0_mask(uint8_t mask);
void set_PIC1_mask(uint8_t mask);
uint8_t get_PIC0_mask(void);
uint8_t get_PIC1_mask(void);

void set_pit_freq(uint32_t frequency);

void load_GDT(void);
void load_TSS(void);

void load_IDT(void);

void keyboard_init(void);
void keyboard_handler(uint8_t input_byte);
int keyboard_fetch_char(uint8_t which_tty);

#endif
