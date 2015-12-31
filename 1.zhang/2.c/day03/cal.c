#include <stdio.h>

#define BASEYEAR	1990
//1990.1.1星期一
int main(void)
{
	int year, month;		
	int i;
	int sumday = 0;
	int firstday;
	int monthday;

	do {
		printf("input year and month:");
		scanf("%d %d", &year, &month);
	} while ((year < 1990 || month <= 0 || month >= 13) && printf("input again\n"));

	// 2003 5

	//1990-2002
	for (i = BASEYEAR; i < year; i++) {
		sumday += ((i%4==0&&i%100!=0)||(i%400==0)) ? 366:365;	
	}
	for (i = 1; i < month; i++) {
		switch (i) {
			case 1: case 3: case 5: case 7: case 8: case 10: case 12:
				sumday += 31; break;
			case 4: case 6: case 9: case 11:
				sumday += 30;break;
			case 2:
				sumday += (year%4==0 && year%100!=0) || (year%400==0) ? 29:28; break;
		}
	}

	firstday = (sumday + 1) % 7;
	//printf("firstday = %d\n", firstday);

	switch (month) {
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
			monthday = 31; break;
		case 4: case 6: case 9: case 11:
			monthday = 30; break;
		case 2:
			monthday = (year%4==0 && year%100!=0) || (year%400==0) ? 29:28; break;
	}

	printf("      %d 月 %d 年\n", month, year);
	printf("日 一 二 三 四 五 六\n");
	for (i = 1; i <= firstday; i++) {
		printf("   ");
	}
	for (i = 1; i <= monthday; i++) {
		printf("%2d%c", i, (firstday+i)%7==0?'\n':' ');
	}
	printf("\n");

	return 0;
}
