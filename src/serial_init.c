#include "headfile.h"



int init_tty(int fd)
{
	struct termios old_flags,new_flags;
	bzero(&new_flags,sizeof(new_flags));
	
	//1. 获取旧的属性
	tcgetattr(fd,&old_flags);
	
	//2. 设置原始模式
	cfmakeraw(&new_flags);
	
	//3. 激活本地连接CLOCAL与接收使能CREAD的选项
	new_flags.c_cflag |= CLOCAL | CREAD; 
	
	//4. 设置波特率
	cfsetispeed(&new_flags, B9600); 
	cfsetospeed(&new_flags, B9600);
	
	//5. 设置数据位为8位
	new_flags.c_cflag &= ~CSIZE; //清空原有的数据位
	new_flags.c_cflag |= CS8;
	
	//6. 设置奇偶检验位
	new_flags.c_cflag &= ~PARENB;
	
	//7. 设置一位停止位
	new_flags.c_cflag &= ~CSTOPB;
	
	//8. 设置等待时间，最少接收字符个数
	new_flags.c_cc[VTIME] = 0;
	new_flags.c_cc[VMIN] = 1;
	
	//9. 清空串口缓冲区
	tcflush(fd, TCIFLUSH);
	
	//10. 设置串口的属性到文件中
	tcsetattr(fd, TCSANOW, &new_flags);
	
	return 0;
}