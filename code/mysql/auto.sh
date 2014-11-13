#!/bin/sh
gcc main.c sqlhandler.c -I/usr/local/mysql/include -lmysqlclient -L/usr/local/mysql/lib -o a.out -Wl,-rpath,/usr/local/mysql/lib
