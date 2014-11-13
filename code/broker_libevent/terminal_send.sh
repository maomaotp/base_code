#!/bin/sh
for((i=0; i<5; i++))
do ((port=2+i)) 
	gnome-terminal -x "/home/liuq/code/libevent_use/client_send" 
	done
