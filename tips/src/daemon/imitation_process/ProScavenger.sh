#!/bin/sh
#kill -9 $(ps -ef | grep -v 'mysql' | grep -v "grep" | grep $1 | awk  '{print $2}')
kill -9 $(ps -ef | grep -v 'mysql' | grep -v 'grep' | grep -v 'tipsdaemon' | grep $1 | awk  '{print $2}')
