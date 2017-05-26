#include <stdarg.h>


#define GREEN 2
#define BLUE 3
#define RED 4
#define PINK 5
#define ORANGE 6
#define LIGHT_GREY 7
#define GREY 8
#define OCEAN_BLUE 9
#define LIME_GREEN 10
#define BRIGHT_BLUE 11
#define CORAL 12
#define BRIGHT_PINK 13
#define YELLOW 14
#define WHITE 15


#define VGA_BLACK 0x00
#define VGA_BLUE 0x01
#define VGA_GREEN 0x02
#define VGA_CYAN 0x03
#define VGA_RED 0x04
#define VGA_PURPLE 0x05
#define VGA_ORANGE 0x06
#define VGA_LIGHT_GREY 0x07
#define VGA_DARK_GREY 0x08
#define VGA_BRIGHT_BLUE 0x09
#define VGA_BRIGHT_GREEN 0x0A
#define VGA_BRIGHT_CYAN 0x0B
#define VGA_MAGENTA 0x0C
#define VGA_BRIGHT_PURPLE 0x0D
#define VGA_YELLOW 0x0E
#define VGA_WHITE 0x0F


extern void VGA_clear(void);
extern void VGA_display_char(char);
extern void VGA_display_str(const char *);

extern int VGA_row_count(void);
extern int VGA_col_count(void);
extern void VGA_display_attr_char(int x, int y, char c, int fg, int bg);


extern void change_text_color(int);
extern void new_text_color(void);

extern void backspace(void);

extern int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
extern void print_uint(unsigned int);
extern void print_int(int);
extern void print_char(char);
extern void print_str(const char *);
/*extern void print_uchar(unsigned char);*/
extern void print_short(short);
extern void print_long_hex(long);
extern void print_long(long);
extern void print_quad(long long);
