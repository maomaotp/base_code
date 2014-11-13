#!/bin/sh

tipsmanager=`ps -ef | grep tipsmanager | grep -v "grep" | awk '{print $2}'`
if [ "$tipsmanager" ] ; then
	kill -9 $tipsmanager
fi

