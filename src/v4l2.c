#include "headfile.h"


//保存一张图片
int save_jpeg(void *data_start, size_t length, char *license)
{   
    printf("save jpeg\n");
    FILE *fp = fopen(license, "a+");
    if (fp < 0) 
    {
            printf("open frame data file failed\n");
            return -1;
    }
    fseek(fp, 0L, SEEK_SET);           //将文件指针偏移到头
    fwrite(data_start, 1, length, fp);  //写入文件
    fclose(fp);                         //关闭文件

    return 0;
}


//初始化摄像头
int camera_init(void)
{
	//打开摄像头设备
	camera_fd = open(CAMERA_DEVICE, O_RDWR);
	if (camera_fd == -1)
	{
		printf("open camera failued\n");
		return -1;
	}

	return camera_fd;
}

//获取V4L2的驱动信息
int get_v4l2_info_driver(void)
{
	int retval;
	struct v4l2_capability cap;

	retval = ioctl(camera_fd, VIDIOC_QUERYCAP, &cap);
	if (retval < 0)
	{
		printf("get v4l2 info driver failed\n");
		return -1;
	}

	return 0;
}

//设置视频格式
int set_v4l2_format(void)
{
	int retval;

	memset(&fmt, 0, sizeof(fmt));

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = VIDEO_WIDTH; 			//640
    fmt.fmt.pix.height      = VIDEO_HEIGHT; 		//480
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;	//JPEG格式
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    retval = ioctl(camera_fd, VIDIOC_S_FMT, &fmt);
    if (retval < 0)
    {
    	printf("set v4l2 format failed\n");
    	return -1;
    }

    return 0;
}


//申请v4l2的数据缓冲帧的缓冲区（几个）
int request_v4l2_buffer(void)
{
	int retval;
    struct v4l2_requestbuffers reqbuf;
    reqbuf.count = BUFFER_COUNT; 		//申请一个拥有四个缓冲帧的缓冲区
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;

    retval = ioctl(camera_fd , VIDIOC_REQBUFS, &reqbuf);
    if(retval < 0) 
    {
        printf("VIDIOC_REQBUFS failed (%d)\n", retval);
        return -1;
    }

    return 0;
}

//获取v4l2的缓冲帧的数据，并将相关信息经过映射后直接存入自己定义的结构体
int get_v4l2_info_buffer(void)
{
	int i, retval;

	for (i = 0; i < BUFFER_COUNT; i++)
	{
		buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        retval = ioctl(camera_fd , VIDIOC_QUERYBUF, &buf);
        if (retval < 0)
        {
        	printf("get v4l2 info buffer failed\n");
        	return -1;
        }

        framebuf[i].length = buf.length; 	//缓冲帧的长度

        //得到用户空间映射地址
        framebuf[i].start = (char *) mmap(0, buf.length, 
        	PROT_READ|PROT_WRITE, MAP_SHARED, camera_fd, buf.m.offset);

        if (framebuf[i].start == MAP_FAILED) 
        {
            printf("mmap (%d) failed: %s\n", i, strerror(errno));
            return -1;
        }

        //把缓冲帧放入缓冲队列
		retval = ioctl(camera_fd , VIDIOC_QBUF, &buf);		
        if (retval < 0)
        {
        	printf("info buffer qbuf failed\n");
        	return -1;
        }   
	}

	return 0;
}

//启动摄像的开始录制数据
int v4l2_vedio_start(void)
{
	int retval;

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    retval = ioctl(camera_fd, VIDIOC_STREAMON, &type); //启动数据流
    if (retval < 0)
    {
    	printf("video start failed\n");
    	return -1;
    }

	return 0;
}

//取出缓冲帧中的数据，进行相应的处理后，重新将缓冲帧放入缓冲队列
int v4l2_data_handle(int choose, char *license)
{
	int retval;

	//从队列中取出帧
	retval = ioctl(camera_fd, VIDIOC_DQBUF, &buf); 
	if (retval < 0)
	{
		printf("v4l2 data dqbuf failed\n");
		return -1;
	}

	/*数据处理
		0、写入一张照片
		1、直接在屏幕上显示
	*/
	switch(choose)
    {
        case 0:
        	save_jpeg(framebuf[buf.index].start, buf.length, license);
        	break;
        case 1:
        	//show_v4l2_jpeg_data(framebuf[buf.index].start,buf.length,0,0,fb_mem);
        	break;
        
        default:
        	break;
    }

    //把缓冲帧放入缓冲队列
    retval = ioctl(camera_fd, VIDIOC_QBUF, &buf);
    if (retval < 0) 
    {
         printf("data handle qbuf failed \n");
        return -1;
    }

    return 0;
}

//将camrea和fb相关的fd和buff删掉
int camera_and_fb_uinit(void)
{
    int i;
    for(i=0; i<4; i++) 
    {
        munmap(framebuf[i].start, framebuf[i].length);
    }

    close(camera_fd);
    return 0;
}


//拍摄照片
int get_video_jpeg(char *license)
{
	camera_init();

	request_v4l2_buffer();

	get_v4l2_info_buffer();

	v4l2_vedio_start();

	v4l2_data_handle(0, license);

	camera_and_fb_uinit();

	return 0;
}