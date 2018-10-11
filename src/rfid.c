#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "headfile.h"


char get_bcc(char *buf, int n)
{
	char bcc = 0;
	int i;
	for(i=0;i<n;i++)
	{
		bcc ^= buf[i];
	}
	
	return (~bcc);
}

int rfid_read(int fd)
{
	//3. 往RFID模块中发送请求命令字
	char wbuf[7];
	char kbuf[8];
	
	//防碰撞的应答帧是0x0A 
	char qbuf[65];
	
	bzero(wbuf,7);
	wbuf[0] = 0x07; //帧长=7byte
	wbuf[1] = 0x02; //命令类型
	wbuf[2] = 'A';  //命令'A' 代表请求
	wbuf[3] = 0x01; //信息长度
	wbuf[4] = 0x52; //信息  ALL=0x52
	wbuf[5] = get_bcc(wbuf,wbuf[0]-2); //计算校验和 
	wbuf[6] = 0x03; //结束标志

	while(1)
	{
		//不断发送数据给RFID，使得RFID模块不断请求附近有没有卡
		//往串口中发送数据
		tcflush(fd,TCIFLUSH);
		int ret = write(fd, wbuf, strlen(wbuf));
		//printf("ret = %d\n",ret);
		//等待数据全部写入RFID中
		usleep(10000);

		bzero(qbuf, 8);
		read(fd, qbuf, sizeof(qbuf));
		
		//如果状态为0，则说明有卡在附近
		if(qbuf[2] == 0x00)
		{
			printf("get card ok!\n");
			break;
		}
	}
	
	//确保卡在附近，发送防碰撞命令字给RFID模块
	int cardid;

	bzero(kbuf,8);
	kbuf[0] = 0x08;  //帧长 8byte
	kbuf[1] = 0x02;  //命令类型ISO14443A命令 0x02
	kbuf[2] = 'B';    //命令：防碰撞
	kbuf[3] = 0x02;  //信息长度
	kbuf[4] = 0x93;  //一级防碰撞
	kbuf[5] = 0x00;  //位计数
	kbuf[6] = get_bcc(kbuf,kbuf[0]-2); //校验和
	kbuf[7] = 0x03;  //结束标志
	
	//往串口中发送数据
	tcflush(fd,TCIFLUSH);
	
	//printf("%d\n",strlen(kbuf)); //5
	write(fd,kbuf,8);
		
	//等待数据全部写入RFID中
	usleep(10000);
		
	bzero(qbuf,65);
	read(fd,qbuf,65);
	
	if(qbuf[2] == 0x00)  //能够获取到卡号
	{
		int i,j;
		for(i=3,j=0;i>=0;i--,j++)
		{
			memcpy((char *)&cardid+j,&qbuf[4+i],1);
		}	
		printf("get cardid ok!\n");
	}
	else{
		printf("get cardid error!\n");
		
	}
	
	printf("cardid = %#x\n",cardid);   //#：代表带着格式输出
	
	//close(fd);

	return cardid;
}

