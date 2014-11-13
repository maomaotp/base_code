#!/bin/sh
for((i=0; i<10; i++))
do ((port=2+i)) 
	gnome-terminal -x "/home/liuq/code/libevent_use/client" 
	sleep 2
	done
