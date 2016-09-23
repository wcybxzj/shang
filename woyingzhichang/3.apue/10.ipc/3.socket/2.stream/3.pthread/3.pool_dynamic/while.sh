#!/bin/bash

while true;                                                                                     
do
	{
	    (./client 127.0.0.1 &); 
	    sleep 1;
	}
done
wait

