#!/bin/sh
#kill -9 $(ps -ef | grep -v 'mysql' | grep -v "grep" | grep -v 'daemon' | grep 'tips' | awk  '{print $2}')
killall tipscache
killall tipsstat
killall tipsmanager
killall tipsuser
killall tipsalarm
killall tipsauditor
killall tipsnetblock
