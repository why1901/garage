#include "headfile.h"


char *perrmsg = NULL;		
int flag_in   = 0;			//flag被用于判断callback是否被调用、当被调用时改变相应的标志位
int flag_out  = 0;
char lic_pth[128] = {0};	//用于获取车辆出场时查询到的以车牌号命名的照片路径	


//创建车辆信息数据库
sqlite3 *create_sqlite3(void)
{
	sqlite3 *db = NULL;
	int retval;
	
	retval = sqlite3_open("car.db", &db);
	if (retval != SQLITE_OK)
	{
		printf("car.db crreate failed\n");
		return NULL;
	}

	return db;
}


//数据库数据输出，Sqlite3_exec传入callback参数时调用
int callback(void *p, int num_of_col, char **text, char **name)
{
	int i;
	char *ptr = (char *) p;
	
	//按一定格式输出查询到的数据库内容
	for (i=0; i<num_of_col; i++)
	{
		printf("%-20s", name[i]);
	}

	printf("\n----------------------------------------------------------------------\n");

	for (i=0; i<num_of_col; i++)
	{
		printf("%-15s\t", text[i] ? text[i] : "NULL");
	}
	printf("\n");

	//改变相应出入场标志位
	if (strcmp(ptr, "in") == 0)
		flag_in = 1;
	if (strcmp(ptr, "out") == 0)
	{
		flag_out = 1;
		memcpy(lic_pth, text[3], 128);
	}

	return 0;
}


//数据库操作函数、通过调用此函数传入数据库语句、并且传入callback时可执行库内容输出
int Sqlite3_exec(sqlite3 *db, const char *sql, 
	int (*callback)(void *, int, char **, char **), void *p, char **perrmsg)
{
	int retval;

	retval = sqlite3_exec(db, sql, callback, p, perrmsg);

	return 0;
}


//LED开
void led_open(void)
{
	ioctl(led_fd, LED1, LED_ON);
	ioctl(led_fd, LED2, LED_ON);
	ioctl(led_fd, LED3, LED_ON);
	ioctl(led_fd, LED4, LED_ON);

	return ;
}
//LED关
void led_close(void)
{
	ioctl(led_fd, LED1, LED_OFF);
	ioctl(led_fd, LED2, LED_OFF);
	ioctl(led_fd, LED3, LED_OFF);
	ioctl(led_fd, LED4, LED_OFF);

	return ;
}

//蜂鸣器开
void bee_on(void)
{
	ioctl(bee_fd, BEE_ON, 1);

	return ;
}

//蜂鸣器关
void bee_close(void)
{
	ioctl(bee_fd, BEE_OFF, 1);

	return ;
}

//蜂鸣器急促响应5声
void bee_five(void)
{
	int i;

	for (i = 0; i < 5; i++)
	{
		bee_on();
		usleep(50000);
		bee_close();
		usleep(50000);
	}

	return ;
}




