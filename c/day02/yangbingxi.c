#include <stdio.h>
#define YEAR 1990
#define MONTH 1
#define DAY 1

int leap(int a){
	if ((a%4 == 0 && a%100!=0)||a%400==0) {
		return 1;
	}else{
		return 0;
	}
}

int get_number(int y, int m, int d){
	int i, sum =0;
	int a[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	int b[12]={31,29,31,30,31,30,31,31,30,31,30,31};
	if (leap(y)) {
		for (i = 0; i < m-1; i++) {
			sum+=b[i];
		}
	}else{
		for (i = 0; i < m-1; i++) {
			sum+=a[i];
		}
	}
	sum++;
	return sum;
}

int get_days(int y, int m)
{
	int a[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	int b[12]={31,29,31,30,31,30,31,31,30,31,30,31};
	if (leap(y)) {
		return b[m-1];
	}else{
		return a[m-1];
	}
}

int main(int argc, const char *argv[])
{
	int i,j,y,m;
	scanf("%d%d",&y,&m);
	if (y < YEAR) {
		return 0;
	}

	int total_days = 0;
	for (i = 1990; i < y; i++) {
		if (leap(i)) {
			total_days += 366;
		}else{
			total_days += 365;
		}
	}

	int sum;
	sum = get_number(y, m, 1);
	total_days += sum;
	//printf("%d\n", total_days);

	int week;
	week = total_days%7;
	//printf("week %d\n", week);

	int days;
	days = get_days(y, m);
	//printf("%d\n", days);

	for (i = 1; i <= 7; i++) {
		printf("%-3d",i);
	}
	printf("\n");

	int row=0;
	for (i = 1; i < week; i++) {
		row++;
		printf("   ");
	}

	for (i = 1; i <= days; i++) {
		row++;
		printf("%-3d",i);
		if (row%7==0) {
			printf("\n");
		}
	}
	printf("\n");

	return 0;
}
