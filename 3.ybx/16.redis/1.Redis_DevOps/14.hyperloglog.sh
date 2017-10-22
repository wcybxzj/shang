#!/bin/bash

elements=""
key="2016_05_01:unique:ids"
for i in `seq 1 10000`
do
	elements="${elements} uuid-"${i}
	if [[ $((i%1000))  == 0 ]];
	then
		redis-cli pfadd ${key} ${elements}
		elements=""
	fi
done
