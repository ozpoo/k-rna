#ifndef VGA_H
#define VGA_H

void vga_clear(void);
void vga_putchar(char c, unsigned char color);
void vga_print(const char* str, unsigned char color);
void vga_print_int(int n, unsigned char color);
void vga_hide_cursor(void);

#endif