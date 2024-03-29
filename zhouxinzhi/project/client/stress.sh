#!/bin/bash

i=1
MAX=100

while [ $i -le $MAX ]
do
	./client.o -i 127.0.0.1 -p 8989 -s 10 -d 0 &
	i=`expr $i + 1`
done
