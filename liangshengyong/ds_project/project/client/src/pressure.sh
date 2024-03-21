#!/bin/bash

max_bg_procs=10
counter=0

for i in {0..99}
do
	./socket_client -i 192.168.2.40 -p 9999 -n $i -m 1 &
	((counter++))
	if [[ $counter -ge $max_bg_procs ]]; then
		wait -n
		((counter--))
	fi
done

wait