//车辆进场处理
int car_in(sqlite3 *db)
{
	wchar_t *wtext;
	int cardid;
	int sac2_fd;			//串口2文件描述符

	struct tm *t;
	time_t tt;

	char sql[128];			//数据库操作语句
	char time_buf[256];		//存放获取到的时间
	char pic_buf[256];		//存放生成的照片路径
	char license[256];		//存放随机生成的车辆号码
	char *p = "in";			//p传给数据库回调函数、确认是进场或出场在调用、改变相应的标志位

	wtext = L"欢迎光临";

	//打开串口2并初始化串口设置
	sac2_fd = open(SAC2_PTH, O_RDWR | O_NOCTTY);
	if (sac2_fd == -1)
	{
		printf("ttySAC2 open failed\n");
		return -1;
	}
	init_tty(sac2_fd);

	while(1)
	{
		//主界面显示欢迎的照片和欢迎光临的字体
		display_format_jpeg(PIC_PTH, lcd_buf);
		Lcd_Show_FreeType(wtext, 64, 0xFF0000, 300, 240, lcd_buf);

		//读取卡片信息
		cardid = rfid_read(sac2_fd);

		//查询车辆数据库、是否车辆已经入库重复刷卡
		bzero(sql, 128);
		sprintf(sql, "select * from carinfo where id like '%x';", cardid);
		printf("sql = %s\n", sql);
		Sqlite3_exec(db, sql, callback, (void *)p, &perrmsg);

		//flag_in为零时说明callback函数没有被调用，相应卡没有重复入场、此时将卡信息记录入数据库中
		if (flag_in == 0)
		{
			printf("flag_in = %d\n", flag_in);
			//获取此时的时间、在放于time_buf中、以便记录到数据库
			bzero(time_buf, 128);
			time(&tt);
			t = localtime(&tt);
			sprintf(time_buf,"%4d/%02d/%02d %02d:%02d:%02d",t->tm_year + 1900, 
				t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		
			//随机生成车牌号码、
			bzero(license, 128);
			get_license(&license);

			//生成摄像头拍摄照片的路径、以车牌号为结尾"../video_jpg/xxxx.jpg"
			bzero(pic_buf, 128);
			printf("license = %s\n", license);
			printf("11111\n");
			sprintf(pic_buf, "../video_jpg/%s.jpg", license);
			printf("pic_buf = %s\n", pic_buf);

			//调用摄像头拍摄车辆号码、并把照片保存在相应路径下
			get_video_jpeg(pic_buf);

			//把拍摄的车辆号码的照片显示在lcd屏幕上
			display_format_jpeg(pic_buf, lcd_buf);

			//把读取到的卡号、获取的时间、车牌号码、拍摄的车辆照片路径存入数据库中
			bzero(sql, 128);
			sprintf(sql, "insert into carinfo values('%x', '%s', '%s', '%s');", 
				cardid, license, time_buf, pic_buf);
			printf("sql = %s\n", sql);

			Sqlite3_exec(db, sql, NULL, NULL, NULL);

			//把数据库中的信息输出在终端上、以比对上面操作有没成功将数据插入到数据库中
			bzero(sql, 128);
			sprintf(sql, "select * from carinfo;");
			printf("sql =%s \n", sql);
			Sqlite3_exec(db, sql, callback, (void *)p, &perrmsg);

			//led灯亮起、并且蜂鸣器长鸣一声、系统播放欢迎光临的语音，表示可通过进入、至此一次车辆入库操作完成
			led_open();
			bee_on();
			sleep(1);
			bee_close();
			system(WELCOME_PTH);
			sleep(1);
			led_close();
		}
		/*
		flag则查询数据库时callback函数被调用、说明数据库中已存在相应卡的信息、属于重复刷卡进入、
		蜂鸣器急促响应5声、并且系统播放重复入场的语音提醒
		*/	
		else
		{
			bee_five();
			system(HAD_COME_IN);
		}

		//无论是否重复刷卡进入、或者第一次刷卡进入、最后都保证flag重新置0
		flag_in = 0;
	}	
	return 0;
}



//车辆出场处理函数
int car_out(sqlite3 *db)
{
	int sac3_fd, cardid;
	char sql[128];
	char *p = "out";

	//打开串口3并初始化配置串口3
	sac3_fd = open(SAC3_PTH, O_RDWR | O_NOCTTY);
	if (sac3_fd == -1)
	{
		printf("ttySAC3 open failed\n");
		return -1;
	}
	init_tty(sac3_fd);

	while(1)
	{
		//读取出场卡片信息
		cardid = rfid_read(sac3_fd);

		//当读取卡号为零时
		if (cardid == 0)
			continue;

		//查询车辆数据库、是否车辆已经入库刷卡
		bzero(sql, 128);
		sprintf(sql, "select * from carinfo where id like '%x';", cardid);
		printf("sql = %s\n", sql);
		Sqlite3_exec(db, sql, callback, (void *)p, &perrmsg);

		//flag_out为1时说明callback未被调用、数据库中有此卡片信息、即卡片已进场
		if (flag_out == 1)
		{
			
			//先把相应卡片在数据库中查找到的照片删除
			bzero(sql, 128);
			sprintf(sql, "rm %s", lic_pth);
			printf("rm sql = %s\n", sql);
			system(sql);
			
			//把相应卡片的信息从数据库删除
			bzero(sql, 128);
			sprintf(sql, "delete from carinfo where id like '%x';", cardid);
			Sqlite3_exec(db, sql, NULL, NULL, NULL);

			//LED灯亮起、蜂鸣器长鸣一声、然后播放离场语音提醒以示可通过
			led_open();
			bee_on();
			sleep(1);
			bee_close();
			system(EXIT_PTH);
			sleep(1);
			led_close();
		}
		/*
		若查询时callback未被调用、则说明数据库未有相应卡片的信息、即相应的卡片
		还未有进场便刷卡离场、此时蜂鸣器急促响应6声、并播放未入场语音提示
		*/
		else
		{
			bee_five();
			system(NOT_ENTRY_PTH);
		}

		//把flag_out 重新置为0
		flag_out = 0;
	}

	return 0;
}