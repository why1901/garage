
#include <stdio.h>
#include <stdlib.h>
#include "headfile.h"
#include <string.h>

char *province[] = {"粤", "赣", "湘", "鄂", "贵",
		    "黑", "沪", "京", "蒙", "藏",
		    "甘", "豫", "鲁", "晋", "桂",
		    "云", "滇", "辽", "川", "闵",
		    "徽", "苏", "津", "吉", "冀",
		    "陕", "宁", "浙", "渝", "琼",
		    "港", "澳", "台"
}; //33

char alphanumeric[] = { 'A', 'B', 'C', 'D', 'E',
			'F', 'G', 'H', 'I', 'J',
			'K', 'L', 'M', 'N', 'O',
			'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y',
			'Z', '0', '1', '2', '3',
			'4', '5', '6', '7', '8',
			'9', '0'
};  //37

//char license[100];

int get_license(char *license)
{
	//char *license = (char *)malloc(100);
	char license_num[6] = {0};
	srand(time(NULL));

	//bzero(license, 100);
	
	license_num[0] = alphanumeric[rand()%37];
	license_num[1] = alphanumeric[rand()%37];
	license_num[2] = alphanumeric[rand()%37];
	license_num[3] = alphanumeric[rand()%37];
	license_num[4] = alphanumeric[rand()%37];
	license_num[5] = alphanumeric[rand()%37];

	sprintf(license,"%s%s",province[rand()%33],license_num);
	//printf("%s\n",license);
	
	return 0;
}
