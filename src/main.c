#include "headfile.h"

#define CREATE_TABLE 	"create table if not exists carinfo(id text primary key not NULL,carid text, time text, photo text);"


int main(void)
{
	sqlite3 *db;		
	int retval;
	int status;
	char *sql;
	pid_t pid;

	//初始化硬件驱动、lcd屏幕、led灯、beep蜂鸣器、lcd屏幕内存映射
	drive_init();

	//关闭LED灯光
	led_close();

	//创建一个车辆信息数据库
	db = create_sqlite3();
	if (db == NULL)
		return -1;

	//创建一个数据库表（卡id,车牌号、时间、照片路径）
	sql = CREATE_TABLE;
	Sqlite3_exec(db, sql, NULL, NULL, NULL);

	//新建进程处理出场
	pid = fork();
	if (pid == 0)
	{
		car_out(db);		//车辆出场处理函数
	}

	//车辆进场处理
	car_in(db);

	wait(&status);		//等待结合子进程

	return 0;
}