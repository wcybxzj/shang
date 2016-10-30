#!/bin/bash
for (( i = 0; i < 1000; i++ )); do
	echo 1111111111 >> 1.txt;
	echo 2222222222 >> 2.txt;
	echo 3333333333 >> 3.txt;
	echo 4444444444 >> 4.txt;
	echo 5555555555 >> 5.txt;
	echo 6666666666 >> 6.txt;
	echo 7777777777 >> 7.txt;
done

for (( i = 0; i < 1; i++ )); do
	echo this is 1 >> 1.txt;
	echo this is 2 >> 2.txt;
	echo this is 3 >> 3.txt;
	echo this is 4 >> 4.txt;
	echo this is 5 >> 5.txt;
	echo this is 6 >> 6.txt;
	echo this is 7 >> 7.txt;
done
