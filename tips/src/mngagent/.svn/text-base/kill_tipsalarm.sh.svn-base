#!/bin/sh
tips_name="tipsalarm"
for state in $tips_name
do
	tips_id=`ps -ef | grep $state | grep -v "grep" | awk '{print $2}'`
	if [ "$tips_id" ]; then
		kill -9 $tips_id
	fi
done
