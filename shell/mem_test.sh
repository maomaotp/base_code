#!/bin/bash
for((i=1;i<100;i++))
do
{
    ps aux|grep $1|grep -v grep|grep -v mysql >>$1_mem.log;
    echo now is the $i 
    echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~>> $1_mem.log
    sleep 30;
}
done;
