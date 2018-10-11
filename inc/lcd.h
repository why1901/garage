#ifndef __LCD_H
#define __LCD_H

extern int open_lcd_drive();

extern int close_lcd_drive();

extern int open_led_drive();

extern void draw_a_point_to_lcd();

extern int open_screen_drive();

extern int open_bee_drive();

extern int drive_init();

extern int display_format_jpeg();

int lcd_fd, led_fd, bee_fd, ts_fd;
unsigned int *lcd_buf;


#endif
