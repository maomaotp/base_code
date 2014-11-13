#! /bin/sh
kill $(ps aux|grep tips|grep -v grep|grep -v mysql|awk '{print $2}')
