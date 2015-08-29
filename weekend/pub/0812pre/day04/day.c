#include <stdio.h>

int main(void)
{
	int year, month;
	int firstday;
	int days = 0;
	int i;
	int monthday;

	printf("input a year and month(1998,8):");
	scanf("%d,%d", &year, &month);

	for (i = 1990; i < year; i++) {
		if ((i%4==0 && i%100!=0) || (i%400==0)) {
			days = days + 366;
		} else {
			days = days + 365;
		}
	}
	for (i = 1; i < month; i++) {
		switch (i) {
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				days += 31; break;
			case 4:
			case 6:
			case 9:
			case 11:
				days += 30; break;
			case 2:
				if (year%4==0&&year%100!=0||year%400==0) {
					days += 29;
				}else {
					days += 28;
				}
				break;
			//days+=(year%4==0&&year%100!=0||year%400==0) ? 366:365;
			default:
				break;
		}
	}
	firstday = (days+1) % 7;

	switch (month) {
		case 1:case 3:case 5:case 7:case 8:case 10:case 12:
			monthday = 31;
			break;
		case 4:case 6:case 9:case 11:
			monthday = 30;
			break;
		case 2:
			monthday = (year%4==0&&year%100!=0||year%400==0) ? 29:28;
			break;
		default:
			break;
	}

	printf("    %d月%d\n", month, year);
	printf("日 一 二 三 四 五 六\n");	
	for (i = 0; i < firstday; i++) {
		printf("   ");
	}
	for (i = 1; i <= monthday; i++) {
		printf("%2d%c", i, (i+firstday)%7==0?'\n':' ');
	}

	printf("\n");

	return 0;
}
