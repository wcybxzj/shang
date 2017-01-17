#!/bin/bash

for i in {1..100} 
do 
{ 
	./2.fork; 
}&
done
wait

