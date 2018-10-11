#ifndef __V4L2_H
#define __V4L2_H

#include <stdio.h>
#include <linux/videodev2.h>

#define CAMERA_DEVICE "/dev/video7"

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FORMAT V4L2_PIX_FMT_YUYV
#define BUFFER_COUNT 4


//用来存储每一帧转换出来的数据帧
typedef struct VideoBuffer {
    void   *start;  	//映射到用户空间的地址
    size_t  length; 	//采集到数据的长度
} VideoBuffer; 			//存放采集数据的位置

VideoBuffer framebuf[BUFFER_COUNT]; 

//摄像头的缓冲帧的数据结构体
struct v4l2_buffer buf;
int camera_fd;

struct v4l2_format fmt;	//视频格式的结构体变量 


extern int save_jpeg();

extern int camera_init();

extern int get_v4l2_info_driver();

extern int set_v4l2_format();

extern int request_v4l2_buffer();

extern int get_v4l2_info_buffer();

extern int v4l2_video_start();

extern int v4l2_data_handle();

extern int camera_and_fb_uinit();

#endif