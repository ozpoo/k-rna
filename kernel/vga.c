#include "config.h"

#define VGA_WIDTH   CFG_SCREEN_WIDTH
#define VGA_HEIGHT  CFG_SCREEN_HEIGHT

static unsigned short* vga = (unsigned short*)CFG_VGA_ADDR;
static int cursor_x = 0;
static int cursor_y = 0;

static void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void vga_hide_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        vga[i] = (unsigned short)' ' | (0x0F << 8);
    cursor_x = 0;
    cursor_y = 0;
}

static void scroll(void) {
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
        vga[i] = vga[i + VGA_WIDTH];
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++)
        vga[i] = (unsigned short)' ' | (0x0F << 8);
    cursor_y = VGA_HEIGHT - 1;
}

void vga_putchar(char c, unsigned char color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        vga[cursor_y * VGA_WIDTH + cursor_x] = (unsigned short)c | ((unsigned short)color << 8);
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    if (cursor_y >= VGA_HEIGHT)
        scroll();
}

void vga_print(const char* str, unsigned char color) {
    for (int i = 0; str[i] != '\0'; i++)
        vga_putchar(str[i], color);
}

void vga_print_int(int n, unsigned char color) {
    if (n == 0) { vga_putchar('0', color); return; }
    char buf[32];
    int i = 0;
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    for (int j = i - 1; j >= 0; j--)
        vga_putchar(buf[j], color);
}

void vga_print_hex(unsigned int n, unsigned char color) {
    const char* hex = "0123456789ABCDEF";
    vga_print("0x", color);
    for (int i = 28; i >= 0; i -= 4)
        vga_putchar(hex[(n >> i) & 0xF], color);
}