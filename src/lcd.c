#include "headfile.h"
#include "lcd.h"


/*打开lcd屏幕并映射屏幕内存空间*/
int open_lcd_drive(void)
{
    lcd_fd = open("/dev/fb0",O_RDWR);
    if(lcd_fd == -1)
    {
        perror("open lcd drive error\n");
        return -1;
    }

    lcd_buf = mmap(NULL,MAP_SIZE,PROT_READ|PROT_WRITE,
            MAP_SHARED,lcd_fd,0);

    if(lcd_buf == NULL)
    {
        perror("mmap error\n");
        return -1;
    }

    return 0;
}


int open_led_drive(void)
{
    int retval;

    led_fd = open("/dev/Led", O_RDWR);
    if (led_fd == -1)
    {
        printf("open led failure\n");
        return -1;
    }

    return 0;

}

/*打开蜂鸣器文件*/
int open_bee_drive(void)
{
    bee_fd = open("/dev/beep", O_RDWR);
    if (bee_fd == -1)
    {
        printf("beep open failure\n");
        return -1;
    }
    return 0;
}

int open_screen_drive(void)
{  
    /*打开触摸屏文件*/
    ts_fd = open("/dev/input/event0", O_RDONLY);
    if (ts_fd == -1)
    {
        printf("open screen error = %s\n");
        return ;
    }
    return 0;
}

/*关闭lcd屏幕，并取消内存映射*/
int close_lcd_drive(void)
{
    munmap(lcd_buf, MAP_SIZE);

    return close(lcd_fd);
}


/*在屏幕上绘点*/
void draw_a_point_to_lcd(int x, int y, unsigned int color, unsigned int *lcd_buf)
{

    *(lcd_buf+800*y+x) = color;

    return ;
}

int drive_init(void)
{
    open_lcd_drive();
    open_led_drive();
    open_bee_drive();

    return 0;
}

