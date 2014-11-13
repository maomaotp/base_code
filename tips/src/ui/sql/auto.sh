#!/bin/sh
g++ main.cpp sqlhandler.cpp -I/usr/local/mysql/include -lmysqlclient -L/usr/local/mysql/lib -o mysqltest -Wl,-rpath,/usr/local/mysql/lib
