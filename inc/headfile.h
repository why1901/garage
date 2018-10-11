#ifndef __HEADFILE_H
#define __HEADFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <linux/videodev2.h>            
#include <errno.h>
#include <malloc.h>
#include <time.h>


#include "sqlite3.h"
#include "freetype.h"
#include "lcd.h"

#include "v4l2.h"

#define MAP_SIZE    800*480*4							//定义LCD屏幕映射空间大小

#define PIC_PTH		"../picture/welcome.jpg"			//定义界面欢迎照片的路径

#define SAC2_PTH	"/dev/ttySAC2"						//定义串口2的路径->入场串口
#define SAC3_PTH	"/dev/ttySAC3"						//定义串口3的路径->出场串口

#define WELCOME_PTH		"mplayer ../mp3/welcome.mp3"	//欢迎进入语音的路径
#define HAD_COME_IN		"mplayer ../mp3/你已经进场了.mp3"//已经进场重复刷卡语音路径

#define EXIT_PTH		"mplayer ../mp3/exit.mp3"		//离场语音路径
#define NOT_ENTRY_PTH	"mplayer ../mp3/你还没进场呢.mp3"	//还没进场刷卡离场语音路径

#define TEST_MAGIC		'x'				
#define LED1 _IO(TEST_MAGIC, 0)
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3)

#define BEE_ON		0									//LED开
#define BEE_OFF		1									//LED关

#define LED_ON		0									//蜂鸣器开
#define LED_OFF		1									//蜂鸣器关



extern int init_tty();									

extern int rfid_read();

extern int get_license();

extern void led_open();

extern void led_close();

extern void bee_on();

extern void bee_close();

extern void bee_five();

extern int car_in();

extern sqlite3 *create_sqlite3();

extern int callback();

extern int Sqlite3_exec();

extern int car_out();

#endif