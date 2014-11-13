#!/bin/sh
kill -9 $(ps -ef | grep -v 'mysql' | grep -v "grep" | grep -v 'daemon' | grep 'tips' | awk  '{print $2}')
